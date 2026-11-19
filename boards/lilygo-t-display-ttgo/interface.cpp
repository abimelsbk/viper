#include "core/powerSave.h"
#include "core/utils.h"
#include "hal/bright/bright.h"
#include "hal/device.h"
#include "hal/inputs/buttons.h"

#include <globals.h>
#include <interface.h>

#define ADC_EN 14
#define BTN_ACT LOW
#define DW_BTN 35
#define UP_BTN 0

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.i2c_bus = {(gpio_num_t)21, (gpio_num_t)22}; // sda, scl (Grove)
    viperConfigPins.rfTx = 21;
    viperConfigPins.rfRx = 22;
    viperConfigPins.irTx = 2;
    viperConfigPins.irRx = 15;
    viperConfigPins.rotation = 3;
    viperConfigPins.uart_bus = {(gpio_num_t)13, (gpio_num_t)12};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)13, (gpio_num_t)12};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)13, (gpio_num_t)12}; // rx, tx
    viperConfigPins.SDCARD_bus = {
        (gpio_num_t)25, (gpio_num_t)27, (gpio_num_t)26, (gpio_num_t)33
    }; // sck,miso,mosi,cs
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)25, (gpio_num_t)27, (gpio_num_t)26, (gpio_num_t)33};
    viperConfigPins.PN532_bus = {(gpio_num_t)25, (gpio_num_t)27, (gpio_num_t)26, (gpio_num_t)33};
    // CC1101/NRF24/W5500 share the main SPI bus (sck=25, miso=27, mosi=26)
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)25, (gpio_num_t)27, (gpio_num_t)26, (gpio_num_t)32, (gpio_num_t)39, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)25, (gpio_num_t)27, (gpio_num_t)26, (gpio_num_t)38, (gpio_num_t)37
    }; // sck,miso,mosi,cs(ss),ce
#if !defined(LITE_VERSION)
    viperConfigPins.W5500_bus = {
        (gpio_num_t)25, (gpio_num_t)27, (gpio_num_t)26, (gpio_num_t)38, (gpio_num_t)37, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,int,rst
#endif

    // setup buttons
    // DW_BTN -> Next (click) / Sel (double click or hold)
    // UP_BTN -> Prev (click) / Esc (double click or hold)
    hal_buttons_init_2(DeviceButtons{DW_BTN, UP_BTN}, 600);

    // setup POWER pin required by the vendor
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);

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

void InputHandler(void) { hal_buttons_poll_2(); }

void powerOff() {
    tft.fillScreen(viperConfig.bgColor);
    digitalWrite(TFT_BL, LOW);
    tft.writecommand(0x10);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)DW_BTN, BTN_ACT);
    esp_deep_sleep_start();
}
