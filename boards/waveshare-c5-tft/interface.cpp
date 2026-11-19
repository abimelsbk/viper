#include "core/powerSave.h"
#include "core/utils.h"
#include "hal/bright/bright.h"
#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include <interface.h>

#define SEL_BTN 28

#define DW_BTN 1
#define L_BTN 14
#define R_BTN 13
#define UP_BTN 0

static DeviceButtons buttonsCfg() { return DeviceButtons{L_BTN, R_BTN, UP_BTN, DW_BTN, SEL_BTN}; }

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.i2c_bus = {(gpio_num_t)4, (gpio_num_t)5}; // sda, scl (Grove)
    viperConfigPins.rfTx = 4;
    viperConfigPins.rfRx = 5;
    viperConfigPins.irTx = 7;
    viperConfigPins.irRx = 26;
    viperConfigPins.rotation = 1;
    viperConfigPins.uart_bus = {(gpio_num_t)12, (gpio_num_t)11}; // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)4, (gpio_num_t)5};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)4, (gpio_num_t)5}; // rx, tx
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)10, (gpio_num_t)9, (gpio_num_t)8, (gpio_num_t)3};
    viperConfigPins.PN532_bus = {(gpio_num_t)10, (gpio_num_t)9, (gpio_num_t)8, (gpio_num_t)3};
    // CC1101/NRF24/SDCARD/W5500 share the main SPI bus (sck=10, miso=9, mosi=8)
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)10, (gpio_num_t)9, (gpio_num_t)8, (gpio_num_t)3, (gpio_num_t)2, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)10, (gpio_num_t)9, (gpio_num_t)8, (gpio_num_t)3, (gpio_num_t)2
    }; // sck,miso,mosi,cs(ss),ce
    viperConfigPins.SDCARD_bus = {
        (gpio_num_t)10, (gpio_num_t)9, (gpio_num_t)8, (gpio_num_t)6
    }; // sck,miso,mosi,cs
#if !defined(LITE_VERSION)
    viperConfigPins.W5500_bus = {
        (gpio_num_t)10, (gpio_num_t)9, (gpio_num_t)8, (gpio_num_t)3, (gpio_num_t)2, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,int,rst
#endif

    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TFT_MOSI, OUTPUT);
    digitalWrite(TFT_MOSI, HIGH);
    pinMode(TFT_SCLK, OUTPUT);

    pinMode(TFT_RST, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_DC, HIGH);

#ifdef HAS_5_BUTTONS
    hal_buttons_init(buttonsCfg(), 5);
#endif

    pinMode(viperConfigPins.NRF24_bus.cs, OUTPUT);
    pinMode(viperConfigPins.CC1101_bus.cs, OUTPUT);
    pinMode(viperConfigPins.SDCARD_bus.cs, OUTPUT);
    pinMode(TFT_CS, OUTPUT);

    digitalWrite(viperConfigPins.NRF24_bus.cs, HIGH);
    digitalWrite(viperConfigPins.CC1101_bus.cs, HIGH);
    digitalWrite(viperConfigPins.SDCARD_bus.cs, HIGH);
    digitalWrite(TFT_CS, HIGH);
#if !defined(LITE_VERSION)
    pinMode(viperConfigPins.W5500_bus.cs, OUTPUT);
    digitalWrite(viperConfigPins.W5500_bus.cs, HIGH);
#endif
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
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) { hal_bright_set(TFT_BL, brightval); }

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) { hal_buttons_poll_5(buttonsCfg()); }

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turnoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
