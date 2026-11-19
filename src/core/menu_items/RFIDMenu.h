#ifndef __RFID_MENU_H__
#define __RFID_MENU_H__

#include <MenuItemInterface.h>

class RFIDMenu : public MenuItemInterface {
public:
    RFIDMenu() : MenuItemInterface("RFID") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return viperConfig.theme.rfid; }
    const String& themePath() override { return viperConfig.theme.paths.rfid; }

private:
    void configMenu(void);
};

#endif
