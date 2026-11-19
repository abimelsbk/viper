plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

// Release signing comes from environment variables (set by CI from GitHub secrets).
// Without them, the release build falls back to the debug key so it still installs.
val keystorePath: String? = System.getenv("VIPER_KEYSTORE_PATH")

android {
    namespace = "io.github.abimelsbk.viper"
    compileSdk = 35

    defaultConfig {
        applicationId = "io.github.abimelsbk.viper"
        minSdk = 26
        targetSdk = 35
        versionCode = (System.getenv("VIPER_VERSION_CODE") ?: "1").toInt()
        versionName = System.getenv("VIPER_VERSION_NAME") ?: "1.17"
    }

    signingConfigs {
        if (keystorePath != null) {
            create("release") {
                storeFile = file(keystorePath)
                storePassword = System.getenv("VIPER_KEYSTORE_PASSWORD")
                keyAlias = System.getenv("VIPER_KEY_ALIAS")
                keyPassword = System.getenv("VIPER_KEY_PASSWORD")
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            signingConfig = if (keystorePath != null) signingConfigs.getByName("release")
                            else signingConfigs.getByName("debug")
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }
}

dependencies {
    implementation("androidx.core:core-ktx:1.15.0")
    implementation("androidx.activity:activity-ktx:1.9.3")
}
