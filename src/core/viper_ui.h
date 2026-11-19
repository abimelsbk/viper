#ifndef __VIPER_UI_H__
#define __VIPER_UI_H__

#include <Arduino.h>

// Procedural Viper brand art. Everything is drawn with primitives so it
// scales to any screen, from 135x240 sticks to 320x480 panels.

// Draws the hexagon frame of the logo. `edges` = how many of the 6 edges (0..6).
void viperDrawHex(int cx, int cy, int r, int edges, uint16_t color);

// Draws the serpent "S" up to `progress` (0..1000) along its body.
// halo=true draws the thicker dark outline pass, false the bright core pass.
void viperDrawSerpent(int cx, int cy, int r, int progress, bool halo);

// Full static logo (hexagon + serpent), used by About screen etc.
void viperDrawLogo(int cx, int cy, int r);

// Animated boot intro. Returns true if the user skipped it with a key press.
bool viperIntro();

// ---- Navigation chrome ------------------------------------------------------
// Position of the item shown in the main-menu carousel (set by MainMenu).
extern int viperNavIndex;
extern int viperNavCount;

// Corner brackets + chamfered "fang" divider under the status bar.
void viperDrawFrame();

// Carousel side chevrons (solid fangs) around the icon area.
void viperDrawChevrons(int cy, int leftX, int rightX, float scale);

// Bracketed "reticle" corners around the selected icon.
void viperDrawReticle(int cx, int cy, int half, uint16_t color);

// Venom pill label (inverted text on a filled bar).
void viperDrawPill(const String &label, int cx, int y, int textSize, uint16_t fill, uint16_t text);

// Position track on the divider + "03/14" counter.
void viperDrawNavTrack(int index, int count);

#endif
