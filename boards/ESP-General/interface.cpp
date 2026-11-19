#include "core/powerSave.h"
#include <interface.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.i2c_bus = {(gpio_num_t)8, (gpio_num_t)46}; // sda, scl (Grove)
    viperConfigPins.rfTx = 8;
    viperConfigPins.rfRx = 46;
    viperConfigPins.irTx = 40;
    viperConfigPins.irRx = 46;
    viperConfigPins.rotation = 1;
    viperConfigPins.uart_bus = {(gpio_num_t)46, (gpio_num_t)8};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)46, (gpio_num_t)8};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)46, (gpio_num_t)8}; // rx, tx (CH9329)
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)10};
    viperConfigPins.PN532_bus = {(gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)10};
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)12, (gpio_num_t)13, (gpio_num_t)11, (gpio_num_t)10, (gpio_num_t)9, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2

    viperConfig.startupApp = "WebUI";
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {}

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
