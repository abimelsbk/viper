#include "core/powerSave.h"
#include "hal/bright/bright.h"
#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include <interface.h>

#define SEL_BTN 1

#define DW_BTN 2
#define UP_BTN 3

static DeviceButtons buttonsCfg() { return DeviceButtons{UP_BTN, DW_BTN, SEL_BTN}; }

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/

void _setup_gpio() {
    viperConfigPins.buzzer = 47;
    viperConfigPins.i2c_bus = {(gpio_num_t)4, (gpio_num_t)5}; // sda, scl (Grove)
    viperConfigPins.rfTx = 4;
    viperConfigPins.rfRx = 5;
    viperConfigPins.irTx = 5;
    viperConfigPins.irRx = 4;
    viperConfigPins.rotation = 1;
    viperConfigPins.uart_bus = {(gpio_num_t)5, (gpio_num_t)4};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)5, (gpio_num_t)4};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)5, (gpio_num_t)4}; // rx, tx (CH9329)
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)10, (gpio_num_t)8, (gpio_num_t)11, (gpio_num_t)6};
    viperConfigPins.PN532_bus = {(gpio_num_t)10, (gpio_num_t)8, (gpio_num_t)11, (gpio_num_t)6};
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)10, (gpio_num_t)8, (gpio_num_t)11, (gpio_num_t)17, (gpio_num_t)16, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)10, (gpio_num_t)8, (gpio_num_t)11, (gpio_num_t)18, (gpio_num_t)21
    }; // sck,miso,mosi,cs(ss),ce
    viperConfigPins.SDCARD_bus = {
        (gpio_num_t)10, (gpio_num_t)8, (gpio_num_t)11, (gpio_num_t)7
    }; // sck,miso,mosi,cs

    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TFT_MOSI, OUTPUT);
    digitalWrite(TFT_MOSI, HIGH);
    pinMode(TFT_SCLK, OUTPUT);

    pinMode(TFT_RST, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_DC, HIGH);

    hal_buttons_init(buttonsCfg(), 3);

    pinMode(viperConfigPins.NRF24_bus.cs, OUTPUT);
    pinMode(viperConfigPins.CC1101_bus.cs, OUTPUT);
    pinMode(viperConfigPins.SDCARD_bus.cs, OUTPUT);
    pinMode(TFT_CS, OUTPUT);

    digitalWrite(viperConfigPins.NRF24_bus.cs, HIGH);
    digitalWrite(viperConfigPins.CC1101_bus.cs, HIGH);
    digitalWrite(viperConfigPins.SDCARD_bus.cs, HIGH);
    digitalWrite(TFT_CS, HIGH);
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
void InputHandler(void) { hal_buttons_poll_3(buttonsCfg()); }

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
