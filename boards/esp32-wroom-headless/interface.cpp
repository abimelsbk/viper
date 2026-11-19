// Viper - headless ESP32-WROOM-32 board (no display, no buttons).
// Controlled entirely through the WebUI (started automatically in AP mode) or serial.
#include "core/powerSave.h"
#include <interface.h>

// Deselect NRF24/CC1101 on the shared SPI bus before anything else runs (before setup())
static void __attribute__((constructor)) _early_spi_deselect() {
    gpio_reset_pin(GPIO_NUM_15); // CC1101 / NRF24 CS
    gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_15, 1); // CS HIGH = deselected

    gpio_reset_pin(GPIO_NUM_4); // NRF24 CE
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_4, 0); // CE LOW = standby
}

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
**
** Default wiring (all changeable later in Config > Set Device pins, via the WebUI):
**   SPI bus (CC1101 / NRF24 / SD / RC522): SCK 18, MISO 19, MOSI 23
**     CC1101 + NRF24 CS 15, CC1101 GDO0 2, NRF24 CE 4, SD card CS 5
**   I2C (PN532, RFID2, Si4713...): SDA 21, SCL 22
**   UART devices (GPS, CH9329): RX 16, TX 17  (UART0 on 1/3 stays free for the USB CLI)
**   IR TX 26, IR RX 27 · RF433 TX 25, RX 33
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.rotation = 1;
    viperConfigPins.irTx = 26;
    viperConfigPins.irRx = 27;
    viperConfigPins.rfTx = 25;
    viperConfigPins.rfRx = 33;
    viperConfigPins.i2c_bus = {(gpio_num_t)21, (gpio_num_t)22}; // sda, scl
    viperConfigPins.uart_bus = {(gpio_num_t)16, (gpio_num_t)17};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)16, (gpio_num_t)17};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)16, (gpio_num_t)17}; // rx, tx (CH9329)
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)18, (gpio_num_t)19, (gpio_num_t)23, (gpio_num_t)15};
    viperConfigPins.PN532_bus = {(gpio_num_t)18, (gpio_num_t)19, (gpio_num_t)23, (gpio_num_t)15};
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)18, (gpio_num_t)19, (gpio_num_t)23, (gpio_num_t)15, (gpio_num_t)2
    }; // sck,miso,mosi,cs,gdo0
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)18, (gpio_num_t)19, (gpio_num_t)23, (gpio_num_t)15, (gpio_num_t)4
    }; // sck,miso,mosi,cs(ss),ce
    viperConfigPins.SDCARD_bus = {(gpio_num_t)18, (gpio_num_t)19, (gpio_num_t)23, (gpio_num_t)5};

    pinMode(viperConfigPins.CC1101_bus.cs, OUTPUT);
    digitalWrite(viperConfigPins.CC1101_bus.cs, HIGH);
    pinMode(viperConfigPins.NRF24_bus.io0, OUTPUT);
    digitalWrite(viperConfigPins.NRF24_bus.io0, LOW);

    // No screen: the web interface is the user interface. Started in AP mode at boot.
    viperConfig.startupApp = "WebUI";
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** No backlight on a headless board.
**********************************************************************/
void _setBrightness(uint8_t brightval) {}

/*********************************************************************
** Function: InputHandler
** No physical buttons; input arrives from the WebUI Navigator / serial "nav".
**********************************************************************/
void InputHandler(void) {}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
**********************************************************************/
void checkReboot() {}
