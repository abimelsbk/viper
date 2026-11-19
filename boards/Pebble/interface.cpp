#include "core/bus_HAL.h"
#include "core/i2c_finder.h"
#include "core/powerSave.h"
#include "core/utils.h"
#include "hal/bright/bright.h"
#include <Wire.h>
#include <globals.h>
#include <interface.h>

#define EXPANDER_INT_PIN 28

// Interrupt flag from expander
volatile bool expanderInterrupt = false;

void IRAM_ATTR expanderISR() { expanderInterrupt = true; }

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    viperConfigPins.i2c_bus = {(gpio_num_t)4, (gpio_num_t)5}; // sda, scl (Grove)
    // The IO expander (buttons, peripheral power) sits on these same Grove pins: declaring them
    // as sys_i2c makes the I2C tools/modules share this Wire (the C5's only controller) instead of
    // re-pinning it and ending it on release, which took the expander down until reboot.
    viperConfigPins.sys_i2c = {(gpio_num_t)4, (gpio_num_t)5}; // sda, scl
    viperConfigPins.rfTx = -1;
    viperConfigPins.rfRx = -1;
    viperConfigPins.irTx = 25;
    viperConfigPins.irRx = 26;
    viperConfigPins.rotation = 1;
    viperConfigPins.uart_bus = {(gpio_num_t)12, (gpio_num_t)11};   // rx, tx
    viperConfigPins.gps_bus = {(gpio_num_t)12, (gpio_num_t)11};    // rx, tx
    viperConfigPins.badusb_bus = {(gpio_num_t)12, (gpio_num_t)11}; // rx, tx (CH9329)
    // Board's default/generic SPI bus (used by drivers without their own bus, e.g. RC522-SPI)
    viperConfigPins.outer_bus = {(gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, (gpio_num_t)8};
    viperConfigPins.PN532_bus = {(gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, (gpio_num_t)8};
    viperConfigPins.CC1101_bus = {
        (gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, (gpio_num_t)9, (gpio_num_t)10, GPIO_NUM_NC
    }; // sck,miso,mosi,cs,gdo0,gdo2
    viperConfigPins.NRF24_bus = {
        (gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, (gpio_num_t)8, (gpio_num_t)1
    }; // sck,miso,mosi,cs(ss),ce
    viperConfigPins.SDCARD_bus = {
        (gpio_num_t)6, (gpio_num_t)2, (gpio_num_t)7, (gpio_num_t)0
    }; // sck,miso,mosi,cs

    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TFT_MOSI, OUTPUT);
    digitalWrite(TFT_MOSI, HIGH);
    pinMode(TFT_SCLK, OUTPUT);

    pinMode(TFT_RST, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_DC, HIGH);

    setSysI2CBus(&Wire); // IO expander lives on the default Wire object (GROVE == sys_i2c)
    Wire.begin(viperConfigPins.sys_i2c.sda, viperConfigPins.sys_i2c.scl);

    // Configure buttons on expander as inputs
    ioExpander.button(IO_EXP_UP);
    ioExpander.button(IO_EXP_DOWN);
    ioExpander.button(IO_EXP_SEL);
    ioExpander.button(IO_EXP_ESC);
    ioExpander.button(IO_EXP_LEFT);
    ioExpander.button(IO_EXP_RIGHT);

    // Enable interrupts on the button pins only
    ioExpander.enableGPIOInterrupts();

    // Setup interrupt pin from AW9523 INT (active LOW)
    pinMode(EXPANDER_INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(EXPANDER_INT_PIN), expanderISR, FALLING);

    // === PMU (BQ25896) Setup ===
    DevicePmic pmicCfg; // no pins: the driver's default I2C init
    pmicCfg.charge_current_ma = 320;
    hal_pmic_init(pmicCfg);

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
***************************************************************************************/
void _post_setup_gpio() {
    hal_bright_attach(TFT_BL);
    hal_bright_set(TFT_BL, 100);
    // Can be used for second-stage init if needed
}

/***************************************************************************************
** Function name: getBattery()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    uint16_t voltage = hal_pmic_get_batt_voltage_mv();

    if (voltage < 3300) return 0;
    if (voltage >= 4200) return 100;

    if (voltage >= 3900) return 80 + (voltage - 3900) * 20 / 300;
    else if (voltage >= 3700) return 40 + (voltage - 3700) * 40 / 200;
    else if (voltage >= 3500) return 10 + (voltage - 3500) * 30 / 200;
    else return (voltage - 3300) * 10 / 200;
}

/***************************************************************************************
** Function name: isCharging()
***************************************************************************************/
bool isCharging() { return hal_pmic_is_charging(); }

/*********************************************************************
** Function: setBrightness
**********************************************************************/
void _setBrightness(uint8_t brightval) { hal_bright_set(TFT_BL, brightval); }

/*********************************************************************
** Function: InputHandler
** Handles PrevPress, NextPress, SelPress, AnyKeyPress, EscPress
** using IO Expander
**********************************************************************/
void InputHandler() {
    static unsigned long tm = 0;

    static bool lastUp = false;
    static bool lastDown = false;
    static bool lastLeft = false;
    static bool lastRight = false;
    static bool lastSel = false;
    static bool lastEsc = false;

    uint16_t pins = ioExpander.inputGPIO();

    bool up = !(pins & (1 << IO_EXP_UP));
    bool down = !(pins & (1 << IO_EXP_DOWN));
    bool left = !(pins & (1 << IO_EXP_LEFT));
    bool right = !(pins & (1 << IO_EXP_RIGHT));
    bool sel = !(pins & (1 << IO_EXP_SEL));
    bool esc = !(pins & (1 << IO_EXP_ESC));
    bool anyPressed = up || down || left || right || sel || esc;

    if (!expanderInterrupt && !LongPress && (millis() - tm < 250)) { return; }

    if (anyPressed) {
        tm = millis();

        if (!wakeUpScreen()) {
            AnyKeyPress = true;
        } else {
            return;
        }
    }

    UpPress = up && !lastUp;
    DownPress = down && !lastDown;
    PrevPress = left && !lastLeft;
    NextPress = right && !lastRight;
    SelPress = sel && !lastSel;
    EscPress = esc && !lastEsc;

    PrevPagePress = UpPress;
    NextPagePress = DownPress;

    lastUp = up;
    lastDown = down;
    lastLeft = left;
    lastRight = right;
    lastSel = sel;
    lastEsc = esc;

    expanderInterrupt = false;
}
/*********************************************************************
** Function: powerOff
**********************************************************************/
void powerOff() { hal_pmic_shutdown(); }

/*********************************************************************
** Function: checkReboot
**********************************************************************/
void checkReboot() {
    int countDown;
    /* Long press power off */
    if (ioExpander.readPin(IO_EXP_ESC) == LOW) {
        uint32_t time_count = millis();
        while (ioExpander.readPin(IO_EXP_ESC) == LOW) {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                tft.setTextSize(1);
                tft.setTextColor(viperConfig.priColor, viperConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                if (countDown < 3)
                    tft.drawCentreString("PWR OFF IN " + String(countDown) + "/2", tftWidth / 2, 12, 1);
                else {
                    tft.fillScreen(viperConfig.bgColor);
                    while (ioExpander.readPin(IO_EXP_ESC) == LOW);
                    delay(200);
                    powerOff();
                }
                delay(10);
            }
        }

        // Clear text after releasing the button
        delay(30);
        tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), viperConfig.bgColor);
    }
}
