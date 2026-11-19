#include "core/bus_HAL.h"
#include "core/powerSave.h"
#include "core/utils.h"
#include "hal/bright/bright.h"
#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include "hal/inputs/touch.h"
#include <globals.h>
#include <interface.h>

#define SEL_BTN 16

#define BTN_ACT LOW
#define DW_BTN 14
#define UP_BTN 0

#ifdef USE_SD_MMC
#define PIN_SD_CMD 13
#define PIN_SD_CLK 11
#define PIN_SD_D0 12
#endif
#ifdef HAS_TOUCH
// CST820 (CST8xx family) over the default Wire object. Raw touch always reads through the old
// fixed touch.setRotation(1) pre-transform (a plain x/y swap): derived algebraically from
// composing it with InputHandler's old per-rotation remap block -- see src/hal/README.md.
static DeviceTouch touchCfg() {
    DeviceTouch cfg;
    cfg.pin_sda = 18;
    cfg.pin_scl = 17;
    cfg.pin_rst = 21;
    cfg.cst8xx_model = 1; // CST816/CST820/CST716
    // rotation:        0      1      2      3
    const bool swapXY[4] = {false, true, false, true};
    const bool mirrorX[4] = {true, false, false, true};
    const bool mirrorY[4] = {false, true, true, false};
    for (int i = 0; i < 4; i++) {
        cfg.SwapXY[i] = swapXY[i];
        cfg.MirrorX[i] = mirrorX[i];
        cfg.MirrorY[i] = mirrorY[i];
    }
    return cfg;
}
#endif

#if defined(T_DISPLAY_S3)

#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.mic_bus = {(gpio_num_t)39, (gpio_num_t)42, GPIO_NUM_NC, MIC_TYPE_PDM}; // clk,data,ws,type
    // GROVE_SDA/SCL, TXLED/RXLED, SERIAL_TX/RX and the shared SPI buses used to vary per
    // env via -D overrides (see lilygo-t-display-s3.ini history) depending on the
    // HAS_TOUCH / USE_SD_MMC pin-mux combination for this board; mirror those 4
    // combinations here now that the macros are gone.
#if defined(HAS_TOUCH) && defined(USE_SD_MMC)
    viperConfigPins.i2c_bus = {(gpio_num_t)18, (gpio_num_t)17}; // sda, scl (Grove)
    viperConfigPins.rfTx = 18;
    viperConfigPins.rfRx = 17;
    viperConfigPins.irTx = 10;
    viperConfigPins.irRx = 44;
    viperConfigPins.uart_bus = {(gpio_num_t)44, (gpio_num_t)16};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)44, (gpio_num_t)16};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)17, (gpio_num_t)18}; // rx, tx
#elif defined(HAS_TOUCH)
    viperConfigPins.i2c_bus = {(gpio_num_t)18, (gpio_num_t)17}; // sda, scl (Grove)
    viperConfigPins.sys_i2c = {(gpio_num_t)18, (gpio_num_t)17}; // sda, scl
    viperConfigPins.rfTx = 18;
    viperConfigPins.rfRx = 17;
    viperConfigPins.irTx = 3;
    viperConfigPins.irRx = 21;
    viperConfigPins.uart_bus = {(gpio_num_t)43, (gpio_num_t)44};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)17, (gpio_num_t)18};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)17, (gpio_num_t)18}; // rx, tx
#elif defined(USE_SD_MMC)
    viperConfigPins.i2c_bus = {(gpio_num_t)16, (gpio_num_t)21}; // sda, scl (Grove)
    viperConfigPins.rfTx = 16;
    viperConfigPins.rfRx = 21;
    viperConfigPins.irTx = 10;
    viperConfigPins.irRx = 44;
    viperConfigPins.uart_bus = {(gpio_num_t)16, (gpio_num_t)21};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)21, (gpio_num_t)16};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)21, (gpio_num_t)16}; // rx, tx
#else
    viperConfigPins.i2c_bus = {(gpio_num_t)44, (gpio_num_t)43}; // sda, scl (Grove)
    viperConfigPins.rfTx = 44;
    viperConfigPins.rfRx = 43;
    viperConfigPins.irTx = 17;
    viperConfigPins.irRx = 18;
    viperConfigPins.uart_bus = {(gpio_num_t)43, (gpio_num_t)44};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)43, (gpio_num_t)44};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)43, (gpio_num_t)44}; // rx, tx
#endif
    viperConfigPins.rotation = 3;

#if defined(USE_SD_MMC)
    viperConfigPins.SDCARD_bus = {(gpio_num_t)-1, (gpio_num_t)-1, (gpio_num_t)-1, (gpio_num_t)-1};
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)43, (gpio_num_t)2, (gpio_num_t)3, (gpio_num_t)1, (gpio_num_t)44, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2
#if defined(HAS_TOUCH)
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)43, (gpio_num_t)2, (gpio_num_t)3, (gpio_num_t)16, (gpio_num_t)44
    }; // sck,miso,mosi,cs(ss),ce
#else
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)43, (gpio_num_t)2, (gpio_num_t)3, (gpio_num_t)18, (gpio_num_t)17
    }; // sck,miso,mosi,cs(ss),ce
#endif
#if !defined(LITE_VERSION)
    viperConfigPins.W5500_bus = {
        (gpio_num_t)43, (gpio_num_t)2, (gpio_num_t)3, (gpio_num_t)-1, (gpio_num_t)-1, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,int,rst
#endif
#else
    viperConfigPins.SDCARD_bus = {
        (gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)1
    }; // sck,miso,mosi,cs
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)2, (gpio_num_t)21, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)10, (gpio_num_t)3
    }; // sck,miso,mosi,cs(ss),ce
#if !defined(LITE_VERSION)
    viperConfigPins.W5500_bus = {
        (gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)-1, (gpio_num_t)-1, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,int,rst
#endif
#endif
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)10};
    viperConfigPins.PN532_bus = {(gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)10};

#ifdef USE_SD_MMC
    SD.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0);
#endif

#ifdef HAS_TOUCH
    gpio_hold_dis((gpio_num_t)21); // PIN_TOUCH_RES
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH); // PIN_POWER_ON
    setSysI2CBus(&Wire);    // Touch lives on the default Wire object
    if (!hal_touch_init(touchCfg(), 0x15 /* CTS820_SLAVE_ADDRESS */)) {
        Serial.println("Touch IC not found");
    }
#endif
    // setup buttons
    pinMode(SEL_BTN, INPUT_PULLUP);
    // DW_BTN -> Next (click) / Sel (double click or hold)
    // UP_BTN -> Prev (click) / Esc (double click or hold)
    hal_buttons_init_2(DeviceButtons{DW_BTN, UP_BTN}, 600);

    // setup POWER pin required by the vendor
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    // Start with default IR, RF and RFID Configs, replace old
    viperConfigPins.rfModule = CC1101_SPI_MODULE;
    viperConfigPins.rfidModule = PN532_I2C_MODULE;

    Serial.begin(115200);
}
/***************************************************************************************
** Function name: _post_setup_gpio()
** Description:   second stage gpio setup after TFT is initialized
***************************************************************************************/
void _post_setup_gpio() {
    // Backlight control via PWM (must be after TFT init)
    hal_bright_attach(TFT_BL);
    hal_bright_set(TFT_BL, 100); // Full brightness initially
}
/*********************************************************************
**  Function: setBrightness
**  set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) { hal_bright_set(TFT_BL, brightval); }

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/

void InputHandler(void) {
    hal_buttons_poll_2();

    static unsigned long tm = 0;
    if (millis() - tm <= 200 && !LongPress) return;
#ifdef HAS_TOUCH
    ViperTouchPoint t;
    if (hal_touch_read(touchCfg(), t)) {
        tm = millis();
        if (!hal_touch_apply(t)) return;
    }
#endif
}

void powerOff() {
#ifdef T_DISPLAY_S3
    tft.fillScreen(viperConfig.bgColor);
    digitalWrite(PIN_POWER_ON, LOW);
    digitalWrite(TFT_BL, LOW);
    tft.writecommand(0x10);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SEL_BTN, BTN_ACT);
    esp_deep_sleep_start();
#endif
}

void checkReboot() {
#ifdef T_DISPLAY_S3
    int countDown = 0;
    /* Long press power off */
    if (digitalRead(UP_BTN) == BTN_ACT && digitalRead(DW_BTN) == BTN_ACT) {
        uint32_t time_count = millis();
        while (digitalRead(UP_BTN) == BTN_ACT && digitalRead(DW_BTN) == BTN_ACT) {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                if (countDown == 0) {
                    int textWidth = tft.textWidth("PWR OFF IN 3/3", 1);
                    tft.fillRect(tftWidth / 2 - textWidth / 2, 7, textWidth, 18, viperConfig.bgColor);
                }
                tft.setTextSize(1);
                tft.setTextColor(viperConfig.priColor, viperConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                if (countDown < 4)
                    tft.drawCentreString("PWR OFF IN " + String(countDown) + "/3", tftWidth / 2, 12, 1);
                else {
                    tft.fillScreen(viperConfig.bgColor);
                    while (digitalRead(UP_BTN) == BTN_ACT || digitalRead(DW_BTN) == BTN_ACT);
                    delay(200);
                    powerOff();
                }
                delay(10);
            }
        }

        // Clear text after releasing the button
        delay(30);
        if (millis() - time_count > 500) {
            tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), viperConfig.bgColor);
            drawStatusBar();
        }
    }
#endif
}
