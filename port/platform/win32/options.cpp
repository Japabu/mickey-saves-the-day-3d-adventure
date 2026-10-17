// options.cpp - the port's options panel (an improvement over the original, which has
// none): Escape (or Cmd+,) opens it over the game, which pauses meanwhile. Music, voices
// and sound effects have a volume each, the window can be full screen, and the 3D can
// be drawn sharper. The choices are kept in port.ini in the data
// directory, next to `cd = <path>`.

#include "internal.h"
#include "../host/host.h"
#include "user.h"
#include "../port.h"

#include <atomic>
#include <mutex>
#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <vector>

namespace win32 {

// ---- port.ini: `key = value` lines; other lines are kept as they are

static std::mutex s_ini_mu;
static std::vector<std::string> s_ini_lines;
static bool s_ini_loaded;

static std::string ini_path() { return data_dir() + "/port.ini"; }

static std::string trim(const std::string &s)
{
    size_t a = s.find_first_not_of(" \t\r"), b = s.find_last_not_of(" \t\r");
    return a == std::string::npos ? "" : s.substr(a, b - a + 1);
}

static void ini_load()
{
    if (s_ini_loaded)
        return;
    s_ini_loaded = true;
    if (FILE *f = fopen(ini_path().c_str(), "r")) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) {
            std::string l = line;
            while (!l.empty() && (l.back() == '\n' || l.back() == '\r'))
                l.pop_back();
            s_ini_lines.push_back(l);
        }
        fclose(f);
    }
}

static bool ini_get(const char *key, std::string &value)
{
    std::lock_guard<std::mutex> lock(s_ini_mu);
    ini_load();
    for (const std::string &l : s_ini_lines) {
        size_t eq = l.find('=');
        if (eq != std::string::npos && !strcasecmp(trim(l.substr(0, eq)).c_str(), key)) {
            value = trim(l.substr(eq + 1));
            return true;
        }
    }
    return false;
}

static void ini_set(const char *key, int value)
{
    std::lock_guard<std::mutex> lock(s_ini_mu);
    ini_load();
    std::string line = std::string(key) + " = " + std::to_string(value);
    bool found = false;
    for (std::string &l : s_ini_lines) {
        size_t eq = l.find('=');
        if (eq != std::string::npos && !strcasecmp(trim(l.substr(0, eq)).c_str(), key)) {
            l = line;
            found = true;
        }
    }
    if (!found)
        s_ini_lines.push_back(line);
    if (FILE *f = fopen(ini_path().c_str(), "w")) {
        for (const std::string &l : s_ini_lines)
            fprintf(f, "%s\n", l.c_str());
        fclose(f);
    }
}

static int ini_int(const char *key, int fallback)
{
    std::string v;
    return ini_get(key, v) && !v.empty() ? atoi(v.c_str()) : fallback;
}

// ---- the settings

enum Row { MUSIC, VOICES, EFFECTS, FULLSCREEN, SHARPNESS, DONE, ROWS };

static std::atomic<int> s_volume[3] = {{-1}, {-1}, {-1}}; // percent; -1 until read
static std::atomic<unsigned> s_volume_version{1};
static const char *const kVolumeKeys[3] = {"music volume", "voice volume", "effects volume"};

static int volume(int which)
{
    int v = s_volume[which];
    if (v < 0) {
        v = ini_int(kVolumeKeys[which], 100);
        v = v < 0 ? 0 : v > 100 ? 100 : v;
        s_volume[which] = v;
    }
    return v;
}

static void set_volume(int which, int v)
{
    v = v < 0 ? 0 : v > 100 ? 100 : v;
    if (v == volume(which))
        return;
    s_volume[which] = v;
    s_volume_version++;
    ini_set(kVolumeKeys[which], v);
}

// The panel's texts in the languages of the game's European versions, picked by
// MICKEY_LANG as the CD's language is (German unless set). Code page 1252.
enum Text { T_TITLE, T_MUSIC, T_VOICES, T_SOUNDS, T_FULLSCREEN, T_SHARPNESS, T_CONTINUE, TEXTS };
static const char *const kTexts[][TEXTS] = {
    {"Optionen", "Musik", "Stimmen", "Ger\xe4usche", "Vollbild", "3D-Sch\xe4rfe", "Weiter"},
    {"Options", "Music", "Voices", "Sounds", "Full screen", "3D sharpness", "Continue"},
    {"Options", "Musique", "Voix", "Bruitages", "Plein \xe9" "cran", "Nettet\xe9 3D", "Continuer"},
    {"Opciones", "M\xfasica", "Voces", "Efectos", "Pantalla completa", "Nitidez 3D", "Continuar"},
    {"Opzioni", "Musica", "Voci", "Effetti", "Schermo intero", "Nitidezza 3D", "Continua"},
    {"Opties", "Muziek", "Stemmen", "Geluiden", "Volledig scherm", "3D-scherpte", "Verder"},
};

static const char *tr(Text t)
{
    static const int lang = [] {
        const char *l = getenv("MICKEY_LANG");
        if (!l || !*l)
            return 0;
        static const char *const codes[][3] = {{"de", "ger", "deu"}, {"en", "eng", "usa"}, {"fr", "fre", "fra"},
                                               {"es", "spa", "esp"}, {"it", "ita", "ita"}, {"nl", "dut", "nld"}};
        for (int i = 0; i < 6; i++)
            for (const char *c : codes[i])
                if (!strncasecmp(l, c, strlen(c)))
                    return i;
        return 1;
    }();
    return kTexts[lang][t];
}

// ---- the panel

static bool s_open;
static int s_row;
static int s_dragging = -1;     // the slider the mouse holds
static Window *s_window;

// Layout in the game's 640x480 coordinates.
static const int PX = 130, PY = 96, PW = 380, PH = 288;
static const int ROW_Y[ROWS] = {164, 200, 236, 274, 310, 352};
static const int LABEL_X = 150, BAR_X = 314, BAR_W = 116, VALUE_X = 488;

static void set_paused(bool paused)
{
    HWND h = main_window();
    if (!h)
        return;
    if (paused) {
        post(h, WM_ACTIVATE, WA_INACTIVE, 0);
        post(h, WM_ACTIVATEAPP, FALSE, 0);
        host::cursor_visible(true);
    } else {
        post(h, WM_ACTIVATEAPP, TRUE, 0);
        post(h, WM_ACTIVATE, WA_ACTIVE, 0);
        host::cursor_visible(false);
    }
}

static void redraw() { port_frame_changed(); }

static void open_panel(Window *w, bool open)
{
    if (open == s_open)
        return;
    s_open = open;
    s_window = w;
    s_row = 0;
    s_dragging = -1;
    set_paused(open);
    redraw();
}

bool options_open() { return s_open; }

static bool fullscreen()
{
    return host::window_is_fullscreen();
}

static void set_fullscreen(bool on)
{
    host::window_fullscreen(on);
    ini_set("fullscreen", on ? 1 : 0);
}

static int sharpness() { return port_scale(); }

static void set_sharpness(int s)
{
    s = s < 1 ? 1 : s > 3 ? 3 : s;
    port_set_scale(s);
    ini_set("scale", s);
}

static void change(int row, int delta)
{
    switch (row) {
    case MUSIC:
    case VOICES:
    case EFFECTS:
        set_volume(row, volume(row) + delta * 10);
        break;
    case FULLSCREEN:
        set_fullscreen(!fullscreen());
        break;
    case SHARPNESS: {
        int s = sharpness() + (delta ? delta : 1);
        if (!delta && s > 3)
            s = 1;
        set_sharpness(s);
        break;
    }
    case DONE:
        open_panel(s_window, false);
        return;
    }
    redraw();
}

static int row_at(int x, int y)
{
    if (x < PX || x >= PX + PW)
        return -1;
    for (int r = 0; r < ROWS; r++)
        if (y >= ROW_Y[r] - 16 && y < ROW_Y[r] + 16)
            return r;
    return -1;
}

static void click(int x, int y)
{
    int r = row_at(x, y);
    if (r < 0)
        return;
    s_row = r;
    if (r <= EFFECTS && x >= BAR_X - 8 && x <= BAR_X + BAR_W + 8) {
        s_dragging = r;
        set_volume(r, (int)lround((x - BAR_X) * 100.0 / BAR_W));
    } else if (r == SHARPNESS && x >= BAR_X && x < BAR_X + 3 * 40) {
        set_sharpness((x - BAR_X) / 40 + 1);
    } else if (r == FULLSCREEN || r == DONE) {
        change(r, 0);
        return;
    }
    redraw();
}

bool options_event(Window *w, const host::Event &e)
{
    if (e.type == host::Event::KeyDown && !e.repeat &&
        (e.vk == host::KeyEscape || (e.vk == host::KeyComma && e.command))) {
        open_panel(w, !s_open);
        return true;
    }
    if (!s_open)
        return false;
    switch (e.type) {
    case host::Event::KeyDown:
        switch (e.vk) {
        case host::KeyUp: s_row = (s_row + ROWS - 1) % ROWS; redraw(); break;
        case host::KeyDown: s_row = (s_row + 1) % ROWS; redraw(); break;
        case host::KeyLeft: change(s_row, -1); break;
        case host::KeyRight: change(s_row, 1); break;
        case host::KeyReturn:
        case host::KeySpace: change(s_row, 0); break;
        }
        return true;
    case host::Event::MouseDown:
        if (e.button == 0)
            click(e.x, e.y);
        return true;
    case host::Event::MouseUp:
        s_dragging = -1;
        return true;
    case host::Event::MouseMove:
        if (s_dragging >= 0) {
            set_volume(s_dragging, (int)lround((e.x - BAR_X) * 100.0 / BAR_W));
            redraw();
        }
        return true;
    case host::Event::KeyUp:
    case host::Event::Text:
    case host::Event::Wheel:
        return true;
    default:
        return false;
    }
}

// ---- drawing, into the frame the window shows (k times the game's 640x480)

namespace {
struct Canvas {
    uint32_t *px;
    int w, h, k;

    void blend(int x, int y, uint32_t c, int alpha)
    {
        if (x < 0 || y < 0 || x >= w || y >= h || alpha <= 0)
            return;
        uint32_t &d = px[(size_t)y * w + x];
        if (alpha >= 255) {
            d = 0xff000000u | c;
            return;
        }
        uint32_t r = (((c >> 16) & 255) * alpha + ((d >> 16) & 255) * (255 - alpha)) / 255;
        uint32_t g = (((c >> 8) & 255) * alpha + ((d >> 8) & 255) * (255 - alpha)) / 255;
        uint32_t b = ((c & 255) * alpha + (d & 255) * (255 - alpha)) / 255;
        d = 0xff000000u | r << 16 | g << 8 | b;
    }
    // a rectangle in the game's coordinates, with rounded corners of radius r
    void rect(double x, double y, double rw, double rh, uint32_t c, int alpha, double r = 0)
    {
        int x0 = (int)(x * k), y0 = (int)(y * k), x1 = (int)((x + rw) * k), y1 = (int)((y + rh) * k);
        double rr = r * k;
        for (int py = y0; py < y1; py++)
            for (int px_ = x0; px_ < x1; px_++) {
                double dx = px_ + 0.5 < x0 + rr ? x0 + rr - (px_ + 0.5) : px_ + 0.5 > x1 - rr ? px_ + 0.5 - (x1 - rr) : 0;
                double dy = py + 0.5 < y0 + rr ? y0 + rr - (py + 0.5) : py + 0.5 > y1 - rr ? py + 0.5 - (y1 - rr) : 0;
                double out = rr > 0 ? sqrt(dx * dx + dy * dy) - rr : -1;
                if (out >= 1)
                    continue;
                int a = out <= 0 ? alpha : (int)(alpha * (1 - out));
                blend(px_, py, c, a);
            }
    }
    // text with its left edge at x and its middle at y (game coordinates)
    void text(const char *s, double x, double y, double size, uint32_t c, bool bold = true, int align = 0)
    {
        static std::map<int, void *> fonts;
        int key = (int)(size * k) * 2 + bold;
        void *&font = fonts[key];
        if (!font)
            font = host::font_create("Arial", size * k, bold, false);
        if (!font)
            return;
        int n = (int)strlen(s), tw = host::text_width(font, s, n) + 2;
        int ascent, descent, leading;
        host::font_metrics(font, &ascent, &descent, &leading);
        int th = ascent + descent;
        std::vector<uint8_t> mask((size_t)tw * th);
        host::text_render(font, s, n, mask.data(), tw, th, descent, true);
        int ox = (int)(x * k) - (align == 2 ? tw : align == 1 ? tw / 2 : 0), oy = (int)(y * k) - th / 2;
        for (int j = 0; j < th; j++)
            for (int i = 0; i < tw; i++)
                blend(ox + i, oy + j, c, mask[(size_t)j * tw + i]);
    }
};
}  // namespace

void options_draw(uint32_t *px, int w, int h, int k)
{
    if (!s_open)
        return;
    Canvas c{px, w, h, k};
    const uint32_t white = 0xffffff, yellow = 0xffc828, dim = 0x9aa8c0, panel = 0x1c2a4a;
    c.rect(0, 0, 640, 480, 0x000000, 110);                     // the game, darkened
    c.rect(PX - 2, PY - 2, PW + 4, PH + 4, white, 230, 16);    // border
    c.rect(PX, PY, PW, PH, panel, 245, 14);
    c.text(tr(T_TITLE), PX + PW / 2.0, PY + 28, 22, yellow, true, 1);
    const char *labels[ROWS] = {tr(T_MUSIC), tr(T_VOICES), tr(T_SOUNDS), tr(T_FULLSCREEN), tr(T_SHARPNESS),
                                tr(T_CONTINUE)};
    for (int r = 0; r < ROWS; r++) {
        int y = ROW_Y[r];
        if (r == s_row && r != DONE)
            c.rect(PX + 10, y - 15, PW - 20, 30, 0x34486e, 255, 8);
        if (r == DONE) {
            bool sel = s_row == DONE;
            c.rect(PX + PW / 2 - 60, y - 15, 120, 30, sel ? yellow : 0x34486e, 255, 15);
            c.text(labels[r], PX + PW / 2.0, y, 16, sel ? panel : white, true, 1);
            continue;
        }
        c.text(labels[r], LABEL_X, y, 15, r == s_row ? yellow : white);
        if (r <= EFFECTS) {
            int v = volume(r);
            c.rect(BAR_X, y - 3, BAR_W, 6, dim, 255, 3);
            c.rect(BAR_X, y - 3, BAR_W * v / 100.0, 6, yellow, 255, 3);
            c.rect(BAR_X + BAR_W * v / 100.0 - 7, y - 7, 14, 14, white, 255, 7);
            char s[8];
            snprintf(s, sizeof(s), "%d%%", v);
            c.text(s, VALUE_X, y, 13, white, false, 2);
        } else if (r == FULLSCREEN) {
            bool on = fullscreen();
            c.rect(BAR_X, y - 9, 18, 18, white, 255, 4);
            if (on)
                c.rect(BAR_X + 4, y - 5, 10, 10, yellow, 255, 2);
        } else if (r == SHARPNESS) {
            int s = sharpness();
            for (int i = 1; i <= 3; i++) {
                char t[4];
                snprintf(t, sizeof(t), "%dx", i);
                c.rect(BAR_X + (i - 1) * 40, y - 11, 34, 22, i == s ? yellow : 0x34486e, 255, 6);
                c.text(t, BAR_X + (i - 1) * 40 + 17, y, 13, i == s ? panel : white, true, 1);
            }
        }
    }
}

}  // namespace win32

extern "C" {

// The volume of a channel of the game's sound manager, in percent: 20 carries the
// voices, 10 the music (its background music stream is marked with port_music_stream),
// the others the sound effects.
int port_sound_gain(int channel)
{
    using namespace win32;
    return volume(channel == 20 ? VOICES : channel == 10 ? MUSIC : EFFECTS);
}

int port_music_gain(void) { return win32::volume(win32::MUSIC); }

// Changes whenever a volume changes: the channels apply their new volume then.
unsigned port_sound_gain_version(void) { return win32::s_volume_version; }

// port.ini settings for the other layers (3D sharpness, full screen), or fallback.
int port_setting(const char *key, int fallback) { return win32::ini_int(key, fallback); }
}
