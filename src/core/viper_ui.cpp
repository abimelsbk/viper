#include "viper_ui.h"
#include "display.h"
#include <globals.h>
#include <math.h>

// ---------------------------------------------------------------------------
// Geometry helpers
// ---------------------------------------------------------------------------

static inline uint16_t dim565(uint16_t c, uint8_t level /*0..255*/) {
    uint16_t r = ((c >> 11) & 0x1F) * level / 255;
    uint16_t g = ((c >> 5) & 0x3F) * level / 255;
    uint16_t b = (c & 0x1F) * level / 255;
    return (r << 11) | (g << 5) | b;
}

static void hexVertex(int cx, int cy, int r, int i, int &x, int &y) {
    // pointy-top hexagon, vertex 0 at the top, clockwise
    float a = (-90.0f + 60.0f * i) * DEG_TO_RAD;
    x = cx + (int)lroundf(r * cosf(a));
    y = cy + (int)lroundf(r * sinf(a));
}

void viperDrawHex(int cx, int cy, int r, int edges, uint16_t color) {
    if (edges > 6) edges = 6;
    for (int i = 0; i < edges; i++) {
        int x0, y0, x1, y1;
        hexVertex(cx, cy, r, i, x0, y0);
        hexVertex(cx, cy, r, i + 1, x1, y1);
        tft.drawLine(x0, y0, x1, y1, color);
        // inner echo line gives the "double frame" look
        hexVertex(cx, cy, r - 3, i, x0, y0);
        hexVertex(cx, cy, r - 3, i + 1, x1, y1);
        tft.drawLine(x0, y0, x1, y1, dim565(color, 90));
    }
}

// The serpent is two stacked arcs forming an S, traced from the tail (bottom
// left) to the head (top right). t in [0,1] -> point + body thickness.
static void serpentPoint(int cx, int cy, int r, float t, float &x, float &y, float &thick) {
    const float a = r * 0.36f; // arc radius
    const float total = 2.0f;  // two arcs, each mapped to 1.0 of t*2
    float s = t * total;
    float deg;
    float ox, oy;
    if (s < 1.0f) {
        // lower arc: from bottom-left (160deg) back round to the top of the arc (-90deg)
        deg = 160.0f - s * 250.0f;
        ox = cx;
        oy = cy + a;
    } else {
        // upper arc: from its bottom (90deg) round the left and top to the head (-30deg)
        deg = 90.0f + (s - 1.0f) * 240.0f;
        ox = cx;
        oy = cy - a;
    }
    float rad = deg * DEG_TO_RAD;
    x = ox + a * cosf(rad);
    y = oy + a * sinf(rad);
    // thin tail, fat body, slim neck
    float body = sinf(t * PI);
    thick = 1.0f + body * r * 0.11f;
    if (t > 0.92f) thick *= 0.75f;
}

void viperDrawSerpent(int cx, int cy, int r, int progress, bool halo) {
    if (progress <= 0) return;
    if (progress > 1000) progress = 1000;
    const int steps = 140;
    int last = steps * progress / 1000;
    uint16_t pri = viperConfig.priColor;
    uint16_t col = halo ? dim565(viperConfig.secColor, 140) : pri;
    for (int i = 0; i <= last; i++) {
        float x, y, th;
        serpentPoint(cx, cy, r, (float)i / steps, x, y, th);
        int rad = (int)th + (halo ? 2 : 0);
        tft.fillCircle((int)x, (int)y, rad, col);
    }
    if (progress < 1000 || halo) return;

    // Head: diamond pointing along the body direction, with an eye.
    float hx, hy, th, px, py, pth;
    serpentPoint(cx, cy, r, 1.0f, hx, hy, th);
    serpentPoint(cx, cy, r, 0.97f, px, py, pth);
    float dx = hx - px, dy = hy - py;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.01f) return;
    dx /= len;
    dy /= len;
    float hs = r * 0.13f; // head size
    float nx = -dy, ny = dx;
    int tipX = hx + dx * hs * 1.6f, tipY = hy + dy * hs * 1.6f;
    int lX = hx + nx * hs, lY = hy + ny * hs;
    int rX = hx - nx * hs, rY = hy - ny * hs;
    int bX = hx - dx * hs * 0.5f, bY = hy - dy * hs * 0.5f;
    tft.fillTriangle(tipX, tipY, lX, lY, rX, rY, pri);
    tft.fillTriangle(bX, bY, lX, lY, rX, rY, pri);
    int eX = hx + dx * hs * 0.4f + nx * hs * 0.45f;
    int eY = hy + dy * hs * 0.4f + ny * hs * 0.45f;
    tft.fillCircle(eX, eY, max(1, (int)(hs * 0.18f)), viperConfig.bgColor);
}

void viperDrawLogo(int cx, int cy, int r) {
    viperDrawHex(cx, cy, r, 6, viperConfig.priColor);
    viperDrawSerpent(cx, cy, r, 1000, true);
    viperDrawSerpent(cx, cy, r, 1000, false);
}

// Forked tongue flick from the head. on=false erases it.
static void tongue(int cx, int cy, int r, bool on) {
    float hx, hy, th, px, py, pth;
    serpentPoint(cx, cy, r, 1.0f, hx, hy, th);
    serpentPoint(cx, cy, r, 0.97f, px, py, pth);
    float dx = hx - px, dy = hy - py;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.01f) return;
    dx /= len;
    dy /= len;
    float hs = r * 0.13f;
    int sx = hx + dx * hs * 1.7f, sy = hy + dy * hs * 1.7f;
    int mx = sx + dx * hs * 1.2f, my = sy + dy * hs * 1.2f;
    float nx = -dy, ny = dx;
    uint16_t c = on ? (uint16_t)TFT_RED : viperConfig.bgColor;
    tft.drawLine(sx, sy, mx, my, c);
    tft.drawLine(mx, my, mx + (dx + nx) * hs * 0.5f, my + (dy + ny) * hs * 0.5f, c);
    tft.drawLine(mx, my, mx + (dx - nx) * hs * 0.5f, my + (dy - ny) * hs * 0.5f, c);
}

// ---------------------------------------------------------------------------
// Boot intro
// ---------------------------------------------------------------------------
//   0 -  450ms  scan grid fades in
// 450 - 1050ms  hexagon traces itself edge by edge
// 1050- 1900ms  serpent shadow slithers in (tail -> head)
// 1900- 2400ms  venom (bright core) floods the body
// 2400- 3000ms  "VIPER" types out, tongue flicks
// 3000- 3600ms  tagline + version, hold
bool viperIntro() {
    const int W = tftWidth, H = tftHeight;
    const bool landscape = W > H;
    int r, cx, cy;
    if (landscape) {
        r = (int)(H * 0.34f);
        cx = W / 2;
        cy = (int)(H * 0.40f);
    } else {
        r = (int)(W * 0.30f);
        cx = W / 2;
        cy = (int)(H * 0.36f);
    }
    if (r < 18) r = 18;
    const int textSize = H >= 200 ? 3 : 2;
    const int textY = cy + r + (H >= 200 ? 14 : 6);

    uint16_t bg = viperConfig.bgColor;
    uint16_t pri = viperConfig.priColor;
    uint16_t grid = dim565(viperConfig.secColor, 45);

    tft.fillScreen(bg);

    uint32_t start = millis();
    int gridDrawn = 0, hexDrawn = 0, shadow = 0, venom = 0, letters = 0;
    bool tagDone = false;
    int flick = -1;
    const char *word = "VIPER";

    while (true) {
        uint32_t t = millis() - start;

        // 1. scan grid
        int gridTarget = t >= 450 ? 1000 : t * 1000 / 450;
        int rows = H / 8;
        while (gridDrawn * 1000 / max(1, rows) < gridTarget && gridDrawn <= rows) {
            int y = gridDrawn * 8;
            tft.drawFastHLine(0, y, W, grid);
            if (gridDrawn == 0)
                for (int x = 0; x < W; x += 8) tft.drawFastVLine(x, 0, H, grid);
            gridDrawn++;
        }

        // 2. hexagon
        if (t >= 450) {
            int edges = t >= 1050 ? 6 : 1 + (t - 450) * 6 / 600;
            if (edges > hexDrawn) {
                tft.fillCircle(cx, cy, r - 4, bg); // clear grid inside the frame
                viperDrawHex(cx, cy, r, edges, pri);
                hexDrawn = edges;
            }
        }

        // 3. serpent shadow
        if (t >= 1050) {
            int p = t >= 1900 ? 1000 : (t - 1050) * 1000 / 850;
            if (p > shadow) {
                viperDrawSerpent(cx, cy, r, p, true);
                shadow = p;
            }
        }

        // 4. venom core
        if (t >= 1900) {
            int p = t >= 2400 ? 1000 : (t - 1900) * 1000 / 500;
            if (p > venom) {
                viperDrawSerpent(cx, cy, r, p, false);
                venom = p;
            }
        }

        // 5. wordmark + tongue
        if (t >= 2400) {
            int n = t >= 2900 ? 5 : 1 + (t - 2400) * 5 / 500;
            if (n > letters) {
                tft.setTextSize(textSize);
                tft.setTextColor(pri, bg);
                int cw = 6 * textSize;
                int x0 = cx - (5 * cw + 4 * textSize) / 2;
                for (int i = letters; i < n; i++) {
                    tft.drawString(String(word[i]), x0 + i * (cw + textSize), textY, 1);
                }
                letters = n;
            }
            int phase = ((t - 2400) / 120) % 2;
            if (t < 3000 && phase != flick) {
                tongue(cx, cy, r, phase == 0);
                flick = phase;
            } else if (t >= 3000 && flick != 2) {
                tongue(cx, cy, r, false);
                flick = 2;
            }
        }

        // 6. tagline + version
        if (t >= 3000 && !tagDone) {
            tft.setTextSize(1);
            tft.setTextColor(dim565(pri, 170), bg);
            int tagY = textY + 8 * textSize + 4;
            if (tagY + 8 <= H) tft.drawCentreString("STRIKE FIRST", cx, tagY, 1);
            tft.setTextColor(dim565(pri, 110), bg);
            tft.drawRightString(VIPER_VERSION, W - 3, H - 10, 1);
            tagDone = true;
        }

        if (t >= 3600) break;

        if (check(AnyKeyPress)) {
            tft.fillScreen(bg);
            return true;
        }
        delay(8);
    }
    return false;
}

// ---------------------------------------------------------------------------
// Navigation chrome
// ---------------------------------------------------------------------------
int viperNavIndex = -1;
int viperNavCount = 0;

void viperDrawFrame() {
    const int B = BORDER_OFFSET_FROM_SCREEN_EDGE;
    const int W = tftWidth, H = tftHeight;
    const uint16_t c = viperConfig.priColor;
    const int L = max(10, min(W, H) / 9);

    // corner brackets (2px)
    for (int k = 0; k < 2; k++) {
        tft.drawFastHLine(B, B + k, L, c);
        tft.drawFastVLine(B + k, B, L, c);
        tft.drawFastHLine(W - B - L, B + k, L, c);
        tft.drawFastVLine(W - B - 1 - k, B, L, c);
        tft.drawFastHLine(B, H - B - 1 - k, L, c);
        tft.drawFastVLine(B + k, H - B - L, L, c);
        tft.drawFastHLine(W - B - L, H - B - 1 - k, L, c);
        tft.drawFastVLine(W - B - 1 - k, H - B - L, L, c);
    }

    // fang divider: chamfered ends dropping into a straight rule
    const int y = STATUS_BAR_HEIGHT - 5;
    tft.drawLine(B, y - 4, B + 4, y, c);
    tft.drawFastHLine(B + 4, y, W - 2 * B - 8, dim565(c, 120));
    tft.drawLine(W - B - 5, y, W - B - 1, y - 4, c);
}

void viperDrawChevrons(int cy, int leftX, int rightX, float scale) {
    const uint16_t c = viperConfig.priColor;
    const uint16_t d = dim565(viperConfig.secColor, 150);
    int s = max(5, (int)(scale * 9));
    // left fang: solid wedge with a dim echo behind it
    tft.fillTriangle(leftX + s + 4, cy - s, leftX + s + 4, cy + s, leftX + 4, cy, d);
    tft.fillTriangle(leftX + s, cy - s, leftX + s, cy + s, leftX, cy, c);
    // right fang
    tft.fillTriangle(rightX - s - 4, cy - s, rightX - s - 4, cy + s, rightX - 4, cy, d);
    tft.fillTriangle(rightX - s, cy - s, rightX - s, cy + s, rightX, cy, c);
}

void viperDrawReticle(int cx, int cy, int half, uint16_t color) {
    int L = max(6, half / 3);
    int x0 = cx - half, x1 = cx + half, y0 = cy - half, y1 = cy + half;
    tft.drawFastHLine(x0, y0, L, color);
    tft.drawFastVLine(x0, y0, L, color);
    tft.drawFastHLine(x1 - L + 1, y0, L, color);
    tft.drawFastVLine(x1, y0, L, color);
    tft.drawFastHLine(x0, y1, L, color);
    tft.drawFastVLine(x0, y1 - L + 1, L, color);
    tft.drawFastHLine(x1 - L + 1, y1, L, color);
    tft.drawFastVLine(x1, y1 - L + 1, L, color);
}

void viperDrawPill(const String &label, int cx, int y, int textSize, uint16_t fill, uint16_t text) {
    int tw = label.length() * LW * textSize;
    int th = LH * textSize;
    int padX = 4 + 2 * textSize;
    int x = cx - tw / 2 - padX;
    int w = tw + 2 * padX;
    // chamfered pill: rectangle with clipped corners
    tft.fillRect(x + 3, y - 2, w - 6, th + 4, fill);
    tft.fillRect(x, y + 1, w, th - 2, fill);
    tft.fillTriangle(x, y + 1, x + 3, y - 2, x + 3, y + 1, fill);
    tft.fillTriangle(x + w - 1, y + 1, x + w - 4, y - 2, x + w - 4, y + 1, fill);
    tft.fillTriangle(x, y + th - 2, x + 3, y + th + 1, x + 3, y + th - 2, fill);
    tft.fillTriangle(x + w - 1, y + th - 2, x + w - 4, y + th + 1, x + w - 4, y + th - 2, fill);
    tft.setTextSize(textSize);
    tft.setTextColor(text, fill);
    tft.drawCentreString(label, cx, y, 1);
}

void viperDrawNavTrack(int index, int count) {
    if (count <= 1 || index < 0) return;
    const int B = BORDER_OFFSET_FROM_SCREEN_EDGE;
    const int y = STATUS_BAR_HEIGHT - 5;
    const int x0 = B + 6, x1 = tftWidth - B - 6;
    const int span = x1 - x0;
    int seg = max(6, span / count);
    int sx = x0 + (span - seg) * index / (count - 1);
    tft.drawFastHLine(x0, y, span, dim565(viperConfig.priColor, 120));
    tft.fillRect(sx, y - 1, seg, 3, viperConfig.priColor);

    // counter "03/14" under the divider, right aligned
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d/%02d", index + 1, count);
    tft.setTextSize(FP);
    tft.setTextColor(dim565(viperConfig.priColor, 170), viperConfig.bgColor);
    tft.drawRightString(buf, tftWidth - B - 4, y + 4, 1);
}
