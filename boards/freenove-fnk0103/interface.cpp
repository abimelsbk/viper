#include "core/powerSave.h"
#include "core/utils.h"
#include "hal/device.h"
#include "hal/inputs/touch.h"
#include <CYD28_TouchscreenR.h>
#include <interface.h>

extern CYD28_TouchR touch; // defined by hal/inputs/touch.cpp

static DeviceTouch touchCfg() {
    DeviceTouch cfg;
    // The XPT2046 reports landscape (rotation 1) coordinates
    // rotation:        0      1      2      3
    const bool swapXY[4] = {true, false, true, false};
    const bool mirrorX[4] = {true, false, false, true};
    const bool mirrorY[4] = {false, false, true, true};
    for (int i = 0; i < 4; i++) {
        cfg.SwapXY[i] = swapXY[i];
        cfg.MirrorX[i] = mirrorX[i];
        cfg.MirrorY[i] = mirrorY[i];
    }
    return cfg;
}

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.i2c_bus = {(gpio_num_t)22, (gpio_num_t)21}; // sda, scl (Grove)
    viperConfigPins.rfTx = 22;
    viperConfigPins.rfRx = 21;
    viperConfigPins.irTx = -1;
    viperConfigPins.irRx = 21;
    viperConfigPins.rotation = 1;
    viperConfigPins.uart_bus = {(gpio_num_t)3, (gpio_num_t)1};     // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)3, (gpio_num_t)1};      // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)21, (gpio_num_t)22}; // rx, tx (Grove)
    viperConfigPins.SDCARD_bus = {
        (gpio_num_t)-1, (gpio_num_t)-1, (gpio_num_t)-1, (gpio_num_t)-1
    }; // sck,miso,mosi,cs (no SD card slot)
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)25, (gpio_num_t)39, (gpio_num_t)32, (gpio_num_t)33};
    viperConfigPins.PN532_bus = {(gpio_num_t)25, (gpio_num_t)39, (gpio_num_t)32, (gpio_num_t)33};

    viperConfig.colorInverted = 0;
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Description:   second stage gpio setup to make touch work
***************************************************************************************/
void _post_setup_gpio() {
    if (!hal_touch_init(touchCfg(), 0, TFT_MOSI == CYD28_TouchR_MOSI)) {
        Serial.println("Touch IC not Started");
        log_i("Touch IC not Started");
    } else Serial.println("Touch IC Started");
}

/*********************************************************************
** Function: setBrightness
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (brightval > 5) digitalWrite(TFT_BL, HIGH);
    else digitalWrite(TFT_BL, LOW);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = millis();
    if (millis() - tm > 300 || LongPress) {
        ViperTouchPoint t;
        if (hal_touch_read(touchCfg(), t)) {
            tm = millis();
            hal_touch_apply(t);
        } else touchPoint.pressed = false;
    }
}

/*********************************************************************
** Function: powerOff
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** Btn logic to turn off the device
**********************************************************************/
void checkReboot() {}
