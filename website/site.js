// Shared device list (also used by flasher.html)
// cols: CC1101, NRF24, NFC, Mic, BadUSB, Speaker   ✅ built in · ➕ external module · ❌ none · — unverified
window.VIPER_DEVICES = [
  { name: "M5Stack CoreS3 / CoreS3 SE", env: "m5stack-cores3", chip: "ESP32-S3", f: ["➕", "➕", "➕", "✅", "✅", "✅"] },
  { name: "M5Stack Cardputer / ADV", env: "m5stack-cardputer", chip: "ESP32-S3", f: ["➕", "➕", "➕", "✅", "✅", "✅"] },
  { name: "M5StickS3", env: "m5stack-sticks3", chip: "ESP32-S3", f: ["➕", "➕", "➕", "✅", "✅", "✅"] },
  { name: "M5StickC Plus2", env: "m5stack-cplus2", chip: "ESP32", f: ["➕", "➕", "➕", "✅", "➕", "buzzer"] },
  { name: "M5StickC Plus 1.1", env: "m5stack-cplus1_1", chip: "ESP32", f: ["➕", "➕", "➕", "✅", "➕", "buzzer"] },
  { name: "M5Stack Core2", env: "m5stack-core2", chip: "ESP32", f: ["➕", "➕", "➕", "✅", "❌", "✅"] },
  { name: "M5Stack Core (16MB)", env: "m5stack-core16mb", chip: "ESP32", f: ["➕", "➕", "➕", "❌", "❌", "✅"] },
  { name: "M5Stack Core (4MB)", env: "m5stack-core4mb", chip: "ESP32", f: ["➕", "➕", "➕", "❌", "❌", "✅"] },
  { name: "M5Stack AtomS3", env: "m5stack-atoms3", chip: "ESP32-S3", f: ["➕", "➕", "➕", "❌", "✅", "❌"] },
  { name: "M5Stack DinMeter", env: "m5stack-dinmeter", chip: "ESP32-S3", f: ["➕", "➕", "➕", "❌", "✅", "buzzer"] },
  { name: "M5Stack StackChan", env: "m5stack-stackchan", chip: "ESP32-S3", f: ["➕", "➕", "➕", "✅", "✅", "✅"] },
  { name: "M5Stack Tab5", env: "m5stack-tab5", chip: "ESP32-P4", f: ["➕", "➕", "➕", "—", "—", "—"] },
  { name: "Arduino Nesso N1", env: "arduino-nesso-n1", chip: "ESP32-C6", f: ["➕", "➕", "➕", "—", "—", "—"] },
  { name: "ESP32 DevKit V1 / WROOM (no screen, WebUI)", env: "esp32-wroom-headless", chip: "ESP32", f: ["➕", "➕", "➕", "❌", "➕ CH9329", "❌"] },
  { name: "ESP32 WROOM + ILI9341 screen", env: "esp-wroom32-ili9341", chip: "ESP32", f: ["➕", "➕", "➕", "❌", "➕ CH9329", "❌"] },
  { name: "ESP32 WROOM + ST7789 screen", env: "esp-wroom32-st7789", chip: "ESP32", f: ["➕", "➕", "➕", "❌", "➕ CH9329", "❌"] },
];

(function () {
  const rows = document.getElementById("deviceRows");
  if (rows) {
    rows.innerHTML = window.VIPER_DEVICES.map(
      (d) =>
        `<tr><td>${d.name}</td>${d.f.map((c) => `<td>${c}</td>`).join("")}<td><code>${d.env}</code></td></tr>`
    ).join("");
  }

  // mobile nav
  const t = document.querySelector(".nav-toggle");
  const links = document.querySelector(".nav-links");
  if (t && links) {
    t.addEventListener("click", () => {
      const open = links.classList.toggle("open");
      t.setAttribute("aria-expanded", String(open));
    });
    links.addEventListener("click", (e) => {
      if (e.target.tagName === "A") links.classList.remove("open");
    });
  }

  // hero mini-device: cycle through menu items like the real carousel
  const item = document.getElementById("demoItem");
  const count = document.getElementById("demoCount");
  const bar = document.querySelector(".rule i");
  if (item && count && bar && !matchMedia("(prefers-reduced-motion: reduce)").matches) {
    const menu = ["WIFI", "BLE", "RF", "RFID", "IR", "FM", "NRF24", "SCRIPTS", "GPS", "FILES", "OTHERS", "CLOCK", "CONNECT", "CONFIG"];
    let i = 0;
    setInterval(() => {
      i = (i + 1) % menu.length;
      item.textContent = menu[i];
      count.textContent = String(i + 1).padStart(2, "0") + "/" + menu.length;
      bar.style.left = (82 * i) / (menu.length - 1) + "%";
    }, 1400);
  }
})();
