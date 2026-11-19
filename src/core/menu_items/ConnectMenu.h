#ifndef __CONNECT_MENU_H__
#define __CONNECT_MENU_H__

#include <MenuItemInterface.h>

class ConnectMenu : public MenuItemInterface {
public:
    ConnectMenu() : MenuItemInterface("Connect") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return viperConfig.theme.connect; }
    const String& themePath() override { return viperConfig.theme.paths.connect; }
};

#endif
