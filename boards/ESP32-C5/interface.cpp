#include "core/powerSave.h"
#include <interface.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.i2c_bus = {(gpio_num_t)0, (gpio_num_t)1}; // sda, scl (Grove)
    viperConfigPins.rfTx = 0;
    viperConfigPins.rfRx = 1;
    viperConfigPins.irTx = 27;
    viperConfigPins.irRx = 1;
    viperConfigPins.rotation = 1;
    viperConfigPins.badusb_bus = {(gpio_num_t)4, (gpio_num_t)5}; // rx, tx (CH9329)

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
