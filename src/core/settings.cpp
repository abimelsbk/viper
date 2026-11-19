#include "settings.h"
#include "core/led_control.h"
#include "core/wifi/wifi_common.h"
#include "current_year.h"
#include "display.h"
#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "modules/bjs_interpreter/interpreter.h"
#endif
#include "modules/ble_api/ble_api.hpp"
#include "modules/others/qrcode_menu.h"
#include "modules/rf/rf_utils.h" // for initRfModule
#include "mykeyboard.h"
#include "powerSave.h"
#include "sd_functions.h"
#include "settingsColor.h"
#include "utils.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <globals.h>
#include <nvs.h>
#if defined(HAS_RESISTIVE_TOUCH)
#include <CYD28_TouchscreenR.h>
#endif

int currentScreenBrightness = -1;

// This function comes from interface.h
void _setBrightness(uint8_t brightval) {}

/*********************************************************************
**  Function: setBrightness
**  set brightness value
**********************************************************************/
void setBrightness(uint8_t brightval, bool save) {
    if (viperConfig.bright > 100) viperConfig.setBright(100);
    _setBrightness(brightval);
    delay(10);

    currentScreenBrightness = brightval;
    if (save) { viperConfig.setBright(brightval); }
}

/*********************************************************************
**  Function: getBrightness
**  get brightness value
**********************************************************************/
void getBrightness() {
    if (viperConfig.bright > 100) {
        viperConfig.setBright(100);
        _setBrightness(viperConfig.bright);
        delay(10);
        setBrightness(100);
    }

    _setBrightness(viperConfig.bright);
    delay(10);

    currentScreenBrightness = viperConfig.bright;
}

/*********************************************************************
**  Function: gsetRotation
**  get/set rotation value
**********************************************************************/
int gsetRotation(bool set) {
    int getRot = viperConfigPins.rotation;
    int result = ROTATION;
    int mask = ROTATION > 1 ? -2 : 2;

    options = {
        {"Default",         [&]() { result = ROTATION; }                        },
        {"Landscape (180)", [&]() { result = ROTATION + mask; }                 },
#if TFT_WIDTH >= 170 && TFT_HEIGHT >= 240
        {"Portrait (+90)",  [&]() { result = ROTATION > 0 ? ROTATION - 1 : 3; } },
        {"Portrait (-90)",  [&]() { result = ROTATION == 3 ? 0 : ROTATION + 1; }},

#endif
    };
    addOptionToMainMenu();
    if (set) loopOptions(options);
    else result = getRot;

    if (result > 3 || result < 0) {
        result = ROTATION;
        set = true;
    }
    if (set) {
        viperConfigPins.setRotation(result);
        tft.setRotation(result);
        tft.setRotation(result); // must repeat, sometimes ESP32S3 miss one SPI command and it just
                                 // jumps this step and don't rotate
    }
    returnToMenu = true;

    if (result & 0b01) { // if 1 or 3
        tftWidth = TFT_HEIGHT;
#if defined(HAS_TOUCH)
        tftHeight = TFT_WIDTH - TOUCH_FOOTER_HEIGHT;
#else
        tftHeight = TFT_WIDTH;
#endif
    } else { // if 2 or 0
        tftWidth = TFT_WIDTH;
#if defined(HAS_TOUCH)
        tftHeight = TFT_HEIGHT - TOUCH_FOOTER_HEIGHT;
#else
        tftHeight = TFT_HEIGHT;
#endif
    }
    return result;
}

/*********************************************************************
**  Function: setBrightnessMenu
**  Handles Menu to set brightness (live-preview slider; drag on touch
**  boards, Next/Prev to step by 5%, Sel to apply, Esc to cancel)
**********************************************************************/
void setBrightnessMenu() {
    returnToMenu = false;

    int original = viperConfig.bright;
    int val = (viperConfig.bright / 5) * 5;
    if (val > 100) val = 100;
    if (val < 0) val = 0;

    const int lineHeight = FM * LH;
    const int hintHeight = FP * LH;
    const int sliderH = 12;
    const int radius = sliderH / 2;
    const int spacing = 8;
    const int paddingTop = 8;
    const int paddingBottom = 8;
    const int paddingSide = 12;

    int contentWidth = static_cast<int>(tftWidth * 0.8f);
    int contentHeight = paddingTop + lineHeight + spacing + sliderH + spacing + hintHeight + paddingBottom;
    int boxX = (tftWidth - contentWidth) / 2;
    int boxY = (tftHeight - contentHeight) / 2;

    int trackX = boxX + paddingSide;
    int trackW = contentWidth - 2 * paddingSide;
    int trackY = boxY + paddingTop + lineHeight + spacing;

    bool redraw = true;
    bool first_draw = true;
    while (1) {
        if (redraw) {
            if (first_draw) {
                tft.fillRoundRect(boxX, boxY, contentWidth, contentHeight, 5, viperConfig.bgColor);
                tft.drawRoundRect(boxX, boxY, contentWidth, contentHeight, 5, viperConfig.priColor);
                first_draw = false;
            }
            tft.setTextSize(FM);
            tft.setTextColor(viperConfig.priColor, viperConfig.bgColor);
            tft.drawCentreString(
                " Bright: " + String(val) + "% ", boxX + contentWidth / 2, boxY + paddingTop, 1
            );

            int indicatorX = trackX + (trackW * val) / 100;
            tft.fillRect(trackX, trackY - 3, trackW, 3, viperConfig.bgColor);
            tft.fillRect(trackX, trackY + sliderH, trackW, 3, viperConfig.bgColor);
            tft.fillRect(trackX - radius - 2, trackY - 4, radius * 2, sliderH + 8, viperConfig.bgColor);
            tft.fillRect(
                trackX + trackW - radius + 2, trackY - 4, radius * 2, sliderH + 8, viperConfig.bgColor
            );
            tft.drawRoundRect(trackX, trackY, trackW, sliderH, radius, viperConfig.secColor);
            int fillW = indicatorX - trackX;

            tft.fillRoundRect(trackX, trackY, fillW, sliderH, radius, viperConfig.secColor);
            tft.fillRoundRect(
                trackX + fillW, trackY + 1, trackW - fillW, sliderH - 2, radius, viperConfig.bgColor
            );

            tft.fillCircle(indicatorX, trackY + sliderH / 2, radius + 2, viperConfig.priColor);

            tft.setTextSize(FP);
            tft.setTextColor(viperConfig.priColor, viperConfig.bgColor);
            tft.drawCentreString("Sel to apply", boxX + contentWidth / 2, trackY + sliderH + spacing, 1);

            setBrightness(val, false);
            redraw = false;
        }

#if defined(HAS_TOUCH)
        if (touchPoint.pressed) {
            const int touchX = touchPoint.x;
            const int touchY = touchPoint.y;
            touchPoint.Clear();

            const int touchPadding = 10;
            if (touchX >= boxX && touchX <= boxX + contentWidth && touchY >= trackY - touchPadding &&
                touchY <= trackY + sliderH + touchPadding) {
                int clampedX = touchX;
                if (clampedX < trackX) clampedX = trackX;
                if (clampedX > trackX + trackW) clampedX = trackX + trackW;
                int newVal = ((clampedX - trackX) * 100 + trackW / 2) / trackW;
                newVal = (newVal / 5) * 5;
                if (newVal > 100) newVal = 100;
                if (newVal < 0) newVal = 0;
                if (newVal != val) {
                    val = newVal;
                    redraw = true;
                }
            }
        }
#endif

        if (check(NextPress)) {
            val += 5;
            if (val > 100) val = 100;
            redraw = true;
        }
        if (check(PrevPress)) {
            val -= 5;
            if (val < 0) val = 0;
            redraw = true;
        }
        if (check(SelPress)) {
            setBrightness(val);
            break;
        }
        if (check(EscPress) || returnToMenu) {
            setBrightness(original, false);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    tft.fillScreen(viperConfig.bgColor);
}

/*********************************************************************
**  Function: setSleepMode
**  Turn screen off and reduces cpu clock
**********************************************************************/
void setSleepMode() {
    sleepModeOn();
    while (1) {
        if (check(AnyKeyPress)) {
            sleepModeOff();
            returnToMenu = true;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/*********************************************************************
**  Function: setDimmerTimeMenu
**  Handles Menu to set dimmer time
**********************************************************************/
void setDimmerTimeMenu() {
    int idx = 0;
    if (viperConfig.dimmerSet == 10) idx = 0;
    else if (viperConfig.dimmerSet == 20) idx = 1;
    else if (viperConfig.dimmerSet == 30) idx = 2;
    else if (viperConfig.dimmerSet == 60) idx = 3;
    else if (viperConfig.dimmerSet == 0) idx = 4;
    options = {
        {"10s",      [=]() { viperConfig.setDimmer(10); }, viperConfig.dimmerSet == 10},
        {"20s",      [=]() { viperConfig.setDimmer(20); }, viperConfig.dimmerSet == 20},
        {"30s",      [=]() { viperConfig.setDimmer(30); }, viperConfig.dimmerSet == 30},
        {"60s",      [=]() { viperConfig.setDimmer(60); }, viperConfig.dimmerSet == 60},
        {"Disabled", [=]() { viperConfig.setDimmer(0); },  viperConfig.dimmerSet == 0 },
    };
    loopOptions(options, idx);
}

/*********************************************************************
**  Function: setUIColor
**  Set and store main UI color
**********************************************************************/
void setUIColor() {

    while (1) {
        options.clear();
        int idx = UI_COLOR_COUNT;
        int i = 0;
        for (const auto &mapping : UI_COLORS) {
            if (viperConfig.priColor == mapping.priColor && viperConfig.secColor == mapping.secColor &&
                viperConfig.bgColor == mapping.bgColor) {
                idx = i;
            }

            options.emplace_back(
                mapping.name,
                [=, &mapping]() {
                    uint16_t secColor = mapping.secColor;
                    uint16_t bgColor = mapping.bgColor;
                    viperConfig.setUiColor(mapping.priColor, &secColor, &bgColor);
                },
                idx == i
            );
            ++i;
        }

        options.push_back(
            {"Custom Color",
             [=]() {
                 uint16_t oldPriColor = viperConfig.priColor;
                 uint16_t oldSecColor = viperConfig.secColor;
                 uint16_t oldBgColor = viperConfig.bgColor;

                 if (setCustomUIColorMenu()) {
                     viperConfig.setUiColor(
                         viperConfig.priColor, &viperConfig.secColor, &viperConfig.bgColor
                     );
                 } else {
                     viperConfig.priColor = oldPriColor;
                     viperConfig.secColor = oldSecColor;
                     viperConfig.bgColor = oldBgColor;
                 }
                 tft.setTextColor(viperConfig.priColor, viperConfig.bgColor);
             },
             idx == UI_COLOR_COUNT}
        );

        options.push_back(
            {"Invert Color",
             [=]() {
                 viperConfig.setColorInverted(!viperConfig.colorInverted);
                 tft.invertDisplay(viperConfig.colorInverted);
             },
             viperConfig.colorInverted > 0}
        );

        addOptionToMainMenu();

        int selectedOption = loopOptions(options, idx);
        if (selectedOption == -1 || selectedOption == options.size() - 1) return;
    }
}

/*********************************************************************
**  Function: setMainMenuStyleMenu
**  Choose how the main menu presents the modules
**********************************************************************/
void setMainMenuStyleMenu() {
    options = {
        {"Carousel",
         []() { viperConfig.setMainMenuStyle(MAIN_MENU_CAROUSEL); },
         viperConfig.mainMenuStyle == MAIN_MENU_CAROUSEL},
        {"Grid",
         []() { viperConfig.setMainMenuStyle(MAIN_MENU_GRID); },
         viperConfig.mainMenuStyle == MAIN_MENU_GRID    },
    };
    addOptionToMainMenu();

    loopOptions(options, viperConfig.mainMenuStyle);
}

uint16_t alterOneColorChannel565(uint16_t color, int newR, int newG, int newB) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;

    if (newR != 256) r = newR & 0x1F;
    if (newG != 256) g = newG & 0x3F;
    if (newB != 256) b = newB & 0x1F;

    return (r << 11) | (g << 5) | b;
}

bool setCustomUIColorMenu() {
    while (1) {
        options = {
            {"Primary",    [=]() { setCustomUIColorChoiceMenu(1); }},
            {"Secondary",  [=]() { setCustomUIColorChoiceMenu(2); }},
            {"Background", [=]() { setCustomUIColorChoiceMenu(3); }},
            {"Save",       [=]() {}                                },
            {"Cancel",     [=]() {}                                }
        };

        int selectedOption = loopOptions(options);
        if (selectedOption == -1 || selectedOption == options.size() - 1) {
            return false;
        } else if (selectedOption == 3) {
            return true;
        }
    }
}

void setCustomUIColorChoiceMenu(int colorType) {
    while (1) {
        options = {
            {"Red Channel",   [=]() { setCustomUIColorSettingMenuR(colorType); }},
            {"Green Channel", [=]() { setCustomUIColorSettingMenuG(colorType); }},
            {"Blue Channel",  [=]() { setCustomUIColorSettingMenuB(colorType); }},
            {"Back",          [=]() {}                                          }
        };

        int selectedOption = loopOptions(options);
        if (selectedOption == -1 || selectedOption == options.size() - 1) return;
    }
}

void setCustomUIColorSettingMenuR(int colorType) {
    setCustomUIColorSettingMenu(colorType, 1, [](uint16_t baseColor, int i) {
        return alterOneColorChannel565(baseColor, i, 256, 256);
    });
}

void setCustomUIColorSettingMenuG(int colorType) {
    setCustomUIColorSettingMenu(colorType, 2, [](uint16_t baseColor, int i) {
        return alterOneColorChannel565(baseColor, 256, i, 256);
    });
}

void setCustomUIColorSettingMenuB(int colorType) {
    setCustomUIColorSettingMenu(colorType, 3, [](uint16_t baseColor, int i) {
        return alterOneColorChannel565(baseColor, 256, 256, i);
    });
}

constexpr const char *colorTypes[] = {
    "Background", // 0
    "Primary",    // 1
    "Secondary"   // 2
};

constexpr const char *rgbNames[] = {
    "Blue", // 0
    "Red",  // 1
    "Green" // 2
};

void setCustomUIColorSettingMenu(
    int colorType, int rgb, std::function<uint16_t(uint16_t, int)> colorGenerator
) {
    uint16_t color = (colorType == 1)   ? viperConfig.priColor
                     : (colorType == 2) ? viperConfig.secColor
                                        : viperConfig.bgColor;

    options.clear();

    static auto hoverFunctionPriColor = [](void *pointer, bool shouldRender) -> bool {
        uint16_t colorToSet = *static_cast<uint16_t *>(pointer);
        // Serial.printf("Setting primary color to: %04X\n", colorToSet);
        viperConfig.priColor = colorToSet;
        return false;
    };
    static auto hoverFunctionSecColor = [](void *pointer, bool shouldRender) -> bool {
        uint16_t colorToSet = *static_cast<uint16_t *>(pointer);
        // Serial.printf("Setting secondary color to: %04X\n", colorToSet);
        viperConfig.secColor = colorToSet;
        return false;
    };

    static auto hoverFunctionBgColor = [](void *pointer, bool shouldRender) -> bool {
        uint16_t colorToSet = *static_cast<uint16_t *>(pointer);
        // Serial.printf("Setting bg color to: %04X\n", colorToSet);
        viperConfig.bgColor = colorToSet;
        tft.fillScreen(viperConfig.bgColor);
        return false;
    };

    static uint16_t colorStorage[32];
    int selectedIndex = 0;
    int i = 0;
    int index = 0;

    if (rgb == 1) {
        selectedIndex = (color >> 11) & 0x1F;
    } else if (rgb == 2) {
        selectedIndex = ((color >> 5) & 0x3F);
    } else {
        selectedIndex = color & 0x1F;
    }

    while (i <= (rgb == 2 ? 63 : 31)) {
        if (i == 0 || (rgb == 2 && (i + 1) % 2 == 0) || (rgb != 2)) {
            uint16_t updatedColor = colorGenerator(color, i);
            colorStorage[index] = updatedColor;

            options.emplace_back(
                String(i),
                [colorType, updatedColor]() {
                    if (colorType == 1) viperConfig.priColor = updatedColor;
                    else if (colorType == 2) viperConfig.secColor = updatedColor;
                    else viperConfig.bgColor = updatedColor;
                },
                selectedIndex == i,
                (colorType == 1 ? hoverFunctionPriColor
                                : (colorType == 2 ? hoverFunctionSecColor : hoverFunctionBgColor)),
                &colorStorage[index]
            );
            ++index;
        }
        ++i;
    }

    addOptionToMainMenu();

    int selectedOption = loopOptions(
        options,
        MENU_TYPE_SUBMENU,
        (String(colorType == 1 ? "Primary" : (colorType == 2 ? "Secondary" : "Background")) + " - " +
         (rgb == 1 ? "Red" : (rgb == 2 ? "Green" : "Blue")))
            .c_str(),
        (rgb != 2) ? selectedIndex : (selectedIndex > 0 ? (selectedIndex + 1) / 2 : 0)
    );
    if (selectedOption == -1 || selectedOption == options.size() - 1) {
        if (colorType == 1) {
            viperConfig.priColor = color;
        } else if (colorType == 2) {
            viperConfig.secColor = color;
        } else {
            viperConfig.bgColor = color;
        }
        return;
    }
}

/*********************************************************************
**  Function: setSoundConfig - 01/2026 - Refactored "ConfigMenu" (this function manteined for
* retrocompatibility)
**  Enable or disable sound
**********************************************************************/
void setSoundConfig() {
    options = {
        {"Sound off", [=]() { viperConfig.setSoundEnabled(0); }, viperConfig.soundEnabled == 0},
        {"Sound on",  [=]() { viperConfig.setSoundEnabled(1); }, viperConfig.soundEnabled == 1},
    };
    loopOptions(options, viperConfig.soundEnabled);
}

/*********************************************************************
**  Function: setSoundVolume
**  Set sound volume
**********************************************************************/
void setSoundVolume() {
    options = {
        {"10%",  [=]() { viperConfig.setSoundVolume(10); },  viperConfig.soundVolume == 10 },
        {"20%",  [=]() { viperConfig.setSoundVolume(20); },  viperConfig.soundVolume == 20 },
        {"30%",  [=]() { viperConfig.setSoundVolume(30); },  viperConfig.soundVolume == 30 },
        {"40%",  [=]() { viperConfig.setSoundVolume(40); },  viperConfig.soundVolume == 40 },
        {"50%",  [=]() { viperConfig.setSoundVolume(50); },  viperConfig.soundVolume == 50 },
        {"60%",  [=]() { viperConfig.setSoundVolume(60); },  viperConfig.soundVolume == 60 },
        {"70%",  [=]() { viperConfig.setSoundVolume(70); },  viperConfig.soundVolume == 70 },
        {"80%",  [=]() { viperConfig.setSoundVolume(80); },  viperConfig.soundVolume == 80 },
        {"90%",  [=]() { viperConfig.setSoundVolume(90); },  viperConfig.soundVolume == 90 },
        {"100%", [=]() { viperConfig.setSoundVolume(100); }, viperConfig.soundVolume == 100},
    };
    loopOptions(options, viperConfig.soundVolume);
}

#ifdef HAS_RGB_LED
/*********************************************************************
**  Function: setLedBlinkConfig - 01/2026 - Refactored "ConfigMenu" (this function manteined for
* retrocompatibility)
**  Enable or disable led blink
**********************************************************************/
void setLedBlinkConfig() {
    options = {
        {"Led Blink off", [=]() { viperConfig.setLedBlinkEnabled(0); }, viperConfig.ledBlinkEnabled == 0},
        {"Led Blink on",  [=]() { viperConfig.setLedBlinkEnabled(1); }, viperConfig.ledBlinkEnabled == 1},
    };
    loopOptions(options, viperConfig.ledBlinkEnabled);
}
#endif

/*********************************************************************
**  Function: setWifiStartupConfig
**  Enable or disable wifi connection at startup
**********************************************************************/
void setWifiStartupConfig() {
    options = {
        {"Disable", [=]() { viperConfig.setWifiAtStartup(0); }, viperConfig.wifiAtStartup == 0},
        {"Enable",  [=]() { viperConfig.setWifiAtStartup(1); }, viperConfig.wifiAtStartup == 1},
    };
    loopOptions(options, viperConfig.wifiAtStartup);
}

/*********************************************************************
**  Function: addEvilWifiMenu
**  Handles Menu to add evil wifi names into config list
**********************************************************************/
void addEvilWifiMenu() {
    String apName = keyboard("", 30, "Evil Portal SSID");
    if (apName != "\x1B") viperConfig.addEvilWifiName(apName);
}

/*********************************************************************
**  Function: removeEvilWifiMenu
**  Handles Menu to remove evil wifi names from config list
**********************************************************************/
void removeEvilWifiMenu() {
    options = {};

    for (const auto &wifi_name : viperConfig.evilWifiNames) {
        options.push_back({wifi_name.c_str(), [wifi_name]() { viperConfig.removeEvilWifiName(wifi_name); }});
    }

    options.push_back({"Cancel", [=]() { backToMenu(); }});

    loopOptions(options);
}

/*********************************************************************
**  Function: setEvilEndpointCreds
**  Handles menu for changing the endpoint to access captured creds
**********************************************************************/
void setEvilEndpointCreds() {
    String userInput = keyboard(viperConfig.evilPortalEndpoints.getCredsEndpoint, 30, "Evil creds endpoint");
    if (userInput != "\x1B") viperConfig.setEvilEndpointCreds(userInput);
}

/*********************************************************************
**  Function: setEvilEndpointSsid
**  Handles menu for changing the endpoint to change evilSsid
**********************************************************************/
void setEvilEndpointSsid() {
    String userInput = keyboard(viperConfig.evilPortalEndpoints.setSsidEndpoint, 30, "Evil creds endpoint");
    if (userInput != "\x1B") viperConfig.setEvilEndpointSsid(userInput);
}

/*********************************************************************
**  Function: setEvilAllowGetCredentials
**  Handles menu for toggling access to the credential list endpoint
**********************************************************************/

void setEvilAllowGetCreds() {
    options = {
        {"Disallow",
         [=]() { viperConfig.setEvilAllowGetCreds(false); },
         viperConfig.evilPortalEndpoints.allowGetCreds == false},
        {"Allow",
         [=]() { viperConfig.setEvilAllowGetCreds(true); },
         viperConfig.evilPortalEndpoints.allowGetCreds == true },
    };
    loopOptions(options, viperConfig.evilPortalEndpoints.allowGetCreds);
}

/*********************************************************************
**  Function: setEvilAllowGetCredentials
**  Handles menu for toggling access to the change SSID endpoint
**********************************************************************/

void setEvilAllowSetSsid() {
    options = {
        {"Disallow",
         [=]() { viperConfig.setEvilAllowSetSsid(false); },
         viperConfig.evilPortalEndpoints.allowSetSsid == false},
        {"Allow",
         [=]() { viperConfig.setEvilAllowSetSsid(true); },
         viperConfig.evilPortalEndpoints.allowSetSsid == true },
    };
    loopOptions(options, viperConfig.evilPortalEndpoints.allowSetSsid);
}

/*********************************************************************
**  Function: setEvilAllowEndpointDisplay
**  Handles menu for toggling the display of the Evil Portal endpoints
**********************************************************************/

void setEvilAllowEndpointDisplay() {
    options = {
        {"Disallow",
         [=]() { viperConfig.setEvilAllowEndpointDisplay(false); },
         viperConfig.evilPortalEndpoints.showEndpoints == false},
        {"Allow",
         [=]() { viperConfig.setEvilAllowEndpointDisplay(true); },
         viperConfig.evilPortalEndpoints.showEndpoints == true },
    };
    loopOptions(options, viperConfig.evilPortalEndpoints.showEndpoints);
}

/*********************************************************************
** Function: setEvilPasswordMode
** Handles menu for setting the evil portal password mode
***********************************************************************/
void setEvilPasswordMode() {
    options = {
        {"Save 'password'",
         [=]() { viperConfig.setEvilPasswordMode(FULL_PASSWORD); },
         viperConfig.evilPortalPasswordMode == FULL_PASSWORD  },
        {"Save 'p******d'",
         [=]() { viperConfig.setEvilPasswordMode(FIRST_LAST_CHAR); },
         viperConfig.evilPortalPasswordMode == FIRST_LAST_CHAR},
        {"Save '*hidden*'",
         [=]() { viperConfig.setEvilPasswordMode(HIDE_PASSWORD); },
         viperConfig.evilPortalPasswordMode == HIDE_PASSWORD  },
        {"Save length",
         [=]() { viperConfig.setEvilPasswordMode(SAVE_LENGTH); },
         viperConfig.evilPortalPasswordMode == SAVE_LENGTH    },
    };
    loopOptions(options, viperConfig.evilPortalPasswordMode);
}

/*********************************************************************
** Function: setEvilGatewayIp
** Handles menu for setting the Evil Portal gateway IP
***********************************************************************/
void setEvilGatewayIp() {
    options = {
        {"172.0.0.1",
         [=]() { viperConfig.setEvilGatewayIp("172.0.0.1"); },
         viperConfig.evilPortalGatewayIp == "172.0.0.1"},
        {"192.168.4.1",
         [=]() { viperConfig.setEvilGatewayIp("192.168.4.1"); },
         viperConfig.evilPortalGatewayIp == "192.168.4.1"},
        {"Custom", [=]() {
             String ip = num_keyboard("", 15, "Gateway Addr");
             viperConfig.setEvilGatewayIp(ip);
         }},
    };
    loopOptions(options, viperConfig.evilPortalGatewayIp == "192.168.4.1" ? 1 : 0);
}

/*********************************************************************
**  Function: setRFModuleMenu
**  Handles Menu to set the RF module in use
**********************************************************************/
void setRFModuleMenu() {
    int result = 0;
    int idx = 0;
    int presetIdx = -1; // -1 = plain "CC1101" (board's default CC1101_bus wiring, set in _setup_gpio())
    if (viperConfigPins.rfModule == M5_RF_MODULE) idx = 0;
    else if (viperConfigPins.rfModule == CC1101_SPI_MODULE) {
        idx = 1;
        for (size_t i = 0; i < viperConfigPins.CC1101_presets.size(); i++) {
            const ViperConfigPins::SPIPins &p = viperConfigPins.CC1101_presets[i].pins;
            if (viperConfigPins.CC1101_bus.sck == p.sck && viperConfigPins.CC1101_bus.mosi == p.mosi &&
                viperConfigPins.CC1101_bus.cs == p.cs) {
                idx = 2 + (int)i;
                break;
            }
        }
    }

    options = {
        {"M5 RF433T/R", [&]() { result = M5_RF_MODULE; }     },
        {"CC1101",      [&]() { result = CC1101_SPI_MODULE; }},
    };
    for (size_t i = 0; i < viperConfigPins.CC1101_presets.size(); i++) {
        options.push_back({viperConfigPins.CC1101_presets[i].label, [&, i]() {
                               result = CC1101_SPI_MODULE;
                               presetIdx = (int)i;
                           }});
    }
    loopOptions(options, idx);
    if (result == CC1101_SPI_MODULE) {
        if (presetIdx >= 0) {
            const ViperConfigPins::SPIPinPreset &preset = viperConfigPins.CC1101_presets[presetIdx];
            viperConfigPins.setCC1101Pins(preset.pins);
            viperConfigPins.setNrf24Pins(preset.pins);
        }
        if (initRfModule()) {
            viperConfigPins.setRfModule(CC1101_SPI_MODULE);
            deinitRfModule();
            return;
        }
        // else display an error
        displayError("CC1101 not found", true);
        if (presetIdx >= 0 && viperConfigPins.CC1101_presets[presetIdx].wiringQrUrl)
            qrcode_display(viperConfigPins.CC1101_presets[presetIdx].wiringQrUrl);
        while (!check(AnyKeyPress)) vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    // fallback to "M5 RF433T/R" on errors
    viperConfigPins.setRfModule(M5_RF_MODULE);
}

/*********************************************************************
**  Function: setRFFreqMenu
**  Handles Menu to set the default frequency for the RF module
**********************************************************************/
void setRFFreqMenu() {
    float result = 433.92;
    String freq_str = num_keyboard(String(viperConfigPins.rfFreq), 10, "Default frequency:");
    if (freq_str == "\x1B") return;
    if (freq_str.length() > 1) {
        result = freq_str.toFloat();          // returns 0 if not valid
        if (result >= 280 && result <= 928) { // TODO: check valid freq according to current module?
            viperConfigPins.setRfFreq(result);
            return;
        }
    }
    // else
    displayError("Invalid frequency");
    viperConfigPins.setRfFreq(433.92); // reset to default
    delay(1000);
}

/*********************************************************************
**  Function: setRFIDModuleMenu
**  Handles Menu to set the RFID module in use
**********************************************************************/
void setRFIDModuleMenu() {
    options = {
        {"M5 RFID2",
         [=]() { viperConfigPins.setRfidModule(M5_RFID2_MODULE); },
         viperConfigPins.rfidModule == M5_RFID2_MODULE                            },
#ifdef M5STICK
        {"PN532 I2C G33",
         [=]() { viperConfigPins.setRfidModule(PN532_I2C_MODULE); },
         viperConfigPins.rfidModule == PN532_I2C_MODULE                           },
        {"PN532 I2C G36",
         [=]() { viperConfigPins.setRfidModule(PN532_I2C_SPI_MODULE); },
         viperConfigPins.rfidModule == PN532_I2C_SPI_MODULE                       },
#else
        {"PN532 on I2C",
         [=]() { viperConfigPins.setRfidModule(PN532_I2C_MODULE); },
         viperConfigPins.rfidModule == PN532_I2C_MODULE},
#endif
        {"PN532 on SPI",
         [=]() { viperConfigPins.setRfidModule(PN532_SPI_MODULE); },
         viperConfigPins.rfidModule == PN532_SPI_MODULE                           },
        {"RC522 on SPI",
         [=]() { viperConfigPins.setRfidModule(RC522_SPI_MODULE); },
         viperConfigPins.rfidModule == RC522_SPI_MODULE                           },
#if !defined(LITE_VERSION)
        {"ST25R3916 SPI",
         [=]() { viperConfigPins.setRfidModule(ST25R3916_SPI_MODULE); },
         viperConfigPins.rfidModule == ST25R3916_SPI_MODULE                       },
        {"ST25R3916 I2C",
         [=]() { viperConfigPins.setRfidModule(ST25R3916_I2C_MODULE); },
         viperConfigPins.rfidModule == ST25R3916_I2C_MODULE                       },
#ifdef CAP_NFC_SS_PIN
        // M5Stack Cap CC1101: its NFC half is an ST25R3916 on the default SPI port.
        // https://docs.m5stack.com/en/cap/Cap_CC1101
        {"CC1101 M5 Cap",
         [=]() {
             viperConfigPins.setSR25RPins(
                 {(gpio_num_t)CAP_NFC_SCK_PIN,
                  (gpio_num_t)CAP_NFC_MISO_PIN,
                  (gpio_num_t)CAP_NFC_MOSI_PIN,
                  (gpio_num_t)CAP_NFC_SS_PIN,
                  (gpio_num_t)CAP_NFC_IRQ_PIN,
                  GPIO_NUM_NC}
             );
             viperConfigPins.setRfidModule(ST25R3916_SPI_MODULE);
         },                                                              viperConfigPins.rfidModule == ST25R3916_SPI_MODULE &&
             viperConfigPins.ST25R_bus.cs == (gpio_num_t)CAP_NFC_SS_PIN},
#endif
#endif
    };
    loopOptions(options, viperConfigPins.rfidModule);
}

/*********************************************************************
**  Function: addMifareKeyMenu
**  Handles Menu to add MIFARE keys into config list
**********************************************************************/
void addMifareKeyMenu() {
    String key = keyboard("", 12, "MIFARE key");
    if (key != "\x1B") viperConfig.addMifareKey(key);
}

/*********************************************************************
**  Function: setClock
**  Handles Menu to set timezone to NTP
**********************************************************************/
const char *ntpServer = "pool.ntp.org";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, 0, 0);

void setClock() {
#if defined(HAS_RTC)
    RTC_TimeTypeDef TimeStruct;
#if defined(HAS_RTC_BM8563)
    _rtc.GetBm8563Time();
#endif
#if defined(HAS_RTC_PCF85063A)
    _rtc.GetPcf85063Time();
#endif
#endif

    options = {
        {"Via NTP Set Timezone",                                                 [&]() { viperConfig.setAutomaticTimeUpdateViaNTP(true); } },
        {"Set Time Manually",                                                    [&]() { viperConfig.setAutomaticTimeUpdateViaNTP(false); }},
        {("Daylight Savings " + String(viperConfig.dst ? "On" : "Off")).c_str(),
         [&]() {
             viperConfig.setDST(!viperConfig.dst);
             updateClockTimezone();
             returnToMenu = true;
         }                                                                                                                                 },
        {(viperConfig.clock24hr ? "24-Hour Format" : "12-Hour Format"),          [&]() {
             viperConfig.setClock24Hr(!viperConfig.clock24hr);
             returnToMenu = true;
         }                                                          }
    };

    addOptionToMainMenu();
    loopOptions(options);

    if (returnToMenu) return;

    if (viperConfig.automaticTimeUpdateViaNTP) {
        if (!wifiConnected) wifiConnectMenu();

        options.clear();

#ifndef LITE_VERSION

        struct TimezoneMapping {
            const char *name;
            float offset;
        };

        constexpr TimezoneMapping timezoneMappings[] = {
            {"UTC-12 (Baker Island, Howland Island)",     -12  },
            {"UTC-11 (Niue, Pago Pago)",                  -11  },
            {"UTC-10 (Honolulu, Papeete)",                -10  },
            {"UTC-9 (Anchorage, Gambell)",                -9   },
            {"UTC-9.5 (Marquesas Islands)",               -9.5 },
            {"UTC-8 (Los Angeles, Vancouver, Tijuana)",   -8   },
            {"UTC-7 (Denver, Phoenix, Edmonton)",         -7   },
            {"UTC-6 (Mexico City, Chicago, Tegucigalpa)", -6   },
            {"UTC-5 (New York, Toronto, Lima)",           -5   },
            {"UTC-4 (Caracas, Santiago, La Paz)",         -4   },
            {"UTC-3 (Brasilia, Sao Paulo, Montevideo)",   -3   },
            {"UTC-2 (South Georgia, Mid-Atlantic)",       -2   },
            {"UTC-1 (Azores, Cape Verde)",                -1   },
            {"UTC+0 (London, Lisbon, Casablanca)",        0    },
            {"UTC+0.5 (Tehran)",                          0.5  },
            {"UTC+1 (Berlin, Paris, Rome)",               1    },
            {"UTC+2 (Cairo, Athens, Johannesburg)",       2    },
            {"UTC+3 (Moscow, Riyadh, Nairobi)",           3    },
            {"UTC+3.5 (Tehran)",                          3.5  },
            {"UTC+4 (Dubai, Baku, Muscat)",               4    },
            {"UTC+4.5 (Kabul)",                           4.5  },
            {"UTC+5 (Islamabad, Karachi, Tashkent)",      5    },
            {"UTC+5.5 (New Delhi, Mumbai, Colombo)",      5.5  },
            {"UTC+5.75 (Kathmandu)",                      5.75 },
            {"UTC+6 (Dhaka, Almaty, Omsk)",               6    },
            {"UTC+6.5 (Yangon, Cocos Islands)",           6.5  },
            {"UTC+7 (Bangkok, Jakarta, Hanoi)",           7    },
            {"UTC+8 (Beijing, Singapore, Perth)",         8    },
            {"UTC+8.75 (Eucla)",                          8.75 },
            {"UTC+9 (Tokyo, Seoul, Pyongyang)",           9    },
            {"UTC+9.5 (Adelaide, Darwin)",                9.5  },
            {"UTC+10 (Sydney, Melbourne, Vladivostok)",   10   },
            {"UTC+10.5 (Lord Howe Island)",               10.5 },
            {"UTC+11 (Solomon Islands, Nouméa)",          11   },
            {"UTC+12 (Auckland, Fiji, Kamchatka)",        12   },
            {"UTC+12.75 (Chatham Islands)",               12.75},
            {"UTC+13 (Tonga, Phoenix Islands)",           13   },
            {"UTC+14 (Kiritimati)",                       14   }
        };

        int idx = 0;
        int i = 0;
        for (const auto &mapping : timezoneMappings) {
            if (viperConfig.tmz == mapping.offset) { idx = i; }

            options.emplace_back(
                mapping.name, [=, &mapping]() { viperConfig.setTmz(mapping.offset); }, idx == i
            );
            ++i;
        }

#else
        constexpr float timezoneOffsets[] = {-12, -11, -10,  -9.5, -9,  -8,    -7, -6, -5,   -4,
                                             -3,  -2,  -1,   0,    0.5, 1,     2,  3,  3.5,  4,
                                             4.5, 5,   5.5,  5.75, 6,   6.5,   7,  8,  8.75, 9,
                                             9.5, 10,  10.5, 11,   12,  12.75, 13, 14};

        int idx = 0;
        int i = 0;
        for (const auto &offset : timezoneOffsets) {
            if (viperConfig.tmz == offset) idx = i;

            options.emplace_back(
                ("UTC" + String(offset >= 0 ? "+" : "") + String(offset)).c_str(),
                [=]() { viperConfig.setTmz(offset); },
                viperConfig.tmz == offset
            );
            ++i;
        }

#endif

        addOptionToMainMenu();

        loopOptions(options, idx);

        updateClockTimezone();

    } else {
        int hr, mn, am = 0; // Initialize am to default value
        options = {};
        for (int i = 0; i < 12; i++) {
            String tmp = String(i < 10 ? "0" : "") + String(i);
            options.push_back({tmp.c_str(), [&]() { delay(1); }});
        }

        hr = loopOptions(options, MENU_TYPE_SUBMENU, "Set Hour");
        options.clear();

        for (int i = 0; i < 60; i++) {
            String tmp = String(i < 10 ? "0" : "") + String(i);
            options.push_back({tmp.c_str(), [&]() { delay(1); }});
        }

        mn = loopOptions(options, MENU_TYPE_SUBMENU, "Set Minute");
        options.clear();

        options = {
            {"AM", [&]() { am = 0; } },
            {"PM", [&]() { am = 12; }},
        };

        loopOptions(options);

#if defined(HAS_RTC)
        TimeStruct.Hours = hr + am;
        TimeStruct.Minutes = mn;
        TimeStruct.Seconds = 0;
        _rtc.SetTime(&TimeStruct);
        _rtc.GetTime(&_time);
        _rtc.GetDate(&_date);

        struct tm timeinfo = {};
        timeinfo.tm_sec = _time.Seconds;
        timeinfo.tm_min = _time.Minutes;
        timeinfo.tm_hour = _time.Hours;
        timeinfo.tm_mday = _date.Date;
        timeinfo.tm_mon = _date.Month > 0 ? _date.Month - 1 : 0;
        timeinfo.tm_year = _date.Year >= 1900 ? _date.Year - 1900 : 0;
        time_t epoch = mktime(&timeinfo);
        struct timeval tv = {.tv_sec = epoch};
        settimeofday(&tv, nullptr);
#else
        rtc.setTime(0, mn, hr + am, 20, 06, CURRENT_YEAR); // send me a gift, @Pirata!
        struct tm t = rtc.getTimeStruct();
        time_t epoch = mktime(&t);
        struct timeval tv = {.tv_sec = epoch};
        settimeofday(&tv, nullptr);
#endif
        clock_set = true;
    }
}

void runClockLoop(bool showMenuHint) {
    int tmp = 0;
    unsigned long hintStartTime = millis();
    bool hintVisible = showMenuHint;

#if defined(HAS_RTC)
#if defined(HAS_RTC_BM8563)
    _rtc.GetBm8563Time();
#endif
#if defined(HAS_RTC_PCF85063A)
    _rtc.GetPcf85063Time();
#endif
    _rtc.GetTime(&_time);
#endif

    // Delay due to SelPress() detected on run
    tft.fillScreen(viperConfig.bgColor);
    delay(300);

    for (;;) {
        if (millis() - tmp > 1000) {
#if defined(HAS_RTC)
            updateTimeStr(_rtc.getTimeStruct());
#else
            updateTimeStr(rtc.getTimeStruct());
#endif
            Serial.print("Current time: ");
            Serial.println(timeStr);
            tft.setTextColor(viperConfig.priColor, viperConfig.bgColor);
            tft.drawRect(
                BORDER_PAD_X,
                BORDER_PAD_X,
                tftWidth - 2 * BORDER_PAD_X,
                tftHeight - 2 * BORDER_PAD_X,
                viperConfig.priColor
            );
            uint8_t f_size = 4;
            for (uint8_t i = 4; i > 0; i--) {
                if (i * LW * strlen(timeStr) < (tftWidth - BORDER_PAD_X * 2)) {
                    f_size = i;
                    break;
                }
            }
            tft.setTextSize(f_size);
            tft.drawCentreString(timeStr, tftWidth / 2, tftHeight / 2 - 13, 1);

            // "OK to show menu" hint management
            if (hintVisible && (millis() - hintStartTime < 5000)) {
                tft.setTextSize(FP);
                tft.drawCentreString("OK to show menu", tftWidth / 2, tftHeight / 2 + 25, 1);
            } else if (hintVisible && (millis() - hintStartTime >= 5000)) {
                // Clear hint after 5 seconds
                tft.fillRect(
                    BORDER_PAD_X + 1,
                    tftHeight / 2 + 20,
                    tftWidth - 2 * BORDER_PAD_X - 2,
                    20,
                    viperConfig.bgColor
                );
                hintVisible = false;
            }
            tmp = millis();
        }

        // Checks to exit the loop
        if (check(SelPress)) {
            tft.fillScreen(viperConfig.bgColor);
            if (showMenuHint) {
                // Exits the loop to return to the caller (ClockMenu)
                break;
            } else {
                // Original behavior
                returnToMenu = true;
                break;
            }
        }

        if (check(EscPress)) {
            tft.fillScreen(viperConfig.bgColor);
            returnToMenu = true;
            break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*********************************************************************
**  Function: gsetIrTxPin
**  get or set IR Tx Pin
**********************************************************************/
int gsetIrTxPin(bool set) {
    int result = viperConfigPins.irTx;

    if (set) {
        options.clear();
        std::vector<std::pair<const char *, int>> pins;
        pins = IR_TX_PINS;
        int idx = 100;
        int j = 0;
        for (auto pin : pins) {
            if (pin.second == viperConfigPins.irTx && idx == 100) idx = j;
            j++;
#ifdef ALLOW_ALL_GPIO_FOR_IR_RF
            int i = pin.second;
            if (i != TFT_CS && i != TFT_RST && i != TFT_SCLK && i != TFT_MOSI && i != TFT_BL &&
                i != TOUCH_CS && i != viperConfigPins.SDCARD_bus.cs && i != viperConfigPins.SDCARD_bus.mosi &&
                i != viperConfigPins.SDCARD_bus.miso)
#endif
                options.push_back(
                    {pin.first,
                     [=]() { viperConfigPins.setIrTxPin(pin.second); },
                     pin.second == viperConfigPins.irTx}
                );
        }

        loopOptions(options, idx);
        options.clear();

        Serial.println("Saved pin: " + String(viperConfigPins.irTx));
    }

    returnToMenu = true;
    return viperConfigPins.irTx;
}

void setIrTxRepeats() {
    uint8_t chRpts = 0; // Chosen Repeats

    options = {
        {"None",             [&]() { chRpts = 0; } },
        {"5  (+ 1 initial)", [&]() { chRpts = 5; } },
        {"10 (+ 1 initial)", [&]() { chRpts = 10; }},
        {"Custom",           [&]() {
             // up to 99 repeats
             String rpt =
                 num_keyboard(String(viperConfigPins.irTxRepeats), 2, "Nbr of Repeats (+ 1 initial)");
             chRpts = static_cast<uint8_t>(rpt.toInt());
         }                       },
    };
    addOptionToMainMenu();

    loopOptions(options);

    if (returnToMenu) return;

    viperConfigPins.setIrTxRepeats(chRpts);
}
/*********************************************************************
**  Function: gsetIrRxPin
**  get or set IR Rx Pin
**********************************************************************/
int gsetIrRxPin(bool set) {
    int result = viperConfigPins.irRx;

    if (result < 0) viperConfigPins.setIrRxPin(GROVE_SCL);
    if (set) {
        options.clear();
        std::vector<std::pair<const char *, int>> pins;
        pins = IR_RX_PINS;
        int idx = -1;
        int j = 0;
        for (auto pin : pins) {
            if (pin.second == viperConfigPins.irRx && idx < 0) idx = j;
            j++;
#ifdef ALLOW_ALL_GPIO_FOR_IR_RF
            int i = pin.second;
            if (i != TFT_CS && i != TFT_RST && i != TFT_SCLK && i != TFT_MOSI && i != TFT_BL &&
                i != TOUCH_CS && i != viperConfigPins.SDCARD_bus.cs && i != viperConfigPins.SDCARD_bus.mosi &&
                i != viperConfigPins.SDCARD_bus.miso)
#endif
                options.push_back(
                    {pin.first,
                     [=]() { viperConfigPins.setIrRxPin(pin.second); },
                     pin.second == viperConfigPins.irRx}
                );
        }

        loopOptions(options);
    }

    returnToMenu = true;
    return viperConfigPins.irRx;
}

/*********************************************************************
**  Function: gsetRfTxPin
**  get or set RF Tx Pin
**********************************************************************/
int gsetRfTxPin(bool set) {
    int result = viperConfigPins.rfTx;

    if (result < 0) viperConfigPins.setRfTxPin(viperConfigPins.i2c_bus.sda);
    if (set) {
        options.clear();
        std::vector<std::pair<const char *, int>> pins;
        pins = RF_TX_PINS;
        int idx = -1;
        int j = 0;
        for (auto pin : pins) {
            if (pin.second == viperConfigPins.rfTx && idx < 0) idx = j;
            j++;
#ifdef ALLOW_ALL_GPIO_FOR_IR_RF
            int i = pin.second;
            if (i != TFT_CS && i != TFT_RST && i != TFT_SCLK && i != TFT_MOSI && i != TFT_BL &&
                i != TOUCH_CS && i != viperConfigPins.SDCARD_bus.cs && i != viperConfigPins.SDCARD_bus.mosi &&
                i != viperConfigPins.SDCARD_bus.miso)
#endif
                options.push_back(
                    {pin.first,
                     [=]() { viperConfigPins.setRfTxPin(pin.second); },
                     pin.second == viperConfigPins.rfTx}
                );
        }

        loopOptions(options);
        options.clear();
    }

    returnToMenu = true;
    return viperConfigPins.rfTx;
}

/*********************************************************************
**  Function: gsetRfRxPin
**  get or set FR Rx Pin
**********************************************************************/
int gsetRfRxPin(bool set) {
    int result = viperConfigPins.rfRx;

    if (result < 0) viperConfigPins.setRfRxPin(GROVE_SCL);
    if (set) {
        options.clear();
        std::vector<std::pair<const char *, int>> pins;
        pins = RF_RX_PINS;
        int idx = -1;
        int j = 0;
        for (auto pin : pins) {
            if (pin.second == viperConfigPins.rfRx && idx < 0) idx = j;
            j++;
#ifdef ALLOW_ALL_GPIO_FOR_IR_RF
            int i = pin.second;
            if (i != TFT_CS && i != TFT_RST && i != TFT_SCLK && i != TFT_MOSI && i != TFT_BL &&
                i != TOUCH_CS && i != viperConfigPins.SDCARD_bus.cs && i != viperConfigPins.SDCARD_bus.mosi &&
                i != viperConfigPins.SDCARD_bus.miso)
#endif
                options.push_back(
                    {pin.first,
                     [=]() { viperConfigPins.setRfRxPin(pin.second); },
                     pin.second == viperConfigPins.rfRx}
                );
        }

        loopOptions(options);
        options.clear();
    }

    returnToMenu = true;
    return viperConfigPins.rfRx;
}

/*********************************************************************
**  Function: setStartupApp
**  Handles Menu to set startup app
**********************************************************************/
void setStartupApp() {
    int idx = 0;
    if (viperConfig.startupApp == "") idx = 0;

    options = {
        {"None", [=]() { viperConfig.setStartupApp(""); }, viperConfig.startupApp == ""}
    };

    int index = 0;
    for (String appName : startupApp.getAppNames()) {
        index++;
        if (viperConfig.startupApp == appName) idx = index;

        options.push_back({appName.c_str(), [=]() {
                               viperConfig.setStartupApp(appName);
#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
                               if (appName == "JS Interpreter") {
                                   options = getScriptsOptionsList("", true);
                                   loopOptions(options, MENU_TYPE_SUBMENU, "Startup Script");
                               }
#endif
                           }});
    }

    loopOptions(options, idx);
    options.clear();
}

/*********************************************************************
**  Function: setGpsBaudrateMenu
**  Handles Menu to set the baudrate for the GPS module
**********************************************************************/
void setGpsBaudrateMenu() {
    options = {
        {"9600 bps",   [=]() { viperConfigPins.setGpsBaudrate(9600); },  viperConfigPins.gpsBaudrate == 9600 },
        {"19200 bps",  [=]() { viperConfigPins.setGpsBaudrate(19200); }, viperConfigPins.gpsBaudrate == 19200},
        {"38400 bps",  [=]() { viperConfigPins.setGpsBaudrate(38400); }, viperConfigPins.gpsBaudrate == 38400},
        {"57600 bps",  [=]() { viperConfigPins.setGpsBaudrate(57600); }, viperConfigPins.gpsBaudrate == 57600},
        {"115200 bps",
         [=]() { viperConfigPins.setGpsBaudrate(115200); },
         viperConfigPins.gpsBaudrate == 115200                                                               },
    };

    loopOptions(options, viperConfigPins.gpsBaudrate);
}

/*********************************************************************
**  Function: setWifiApSsidMenu
**  Handles Menu to set the WiFi AP SSID
**********************************************************************/
void setWifiApSsidMenu() {
    const bool isDefault = viperConfig.wifiAp.ssid == "ViperNet";

    options = {
        {"Default (ViperNet)",
         [=]() { viperConfig.setWifiApCreds("ViperNet", viperConfig.wifiAp.pwd); },
         isDefault                                                                            },
        {"Custom",
         [=]() {
             String newSsid = keyboard(viperConfig.wifiAp.ssid, 32, "WiFi AP SSID:");
             if (newSsid != "\x1B") {
                 if (!newSsid.isEmpty()) viperConfig.setWifiApCreds(newSsid, viperConfig.wifiAp.pwd);
                 else displayError("SSID cannot be empty", true);
             }
         },                                                                         !isDefault},
    };
    addOptionToMainMenu();

    loopOptions(options, isDefault ? 0 : 1);
}

/*********************************************************************
**  Function: setWifiApPasswordMenu
**  Handles Menu to set the WiFi AP Password
**********************************************************************/
void setWifiApPasswordMenu() {
    const bool isDefault = viperConfig.wifiAp.pwd == "vipernet";

    options = {
        {"Default (vipernet)",
         [=]() { viperConfig.setWifiApCreds(viperConfig.wifiAp.ssid, "vipernet"); },
         isDefault                                                                             },
        {"Custom",
         [=]() {
             String newPassword = keyboard(viperConfig.wifiAp.pwd, 32, "WiFi AP Password:", true);
             if (newPassword != "\x1B") {
                 if (!newPassword.isEmpty()) viperConfig.setWifiApCreds(viperConfig.wifiAp.ssid, newPassword);
                 else displayError("Password cannot be empty", true);
             }
         },                                                                          !isDefault},
    };
    addOptionToMainMenu();

    loopOptions(options, isDefault ? 0 : 1);
}

/*********************************************************************
**  Function: setWifiApCredsMenu
**  Handles Menu to configure WiFi AP Credentials
**********************************************************************/
void setWifiApCredsMenu() {
    options = {
        {"SSID",     setWifiApSsidMenu    },
        {"Password", setWifiApPasswordMenu},
    };
    addOptionToMainMenu();

    loopOptions(options);
}

/*********************************************************************
**  Function: setNetworkCredsMenu
**  Main Menu for setting Network credentials (BLE & WiFi)
**********************************************************************/
void setNetworkCredsMenu() {
    options = {
        {"WiFi AP Creds", setWifiApCredsMenu}
    };
    addOptionToMainMenu();

    loopOptions(options);
}

/*********************************************************************
**  Function: setBadUSBBLEMenu
**  Main Menu for setting Bad USB/BLE options
**********************************************************************/
void setBadUSBBLEMenu() {
    options = {
        {"Keyboard Layout", setBadUSBBLEKeyboardLayoutMenu},
        {"Key Delay",       setBadUSBBLEKeyDelayMenu      },
        {"Show Output",     setBadUSBBLEShowOutputMenu    },
    };
    addOptionToMainMenu();

    loopOptions(options);
}

/*********************************************************************
**  Function: setBadUSBBLEKeyboardLayoutMenu
**  Main Menu for setting Bad USB/BLE Keyboard Layout
**********************************************************************/
void setBadUSBBLEKeyboardLayoutMenu() {
    uint8_t opt = viperConfig.badUSBBLEKeyboardLayout;

    options.clear();
    options = {
        {"US International",      [&]() { opt = 0; } },
        {"Danish",                [&]() { opt = 1; } },
        {"English (UK)",          [&]() { opt = 2; } },
        {"French (AZERTY)",       [&]() { opt = 3; } },
        {"German",                [&]() { opt = 4; } },
        {"Hungarian",             [&]() { opt = 5; } },
        {"Italian",               [&]() { opt = 6; } },
        {"Polish",                [&]() { opt = 7; } },
        {"Portuguese (Brazil)",   [&]() { opt = 8; } },
        {"Portuguese (Portugal)", [&]() { opt = 9; } },
        {"Slovenian",             [&]() { opt = 10; }},
        {"Spanish",               [&]() { opt = 11; }},
        {"Swedish",               [&]() { opt = 12; }},
        {"Turkish",               [&]() { opt = 13; }},
    };
    addOptionToMainMenu();

    loopOptions(options, opt);

    if (opt != viperConfig.badUSBBLEKeyboardLayout) { viperConfig.setBadUSBBLEKeyboardLayout(opt); }
}

/*********************************************************************
**  Function: setBadUSBBLEKeyDelayMenu
**  Main Menu for setting Bad USB/BLE Keyboard Key Delay
**********************************************************************/
void setBadUSBBLEKeyDelayMenu() {
    String delayStr = num_keyboard(String(viperConfig.badUSBBLEKeyDelay), 3, "Key Delay (ms):");
    if (delayStr != "\x1B") {
        uint16_t delayVal = static_cast<uint16_t>(delayStr.toInt());
        if (delayVal <= 500) {
            viperConfig.setBadUSBBLEKeyDelay(delayVal);
        } else if (delayVal != 0) {
            displayError("Invalid key delay value (0 to 500)", true);
        }
    }
}

/*********************************************************************
**  Function: setBadUSBBLEShowOutputMenu
**  Main Menu for setting Bad USB/BLE Show Output
**********************************************************************/
void setBadUSBBLEShowOutputMenu() {
    options.clear();
    options = {
        {"Enable",  [&]() { viperConfig.setBadUSBBLEShowOutput(true); } },
        {"Disable", [&]() { viperConfig.setBadUSBBLEShowOutput(false); }},
    };
    addOptionToMainMenu();

    loopOptions(options, viperConfig.badUSBBLEShowOutput ? 0 : 1);
}

/*********************************************************************
**  Function: setMacAddressMenu - @IncursioHack
**  Handles Menu to configure WiFi MAC Address
**********************************************************************/
void setMacAddressMenu() {
    String currentMAC = viperConfig.wifiMAC;
    if (currentMAC == "") currentMAC = WiFi.macAddress();

    options.clear();
    options = {
        {"Default MAC (" + WiFi.macAddress() + ")",
         [&]() { viperConfig.setWifiMAC(""); },
         viperConfig.wifiMAC == ""},
        {"Set Custom MAC",
         [&]() {
             String newMAC = keyboard(viperConfig.wifiMAC, 17, "XX:YY:ZZ:AA:BB:CC");
             if (newMAC == "\x1B") return;
             if (newMAC.length() == 17) {
                 viperConfig.setWifiMAC(newMAC);
             } else {
                 displayError("Invalid MAC format");
             }
         }, viperConfig.wifiMAC != ""},
        {"Random MAC", [&]() {
             uint8_t randomMac[6];
             for (int i = 0; i < 6; i++) randomMac[i] = random(0x00, 0xFF);
             char buf[18];
             snprintf(
                 buf,
                 sizeof(buf),
                 "%02X:%02X:%02X:%02X:%02X:%02X",
                 randomMac[0],
                 randomMac[1],
                 randomMac[2],
                 randomMac[3],
                 randomMac[4],
                 randomMac[5]
             );
             viperConfig.setWifiMAC(String(buf));
         }}
    };

    addOptionToMainMenu();
    loopOptions(options, MENU_TYPE_REGULAR, ("Current: " + currentMAC).c_str());
}

/*********************************************************************
**  Function: setSPIPins
**  Main Menu to manually set SPI Pins
**********************************************************************/
void setSPIPinsMenu(ViperConfigPins::SPIPins &value) {
    uint8_t opt = 0;
    bool changed = false;
    ViperConfigPins::SPIPins points = value;

RELOAD:
    options = {
        {String("SCK =" + String(points.sck)).c_str(), [&]() { opt = 1; }},
        {String("MISO=" + String(points.miso)).c_str(), [&]() { opt = 2; }},
        {String("MOSI=" + String(points.mosi)).c_str(), [&]() { opt = 3; }},
        {String("CS  =" + String(points.cs)).c_str(), [&]() { opt = 4; }},
        {String("CE/GDO0=" + String(points.io0)).c_str(), [&]() { opt = 5; }},
        {String("NC/GDO2=" + String(points.io2)).c_str(), [&]() { opt = 6; }},
        {"Save Config", [&]() { opt = 7; }, changed},
        {"Main Menu", [&]() { opt = 0; }},
    };

    loopOptions(options);
    if (opt == 0) return;
    else if (opt == 7) {
        if (changed) {
            value = points;
            viperConfigPins.setSpiPins(value);
        }
    } else {
        options = {};
        gpio_num_t sel = GPIO_NUM_NC;
        int index = 0;
        if (opt == 1) index = points.sck + 1;
        else if (opt == 2) index = points.miso + 1;
        else if (opt == 3) index = points.mosi + 1;
        else if (opt == 4) index = points.cs + 1;
        else if (opt == 5) index = points.io0 + 1;
        else if (opt == 6) index = points.io2 + 1;
        for (int8_t i = -1; i <= GPIO_NUM_MAX; i++) {
            String tmp = String(i);
            options.push_back({tmp.c_str(), [i, &sel]() { sel = (gpio_num_t)i; }});
        }
        loopOptions(options, index);
        options.clear();
        if (opt == 1) points.sck = sel;
        else if (opt == 2) points.miso = sel;
        else if (opt == 3) points.mosi = sel;
        else if (opt == 4) points.cs = sel;
        else if (opt == 5) points.io0 = sel;
        else if (opt == 6) points.io2 = sel;
        changed = true;
        goto RELOAD;
    }
}

/*********************************************************************
**  Function: setUARTPins
**  Main Menu to manually set SPI Pins
**********************************************************************/
void setUARTPinsMenu(ViperConfigPins::UARTPins &value) {
    uint8_t opt = 0;
    bool changed = false;
    ViperConfigPins::UARTPins points = value;

RELOAD:
    options = {
        {String("RX = " + String(points.rx)).c_str(), [&]() { opt = 1; }},
        {String("TX = " + String(points.tx)).c_str(), [&]() { opt = 2; }},
        {"Save Config", [&]() { opt = 7; }, changed},
        {"Main Menu", [&]() { opt = 0; }},
    };

    loopOptions(options);
    if (opt == 0) return;
    else if (opt == 7) {
        if (changed) {
            value = points;
            viperConfigPins.setUARTPins(value);
        }
    } else {
        options = {};
        gpio_num_t sel = GPIO_NUM_NC;
        int index = 0;
        if (opt == 1) index = points.rx + 1;
        else if (opt == 2) index = points.tx + 1;
        for (int8_t i = -1; i <= GPIO_NUM_MAX; i++) {
            String tmp = String(i);
            options.push_back({tmp.c_str(), [i, &sel]() { sel = (gpio_num_t)i; }});
        }
        loopOptions(options, index);
        options.clear();
        if (opt == 1) points.rx = sel;
        else if (opt == 2) points.tx = sel;
        changed = true;
        goto RELOAD;
    }
}

/*********************************************************************
**  Function: setI2CPins
**  Main Menu to manually set SPI Pins
**********************************************************************/
void setI2CPinsMenu(ViperConfigPins::I2CPins &value) {
#if defined(SOC_HP_I2C_NUM) && SOC_HP_I2C_NUM < 2
    // On SoCs with a single HP I2C peripheral, Grove I2C shares hardware with the system I2C bus
    // (touch/RTC/PMIC), so its pins can't be changed independently when the board has a real,
    // distinct system I2C bus.
    if (viperConfigPins.sys_i2c.sda >= 0 && viperConfigPins.sys_i2c.scl >= 0) {
        displayError("I2C Pins cannot be changed on this board", true);
        return;
    }
#endif
    uint8_t opt = 0;
    bool changed = false;
    ViperConfigPins::I2CPins points = value;

RELOAD:
    options = {
        {String("SDA = " + String(points.sda)).c_str(), [&]() { opt = 1; }},
        {String("SCL = " + String(points.scl)).c_str(), [&]() { opt = 2; }},
        {"Save Config", [&]() { opt = 7; }, changed},
        {"Main Menu", [&]() { opt = 0; }},
    };

    loopOptions(options);
    if (opt == 0) return;
    else if (opt == 7) {
        if (changed) {
            value = points;
            viperConfigPins.setI2CPins(value);
        }
    } else {
        options = {};
        gpio_num_t sel = GPIO_NUM_NC;
        int index = 0;
        if (opt == 1) index = points.sda + 1;
        else if (opt == 2) index = points.scl + 1;
        for (int8_t i = -1; i <= GPIO_NUM_MAX; i++) {
            String tmp = String(i);
            options.push_back({tmp.c_str(), [i, &sel]() { sel = (gpio_num_t)i; }});
        }
        loopOptions(options, index);
        options.clear();
        if (opt == 1) points.sda = sel;
        else if (opt == 2) points.scl = sel;
        changed = true;
        goto RELOAD;
    }
}

/*********************************************************************
**  Function: setSpeakerPinsMenu
**  Menu to manually set the I2S speaker pins (was the BCLK/WCLK/DOUT/MCLK -D macros)
**********************************************************************/
void setSpeakerPinsMenu(ViperConfigPins::SpeakerPins &value) {
    uint8_t opt = 0;
    bool changed = false;
    ViperConfigPins::SpeakerPins points = value;

RELOAD:
    options = {
        {String("BCLK=" + String(points.bclk)).c_str(), [&]() { opt = 1; }},
        {String("WCLK=" + String(points.ws)).c_str(), [&]() { opt = 2; }},
        {String("DOUT=" + String(points.dout)).c_str(), [&]() { opt = 3; }},
        {String("MCLK=" + String(points.mclk)).c_str(), [&]() { opt = 4; }},
        {"Save Config", [&]() { opt = 7; }, changed},
        {"Main Menu", [&]() { opt = 0; }},
    };

    loopOptions(options);
    if (opt == 0) return;
    else if (opt == 7) {
        if (changed) {
            value = points;
            viperConfigPins.setSpeakerPins(value);
        }
    } else {
        options = {};
        gpio_num_t sel = GPIO_NUM_NC;
        int index = 0;
        if (opt == 1) index = points.bclk + 1;
        else if (opt == 2) index = points.ws + 1;
        else if (opt == 3) index = points.dout + 1;
        else if (opt == 4) index = points.mclk + 1;
        for (int8_t i = -1; i <= GPIO_NUM_MAX; i++) {
            String tmp = String(i);
            options.push_back({tmp.c_str(), [i, &sel]() { sel = (gpio_num_t)i; }});
        }
        loopOptions(options, index);
        options.clear();
        if (opt == 1) points.bclk = sel;
        else if (opt == 2) points.ws = sel;
        else if (opt == 3) points.dout = sel;
        else if (opt == 4) points.mclk = sel;
        changed = true;
        goto RELOAD;
    }
}

/*********************************************************************
**  Function: setMicPinsMenu
**  Menu to manually set the microphone wiring (was PIN_CLK/PIN_DATA/PIN_BCLK/PIN_WS)
**********************************************************************/
void setMicPinsMenu(ViperConfigPins::MicPins &value) {
    uint8_t opt = 0;
    bool changed = false;
    ViperConfigPins::MicPins points = value;

RELOAD:
    {
        const char *typeName = points.type == MIC_TYPE_I2S_PHILIPS ? "I2S Philips"
                               : points.type == MIC_TYPE_I2S_MSB   ? "I2S MSB"
                                                                   : "PDM";
        options = {
            {String("Type=" + String(typeName)).c_str(), [&]() { opt = 1; }},
            {String("CLK/BCLK=" + String(points.clk)).c_str(), [&]() { opt = 2; }},
            {String("WS  =" + String(points.ws)).c_str(), [&]() { opt = 3; }},
            {String("DATA=" + String(points.data)).c_str(), [&]() { opt = 4; }},
            {"Save Config", [&]() { opt = 7; }, changed},
            {"Main Menu", [&]() { opt = 0; }},
        };
    }

    loopOptions(options);
    if (opt == 0) return;
    else if (opt == 7) {
        if (changed) {
            value = points;
            viperConfigPins.setMicPins(value);
        }
    } else if (opt == 1) {
        options = {
            {"PDM",         [&]() { points.type = MIC_TYPE_PDM; }        },
            {"I2S MSB",     [&]() { points.type = MIC_TYPE_I2S_MSB; }    },
            {"I2S Philips", [&]() { points.type = MIC_TYPE_I2S_PHILIPS; }},
        };
        loopOptions(options, points.type);
        options.clear();
        changed = true;
        goto RELOAD;
    } else {
        options = {};
        gpio_num_t sel = GPIO_NUM_NC;
        int index = 0;
        if (opt == 2) index = points.clk + 1;
        else if (opt == 3) index = points.ws + 1;
        else if (opt == 4) index = points.data + 1;
        for (int8_t i = -1; i <= GPIO_NUM_MAX; i++) {
            String tmp = String(i);
            options.push_back({tmp.c_str(), [i, &sel]() { sel = (gpio_num_t)i; }});
        }
        loopOptions(options, index);
        options.clear();
        if (opt == 2) points.clk = sel;
        else if (opt == 3) points.ws = sel;
        else if (opt == 4) points.data = sel;
        changed = true;
        goto RELOAD;
    }
}

/*********************************************************************
**  Function: setBuzzerPinMenu
**  Menu to manually set the buzzer pin (was the BUZZ_PIN -D macro).
**  -1 disables the buzzer.
**********************************************************************/
void setBuzzerPinMenu() {
    options = {};
    int sel = viperConfigPins.buzzer;
    for (int8_t i = -1; i <= GPIO_NUM_MAX; i++) {
        String tmp = String(i);
        options.push_back({tmp.c_str(), [i, &sel]() { sel = i; }});
    }
    loopOptions(options, viperConfigPins.buzzer + 1);
    options.clear();
    viperConfigPins.setBuzzerPin(sel);
}

/*********************************************************************
**  Function: setTheme
**  Menu to change Theme
**********************************************************************/
void setTheme() {
    FS *fs = &LittleFS;
    options = {
        {"Little FS", [&]() { fs = &LittleFS; }},
        {"Default",
         [&]() {
             viperConfig.removeTheme();
             viperConfig.themePath = "";
             viperConfig.theme.fs = 0;
             viperConfig.secColor = DEFAULT_SECCOLOR;
             viperConfig.bgColor = TFT_BLACK;
             viperConfig.setUiColor(DEFAULT_PRICOLOR);
#ifdef HAS_RGB_LED
             viperConfig.ledBright = 50;
             viperConfig.ledColor = 0x960064;
             viperConfig.ledEffect = 0;
             viperConfig.ledEffectSpeed = 5;
             viperConfig.ledEffectDirection = 1;
             ledSetup();
#endif
             viperConfig.saveFile();
             fs = nullptr;
         }                                     },
        {"Main Menu", [&]() { fs = nullptr; }  }
    };
    if (setupSdCard()) {
        options.insert(options.begin(), {"SD Card", [&]() { fs = &SD; }});
    }
    loopOptions(options);
    if (fs == nullptr) return;

    String filepath = loopSD(*fs, true, "JSON");
    if (viperConfig.openThemeFile(fs, filepath, true)) {
        viperConfig.themePath = filepath;
        if (fs == &LittleFS) viperConfig.theme.fs = 1;
        else if (fs == &SD) viperConfig.theme.fs = 2;
        else viperConfig.theme.fs = 0;

        viperConfig.saveFile();
    }
}
#if !defined(LITE_VERSION)
BLE_API bleApi;
static bool ble_api_enabled = false;

void enableBLEAPI() {
    if (!ble_api_enabled) {
        // displayWarning("BLE API require huge amount of RAM.");
        // displayWarning("Some features may stop working.");
        Serial.println(ESP.getFreeHeap());
        bleApi.setup();
        Serial.println(ESP.getFreeHeap());
    } else {
        bleApi.end();
    }

    ble_api_enabled = !ble_api_enabled;

    // Give the user visual feedback about the new state, otherwise the toggle
    // looks like it does nothing and gets pressed repeatedly (which cycles the
    // BLE stack setup/teardown and can corrupt the GATT table).
    if (ble_api_enabled) {
        displayInfo("BLE API ON > Advertising as 'Viper'", true);
    } else {
        displayInfo("BLE API OFF", true);
    }
}

bool appStoreInstalled() {
    FS *fs;
    if (!getFsStorage(fs)) {
        log_i("Fail getting filesystem");
        return false;
    }

    return fs->exists("/ViperJS/Tools/App Store.bin");
}

#include <HTTPClient.h>
void installAppStoreJS() {

    if (!WiFi.isConnected()) { wifiConnectMenu(WIFI_STA); }
    if (!WiFi.isConnected()) {
        displayWarning("WiFi not connected", true);
        return;
    }

    FS *fs;
    if (!getFsStorage(fs)) {
        log_i("Fail getting filesystem");
        return;
    }

    if (!fs->exists("/ViperJS")) {
        if (!fs->mkdir("/ViperJS")) {
            displayWarning("Failed to create /ViperJS directory", true);
            return;
        }
    }

    if (!fs->exists("/ViperJS/Tools")) {
        if (!fs->mkdir("/ViperJS/Tools")) {
            displayWarning("Failed to create /ViperJS/Tools directory", true);
            return;
        }
    }

    HTTPClient http;
    http.begin("http://appstore.bruce.computer/service/appstorebin/");
    int httpCode = http.GET();
    if (httpCode != 200) {
        http.end();
        displayWarning("Failed to download App Store", true);
        return;
    }

    File file = fs->open("/ViperJS/Tools/App Store.bin", FILE_WRITE);
    if (!file) {
        displayWarning("Failed to save App Store", true);
        return;
    }
    file.print(http.getString());
    http.end();
    file.close();

    // Check for old App Store.js and delete if found
    if (fs->exists("/ViperJS/Tools/App Store.js")) {
        fs->remove("/ViperJS/Tools/App Store.js");
    }

    displaySuccess("App Store installed", true);
    displaySuccess("Goto JS Interpreter -> Tools -> App Store", true);
}
#endif

#if defined(HAS_RESISTIVE_TOUCH)
extern CYD28_TouchR touch; // defined by hal/inputs/touch.cpp

static constexpr const char *TOUCH_CAL_NAMESPACE = "touch_cal";

static bool validTouchCalibration(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1) {
    return x0 > 0 && x1 > 0 && y0 > 0 && y1 > 0 && x0 != x1 && y0 != y1;
}

static esp_err_t readTouchCalibrationItems(
    nvs_handle_t handle, uint16_t &x0, uint16_t &x1, uint16_t &y0, uint16_t &y1, uint8_t &rot
) {
    esp_err_t err = nvs_get_u16(handle, "x0", &x0);
    err |= nvs_get_u16(handle, "x1", &x1);
    err |= nvs_get_u16(handle, "y0", &y0);
    err |= nvs_get_u16(handle, "y1", &y1);
    err |= nvs_get_u8(handle, "r", &rot);
    if (err == ESP_OK) return err;

    // Older builds used single-letter keys.
    err = nvs_get_u16(handle, "x", &x0);
    err |= nvs_get_u16(handle, "X", &x1);
    err |= nvs_get_u16(handle, "y", &y0);
    err |= nvs_get_u16(handle, "Y", &y1);
    err |= nvs_get_u8(handle, "r", &rot);
    return err;
}

static bool getTouchCalibration(uint16_t &x0, uint16_t &x1, uint16_t &y0, uint16_t &y1, uint8_t &rot) {
    x0 = x1 = y0 = y1 = 0;
    rot = 0;

    nvs_handle_t handle;
    if (nvs_open(TOUCH_CAL_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        log_i("getTouchCalibration: no %s namespace found", TOUCH_CAL_NAMESPACE);
        return false;
    }
    esp_err_t err = readTouchCalibrationItems(handle, x0, x1, y0, y1, rot);
    nvs_close(handle);
    rot &= 0x07;
    return err == ESP_OK && validTouchCalibration(x0, x1, y0, y1);
}

// Loads the calibration from the NVS namespace "touch_cal" and applies it to the touch driver.
// Returns false when there is no valid calibration stored.
bool loadTouchCalibration() {
    uint16_t x0, x1, y0, y1;
    uint8_t rot;

    if (!getTouchCalibration(x0, x1, y0, y1, rot)) {
        Serial.println("loadTouchCalibration: Failed to load valid calibration data");
        return false;
    }

    uint16_t parameters[5] = {x0, x1, y0, y1, rot};
    touch.setTouch(parameters);
    Serial.printf(
        "loadTouchCalibration: Loaded calibration - x0:%u x1:%u y0:%u y1:%u rot:%u\n", x0, x1, y0, y1, rot
    );
    return true;
}

bool saveTouchCalibration(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint8_t rot) {
    if (!validTouchCalibration(x0, x1, y0, y1)) {
        Serial.printf(
            "saveTouchCalibration: Invalid calibration - x0:%u x1:%u y0:%u y1:%u rot:%u\n",
            x0,
            x1,
            y0,
            y1,
            rot
        );
        return false;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(TOUCH_CAL_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        Serial.printf("saveTouchCalibration: Failed to open %s namespace\n", TOUCH_CAL_NAMESPACE);
        return false;
    }

    rot &= 0x07;
    err = nvs_set_u16(handle, "x0", x0);
    err |= nvs_set_u16(handle, "x1", x1);
    err |= nvs_set_u16(handle, "y0", y0);
    err |= nvs_set_u16(handle, "y1", y1);
    err |= nvs_set_u8(handle, "r", rot);
    if (err == ESP_OK) err = nvs_commit(handle);
    nvs_close(handle);

    if (err == ESP_OK) {
        Serial.printf(
            "saveTouchCalibration: Saved calibration - x0:%u x1:%u y0:%u y1:%u rot:%u\n", x0, x1, y0, y1, rot
        );
        return true;
    }
    Serial.printf("saveTouchCalibration: Failed to save calibration data: %s\n", esp_err_to_name(err));
    return false;
}

// Asks the user to touch the 4 corners, computes the calibration, applies it and stores it in NVS.
void calibrateTouch() {
    // The input task reads the same touch driver: keep it out while the raw readings are taken.
    inputLock();
    tft.setRotation(0);
    tft.fillScreen(viperConfig.bgColor);
    wakeUpScreen();
    const uint16_t _w = tft.width();
    const uint16_t _h = tft.height();

    struct RawTouchPoint {
        uint16_t x;
        uint16_t y;
    };

    auto drawCenteredLine = [&](const char *text, int16_t y) { tft.drawCentreString(text, _w / 2, y, 1); };

    tft.setTextColor(viperConfig.priColor, viperConfig.bgColor);
    tft.setTextSize(FP);
    const int16_t lineHeight = LH;
    int16_t y = (_h - lineHeight * 4) / 2;
    drawCenteredLine("Viper Touch Calibration", y);
    y += lineHeight;
    drawCenteredLine("---------------------------", y);
    y += lineHeight;
    drawCenteredLine("Touch the screen corners", y);
    y += lineHeight;
    drawCenteredLine("indicated by the arrows", y);
    delay(500);

    auto drawArrow = [&](uint8_t corner) {
        tft.fillRect(0, 0, 30, 30, viperConfig.bgColor);
        tft.fillRect(0, _h - 30, 30, 30, viperConfig.bgColor);
        tft.fillRect(_w - 30, 0, 30, 30, viperConfig.bgColor);
        tft.fillRect(_w - 30, _h - 30, 30, 30, viperConfig.bgColor);
        const int16_t edge = 0;
        const int16_t len = 28;
        const int16_t head = 8;
        const bool right = corner == 1 || corner == 2;
        const bool bottom = corner >= 2;
        const int16_t x0 = right ? _w - edge : edge;
        const int16_t y0 = bottom ? _h - edge : edge;
        const int16_t sx = right ? -1 : 1;
        const int16_t sy = bottom ? -1 : 1;

        tft.drawLine(x0 + sx * len, y0 + sy * len, x0, y0, viperConfig.priColor);
        tft.drawLine(x0, y0, x0 + sx * head, y0, viperConfig.priColor);
        tft.drawLine(x0, y0, x0, y0 + sy * head, viperConfig.priColor);
        tft.drawLine(x0 + 1, y0 + sy, x0 + sx * (head + 1), y0 + sy, viperConfig.priColor);
        tft.drawLine(x0 + sx, y0 + 1, x0 + sx, y0 + sy * (head + 1), viperConfig.priColor);
    };

    auto logRaw = [&](const char *phase) {
        static unsigned long lastLog = 0;
        if (millis() - lastLog < 1000) return;
        lastLog = millis();
        auto r = touch.getPointRaw();
        Serial.printf(
            "calibrateTouch[%s]: raw x=%d y=%d z=%d isrWake=%d\n", phase, r.x, r.y, r.z, touch.isrWake
        );
    };
    auto readRawPoint = [&]() {
        while (touch.touched()) {
            logRaw("release");
            delay(10);
        }
        while (!touch.touched()) {
            logRaw("wait");
            delay(10);
        }

        uint32_t sx = 0;
        uint32_t sy = 0;
        const uint8_t samples = 6;
        for (uint8_t i = 0; i < samples; ++i) {
            auto p = touch.getPointRaw();
            sx += p.x;
            sy += p.y;
            delay(18);
        }

        while (touch.touched()) { delay(10); }
        return RawTouchPoint{uint16_t(sx / samples), uint16_t(sy / samples)};
    };

    RawTouchPoint p[4];
    for (uint8_t i = 0; i < 4; ++i) {
        drawArrow(i);
        p[i] = readRawPoint();
    }

    const int32_t leftRawX = (int32_t(p[0].x) + p[3].x) / 2;
    const int32_t rightRawX = (int32_t(p[1].x) + p[2].x) / 2;
    const int32_t leftRawY = (int32_t(p[0].y) + p[3].y) / 2;
    const int32_t rightRawY = (int32_t(p[1].y) + p[2].y) / 2;

    const uint8_t swapXY = abs(rightRawX - leftRawX) < abs(rightRawY - leftRawY);
    const uint8_t invertX = swapXY ? p[0].y > p[1].y : p[0].x > p[1].x;
    const uint8_t invertY = swapXY ? p[0].x > p[3].x : p[0].y > p[3].y;
    // Same bit layout the Launcher stores (swapXY and invertX are inverted for compatibility with
    // the CYD28_TouchscreenR orientation handling), so both firmwares can share the NVS values.
    const uint8_t rot = !swapXY | (invertY << 1) | (!invertX << 2);
    uint16_t xMin = !swapXY ? p[0].y : p[0].x;
    uint16_t xMax = xMin;
    uint16_t yMin = !swapXY ? p[0].x : p[0].y;
    uint16_t yMax = yMin;
    for (uint8_t i = 1; i < 4; ++i) {
        const uint16_t rx = !swapXY ? p[i].y : p[i].x;
        const uint16_t ry = !swapXY ? p[i].x : p[i].y;
        if (rx < xMin) xMin = rx;
        if (rx > xMax) xMax = rx;
        if (ry < yMin) yMin = ry;
        if (ry > yMax) yMax = ry;
    }

    uint16_t parameters[5] = {xMin, xMax, yMin, yMax, rot};
    touch.setTouch(parameters);
    saveTouchCalibration(xMin, xMax, yMin, yMax, rot);

    Serial.printf(
        "calibrateTouch: x0:%u x1:%u y0:%u y1:%u rot:%u swap:%u invX:%u invY:%u\n",
        xMin,
        xMax,
        yMin,
        yMax,
        rot,
        swapXY,
        invertX,
        invertY
    );
    tft.setRotation(viperConfigPins.rotation);
    tft.fillScreen(viperConfig.bgColor);
    wakeUpScreen();
    inputUnlock();
}
#endif
