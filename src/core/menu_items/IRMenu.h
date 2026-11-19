#ifndef __IR_MENU_H__
#define __IR_MENU_H__

#include <MenuItemInterface.h>

class IRMenu : public MenuItemInterface {
public:
    IRMenu() : MenuItemInterface("IR") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return viperConfig.theme.ir; }
    const String& themePath() override { return viperConfig.theme.paths.ir; }

private:
    void configMenu(void);
};

#endif
