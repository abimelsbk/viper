package io.github.abimelsbk.viper

import android.Manifest
import android.annotation.SuppressLint
import android.content.ContentValues
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.net.Uri
import android.net.wifi.WifiNetworkSpecifier
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.os.Handler
import android.os.Looper
import android.provider.MediaStore
import android.provider.Settings
import android.text.InputType
import android.view.View
import android.webkit.CookieManager
import android.webkit.ValueCallback
import android.webkit.WebChromeClient
import android.webkit.WebResourceError
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.OnBackPressedCallback
import androidx.activity.result.contract.ActivityResultContracts
import java.io.File
import java.io.FileOutputStream
import java.io.OutputStream
import java.net.HttpURLConnection
import java.net.URL
import java.net.URLDecoder
import kotlin.concurrent.thread

/**
 * Viper companion app.
 *
 * Joins the device's own access point (ViperNet by default), binds the app's traffic to that
 * network (it has no internet, so Android would otherwise route requests over mobile data),
 * and shows Viper's web interface with working file upload and download.
 */
class MainActivity : ComponentActivity() {

    private lateinit var web: WebView
    private lateinit var connectPanel: View
    private lateinit var statusText: TextView
    private lateinit var connectHint: TextView

    private val prefs by lazy { getSharedPreferences("viper", Context.MODE_PRIVATE) }
    private val cm by lazy { getSystemService(ConnectivityManager::class.java) }
    private val main = Handler(Looper.getMainLooper())

    private var viperNetCallback: ConnectivityManager.NetworkCallback? = null
    private var boundNetwork: Network? = null
    private var fileCallback: ValueCallback<Array<Uri>>? = null

    private val host get() = prefs.getString("host", DEFAULT_HOST)!!.trim()
    private val ssid get() = prefs.getString("ssid", DEFAULT_SSID)!!
    private val pass get() = prefs.getString("pass", DEFAULT_PASS)!!
    private val baseUrl get() = "http://$host/"

    private val pickFiles =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { res ->
            val cb = fileCallback ?: return@registerForActivityResult
            fileCallback = null
            cb.onReceiveValue(WebChromeClient.FileChooserParams.parseResult(res.resultCode, res.data))
        }

    private val askPermissions =
        registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) {
            joinViperNet() // try regardless; the system dialog explains if it can't
        }

    // ------------------------------------------------------------------ lifecycle

    @SuppressLint("SetJavaScriptEnabled")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        web = findViewById(R.id.web)
        connectPanel = findViewById(R.id.connect_panel)
        statusText = findViewById(R.id.status)
        connectHint = findViewById(R.id.connect_hint)

        findViewById<View>(R.id.btn_join).setOnClickListener { requestJoin() }
        findViewById<View>(R.id.btn_already).setOnClickListener { useCurrentWifi() }
        findViewById<View>(R.id.btn_reload).setOnClickListener { checkAndLoad() }
        findViewById<View>(R.id.btn_settings).setOnClickListener { showSettings() }

        with(web.settings) {
            javaScriptEnabled = true
            domStorageEnabled = true
            allowFileAccess = false
            builtInZoomControls = false
            useWideViewPort = true
            loadWithOverviewMode = true
        }
        CookieManager.getInstance().setAcceptCookie(true)

        web.webViewClient = object : WebViewClient() {
            override fun shouldOverrideUrlLoading(view: WebView, req: WebResourceRequest): Boolean {
                val url = req.url
                if (url.host == host) return false
                // Links to docs, GitHub etc. open in the normal browser.
                runCatching { startActivity(Intent(Intent.ACTION_VIEW, url)) }
                return true
            }

            override fun onPageFinished(view: WebView, url: String) {
                setStatus(getString(R.string.status_connected, host), connected = true)
            }

            override fun onReceivedError(view: WebView, req: WebResourceRequest, err: WebResourceError) {
                if (req.isForMainFrame) showConnectPanel(getString(R.string.status_unreachable, host))
            }
        }

        web.webChromeClient = object : WebChromeClient() {
            override fun onShowFileChooser(
                view: WebView, callback: ValueCallback<Array<Uri>>, params: FileChooserParams
            ): Boolean {
                fileCallback?.onReceiveValue(null)
                fileCallback = callback
                return try {
                    pickFiles.launch(params.createIntent().apply {
                        putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true)
                    })
                    true
                } catch (e: Exception) {
                    fileCallback = null
                    false
                }
            }
        }

        web.setDownloadListener { url, _, contentDisposition, mime, _ ->
            download(url, contentDisposition, mime)
        }

        onBackPressedDispatcher.addCallback(this, object : OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                if (connectPanel.visibility != View.VISIBLE && web.canGoBack()) web.goBack()
                else finish()
            }
        })

        if (savedInstanceState != null) web.restoreState(savedInstanceState)
        checkAndLoad()
    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)
        web.saveState(outState)
    }

    override fun onDestroy() {
        releaseViperNet()
        super.onDestroy()
    }

    // ------------------------------------------------------------------ connection

    /** Probe the device; show the web UI if it answers, otherwise the connect panel. */
    private fun checkAndLoad() {
        setStatus(getString(R.string.status_checking, host), connected = false)
        thread {
            val ok = runCatching {
                val c = openConnection(baseUrl)
                c.connectTimeout = 2500
                c.readTimeout = 2500
                c.requestMethod = "GET"
                val code = c.responseCode
                c.disconnect()
                code in 200..499
            }.getOrDefault(false)
            main.post {
                if (ok) {
                    connectPanel.visibility = View.GONE
                    web.visibility = View.VISIBLE
                    if (web.url == null || !web.url!!.contains(host)) web.loadUrl(baseUrl) else web.reload()
                } else {
                    showConnectPanel(getString(R.string.status_unreachable, host))
                }
            }
        }
    }

    private fun requestJoin() {
        val needed = buildList {
            if (Build.VERSION.SDK_INT >= 33) add(Manifest.permission.NEARBY_WIFI_DEVICES)
            else add(Manifest.permission.ACCESS_FINE_LOCATION)
        }.filter { checkSelfPermission(it) != PackageManager.PERMISSION_GRANTED }
        if (needed.isEmpty()) joinViperNet() else askPermissions.launch(needed.toTypedArray())
    }

    /** Ask Android to join ViperNet and route this app's traffic through it. */
    private fun joinViperNet() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            Toast.makeText(this, getString(R.string.join_manually, ssid), Toast.LENGTH_LONG).show()
            startActivity(Intent(Settings.ACTION_WIFI_SETTINGS))
            return
        }
        releaseViperNet()
        setStatus(getString(R.string.status_joining, ssid), connected = false)

        val spec = WifiNetworkSpecifier.Builder().setSsid(ssid).apply {
            if (pass.isNotEmpty()) setWpa2Passphrase(pass)
        }.build()
        val request = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
            .removeCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
            .setNetworkSpecifier(spec)
            .build()

        val cb = object : ConnectivityManager.NetworkCallback() {
            override fun onAvailable(network: Network) {
                cm.bindProcessToNetwork(network)
                boundNetwork = network
                main.post { checkAndLoad() }
            }

            override fun onUnavailable() {
                main.post { showConnectPanel(getString(R.string.status_join_failed, ssid)) }
            }

            override fun onLost(network: Network) {
                if (network == boundNetwork) {
                    cm.bindProcessToNetwork(null)
                    boundNetwork = null
                    main.post { showConnectPanel(getString(R.string.status_lost, ssid)) }
                }
            }
        }
        viperNetCallback = cb
        cm.requestNetwork(request, cb, 30_000)
    }

    /** The phone is already on ViperNet (or Viper is on your home Wi-Fi): pin traffic to Wi-Fi. */
    private fun useCurrentWifi() {
        val wifi = cm.allNetworks.firstOrNull {
            cm.getNetworkCapabilities(it)?.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) == true
        }
        if (wifi == null) {
            Toast.makeText(this, R.string.no_wifi, Toast.LENGTH_LONG).show()
            return
        }
        cm.bindProcessToNetwork(wifi)
        boundNetwork = wifi
        checkAndLoad()
    }

    private fun releaseViperNet() {
        viperNetCallback?.let { runCatching { cm.unregisterNetworkCallback(it) } }
        viperNetCallback = null
        cm.bindProcessToNetwork(null)
        boundNetwork = null
    }

    private fun openConnection(url: String): HttpURLConnection =
        (boundNetwork?.openConnection(URL(url)) ?: URL(url).openConnection()) as HttpURLConnection

    // ------------------------------------------------------------------ downloads

    /** The web UI's download links need the login cookie and the ViperNet route, so fetch in-app. */
    private fun download(url: String, contentDisposition: String?, mime: String?) {
        val name = fileNameFrom(url, contentDisposition)
        Toast.makeText(this, getString(R.string.downloading, name), Toast.LENGTH_SHORT).show()
        thread {
            val result = runCatching {
                val c = openConnection(url)
                CookieManager.getInstance().getCookie(url)?.let { c.setRequestProperty("Cookie", it) }
                c.connectTimeout = 5000
                c.readTimeout = 30000
                if (c.responseCode !in 200..299) error("HTTP ${c.responseCode}")
                c.inputStream.use { input -> openDownloadTarget(name, mime).use { out -> input.copyTo(out) } }
                c.disconnect()
            }
            main.post {
                val msg = if (result.isSuccess) getString(R.string.downloaded, name)
                          else getString(R.string.download_failed, result.exceptionOrNull()?.message ?: "")
                Toast.makeText(this, msg, Toast.LENGTH_LONG).show()
            }
        }
    }

    private fun openDownloadTarget(name: String, mime: String?): OutputStream {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            val values = ContentValues().apply {
                put(MediaStore.Downloads.DISPLAY_NAME, name)
                put(MediaStore.Downloads.MIME_TYPE, mime ?: "application/octet-stream")
                put(MediaStore.Downloads.RELATIVE_PATH, Environment.DIRECTORY_DOWNLOADS + "/Viper")
            }
            val uri = contentResolver.insert(MediaStore.Downloads.EXTERNAL_CONTENT_URI, values)
                ?: error("cannot create file")
            return contentResolver.openOutputStream(uri) ?: error("cannot open file")
        }
        val dir = File(getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS), "Viper").apply { mkdirs() }
        return FileOutputStream(File(dir, name))
    }

    private fun fileNameFrom(url: String, contentDisposition: String?): String {
        // Viper links look like /file?fs=SD&name=%2FViperIR%2Ftv.ir&action=download
        Uri.parse(url).getQueryParameter("name")?.let { raw ->
            val decoded = runCatching { URLDecoder.decode(raw, "UTF-8") }.getOrDefault(raw)
            decoded.substringAfterLast('/').takeIf { it.isNotBlank() }?.let { return it }
        }
        contentDisposition?.let { cd ->
            Regex("filename=\"?([^\";]+)\"?").find(cd)?.groupValues?.get(1)?.let { return it }
        }
        return "viper-download.bin"
    }

    // ------------------------------------------------------------------ UI

    private fun showConnectPanel(status: String) {
        setStatus(status, connected = false)
        connectHint.text = getString(R.string.connect_hint, ssid, host)
        connectPanel.visibility = View.VISIBLE
        web.visibility = View.GONE
    }

    private fun setStatus(text: String, connected: Boolean) {
        statusText.text = text
        statusText.setTextColor(getColor(if (connected) R.color.venom else R.color.muted))
    }

    private fun showSettings() {
        val pad = (16 * resources.displayMetrics.density).toInt()
        fun field(label: String, value: String, password: Boolean = false) = EditText(this).apply {
            hint = label
            setText(value)
            setSingleLine()
            if (password) inputType = InputType.TYPE_CLASS_TEXT or InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
        }
        val ssidField = field(getString(R.string.setting_ssid), ssid)
        val passField = field(getString(R.string.setting_pass), pass, password = true)
        val hostField = field(getString(R.string.setting_host), host)
        val box = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(pad, pad / 2, pad, 0)
            fun label(t: String) = addView(TextView(context).apply { text = t; setPadding(0, pad / 2, 0, 0) })
            label(getString(R.string.setting_ssid)); addView(ssidField)
            label(getString(R.string.setting_pass)); addView(passField)
            label(getString(R.string.setting_host)); addView(hostField)
        }
        android.app.AlertDialog.Builder(this, android.R.style.Theme_DeviceDefault_Dialog_Alert)
            .setTitle(R.string.settings)
            .setView(box)
            .setPositiveButton(R.string.save) { _, _ ->
                prefs.edit()
                    .putString("ssid", ssidField.text.toString().ifBlank { DEFAULT_SSID })
                    .putString("pass", passField.text.toString())
                    .putString("host", hostField.text.toString().ifBlank { DEFAULT_HOST })
                    .apply()
                checkAndLoad()
            }
            .setNeutralButton(R.string.reset) { _, _ ->
                prefs.edit().clear().apply()
                checkAndLoad()
            }
            .setNegativeButton(android.R.string.cancel, null)
            .show()
    }

    companion object {
        const val DEFAULT_HOST = "172.0.0.1"
        const val DEFAULT_SSID = "ViperNet"
        const val DEFAULT_PASS = "vipernet"
    }
}
