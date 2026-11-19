#ifndef __MENU_ITEM_INTERFACE_H__
#define __MENU_ITEM_INTERFACE_H__

#include "core/display.h"
#include "core/viper_ui.h"
#include <globals.h>

// Pixel span a menu icon covers when drawn at scale 1. Lower it to render the icons
// bigger inside the same box, raise it if the widest ones start touching their label.
#define ICON_SCALE_REFERENCE 80.0f

class MenuItemInterface {
public:
    virtual ~MenuItemInterface() = default;
    virtual void optionsMenu(void) = 0;
    virtual void drawIcon(float scale = 1) = 0;
    virtual void drawIconImg() {
        drawImg(
            *viperConfig.themeFS(),
            viperConfig.getThemeItemImg(themePath()),  // const String& - no heap alloc
            0,
            imgCenterY,
            true,
            viperConfig.theme.gifDuration,
            false
        );
    }
    virtual bool hasTheme() = 0;
    virtual const String& themePath() = 0;

    bool checkTheme() { return hasTheme() && themePath().length() > 0; }
    String getName() const { return String(_name); }

    void draw(float scale = 1) {
        if (rotation != viperConfigPins.rotation) resetCoordinates();
        if (!checkTheme()) {
            tft.fillRect(0, 27, tftWidth, tftHeight - 27, viperConfig.bgColor);
            drawIcon(scale);
            viperDrawReticle(iconCenterX, iconCenterY, iconAreaH / 2 - 2, viperConfig.secColor);
            drawArrows(scale);
            drawTitle(scale);
        } else {
            if (viperConfig.theme.label)
                drawTitle(scale); // If using .GIF, labels are draw after complete, which takes some time
            drawIconImg();
            if (viperConfig.theme.label) drawTitle(scale); // Makes sure to draw over the image
        }
        drawStatusBar();
        viperDrawNavTrack(viperNavIndex, viperNavCount);
    }

    // Renders drawIcon() inside an arbitrary box instead of the full screen icon area,
    // used by the grid main menu. The icons paint straight to `tft` using the member
    // coordinates and read their colors from viperConfig, so both are swapped for the
    // call and put back afterwards.
    void drawIconInBox(int centerX, int centerY, int box, uint16_t fgColor, uint16_t bgColor) {
        int oldCenterX = iconCenterX, oldCenterY = iconCenterY;
        int oldAreaX = iconAreaX, oldAreaY = iconAreaY;
        int oldAreaW = iconAreaW, oldAreaH = iconAreaH;
        uint16_t oldPriColor = viperConfig.priColor;
        uint16_t oldBgColor = viperConfig.bgColor;

        iconCenterX = centerX;
        iconCenterY = centerY;
        iconAreaW = box;
        iconAreaH = box;
        iconAreaX = centerX - box / 2;
        iconAreaY = centerY - box / 2;
        viperConfig.priColor = fgColor;
        viperConfig.bgColor = bgColor;

        // The widest icon spans about ICON_SCALE_REFERENCE px at scale 1, so dividing by it
        // keeps every icon inside `box` and stops it bleeding into the neighbouring cells.
        drawIcon((float)box / ICON_SCALE_REFERENCE);

        viperConfig.priColor = oldPriColor;
        viperConfig.bgColor = oldBgColor;
        iconCenterX = oldCenterX;
        iconCenterY = oldCenterY;
        iconAreaX = oldAreaX;
        iconAreaY = oldAreaY;
        iconAreaW = oldAreaW;
        iconAreaH = oldAreaH;
    }

    void drawArrows(float scale = 1) {
        tft.fillRect(arrowAreaX, iconAreaY, arrowAreaW, iconAreaH, viperConfig.bgColor);
        tft.fillRect(
            tftWidth - arrowAreaX - arrowAreaW, iconAreaY, arrowAreaW, iconAreaH, viperConfig.bgColor
        );

        // Viper fangs: solid wedges either side of the icon
        viperDrawChevrons(iconCenterY, arrowAreaX, tftWidth - arrowAreaX, scale);
    }

    void drawTitle(float scale = 1) {
        int titleY = iconCenterY + iconAreaH / 2 + FG;

        tft.drawPixel(0, 0, 0);
        tft.fillRect(arrowAreaX, titleY - 2, tftWidth - 2 * arrowAreaX, LH * FM + 4, viperConfig.bgColor);
        int nchars = (tftWidth - 32) / (LW * FM);
        String label = getName().substring(0, nchars);
        label.toUpperCase();
        viperDrawPill(label, iconCenterX, titleY, FM, viperConfig.priColor, viperConfig.bgColor);
    }

protected:
    const char *_name = "";
    uint8_t rotation = ROTATION;

    int iconAreaH =
        ((tftHeight - 2 * BORDER_PAD_Y) % 2 == 0 ? tftHeight - 2 * BORDER_PAD_Y
                                                 : tftHeight - 2 * BORDER_PAD_Y + 1);
    int iconAreaW = iconAreaH;

    int iconCenterX = tftWidth / 2;
    int iconCenterY = tftHeight / 2;
    int imgCenterY = 13;

    int iconAreaX = iconCenterX - iconAreaW / 2;
    int iconAreaY = iconCenterY - iconAreaH / 2;

    int arrowAreaX = BORDER_PAD_X;
    int arrowAreaW = iconAreaX - arrowAreaX;

    MenuItemInterface(const char *name) : _name(name) {}

    void clearIconArea(void) {
        tft.fillRect(iconAreaX, iconAreaY, iconAreaW, iconAreaH, viperConfig.bgColor);
    }
    void clearImgArea(void) { tft.fillRect(7, 27, tftWidth - 14, tftHeight - 34, viperConfig.bgColor); }
    void resetCoordinates(void) {
        // Recalculate Center and ared due to portrait/landscape changings
        if (tftWidth > tftHeight) {
            iconAreaH =
                ((tftHeight - 2 * BORDER_PAD_Y) % 2 == 0 ? tftHeight - 2 * BORDER_PAD_Y
                                                         : tftHeight - 2 * BORDER_PAD_Y + 1);
        } else {
            iconAreaH =
                ((tftWidth - 2 * BORDER_PAD_Y) % 2 == 0 ? tftWidth - 2 * BORDER_PAD_Y
                                                        : tftWidth - 2 * BORDER_PAD_Y + 1);
        }

        iconAreaW = iconAreaH;

        iconCenterX = tftWidth / 2;
        iconCenterY = tftHeight / 2;

        iconAreaX = iconCenterX - iconAreaW / 2;
        iconAreaY = iconCenterY - iconAreaH / 2;

        arrowAreaX = BORDER_PAD_X;
        arrowAreaW = iconAreaX - arrowAreaX;

        rotation = viperConfigPins.rotation;
    }

private:
};

#endif // __MENU_ITEM_INTERFACE_H__
