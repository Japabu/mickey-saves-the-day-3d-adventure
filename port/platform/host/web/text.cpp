// text.cpp - the web host's fonts: the browser draws the text on an OffscreenCanvas in
// the calling thread (a worker). Windows faces fall back to the page's sans-serif when
// the system has no font of that name.
#include "../host.h"

#include <emscripten.h>

#include <string.h>
#include <strings.h>

namespace host {

struct WebFont {
    char css[160];   // the CSS font: style, weight, size, families
};

// clang-format off
EM_JS(void, web_text_metrics, (const char *css, int *out), {
    const c = new OffscreenCanvas(1, 1).getContext('2d');
    c.font = UTF8ToString(css);
    const m = c.measureText('Hg');
    const ascent = Math.ceil(m.fontBoundingBoxAscent ?? m.actualBoundingBoxAscent);
    const descent = Math.ceil(m.fontBoundingBoxDescent ?? m.actualBoundingBoxDescent);
    HEAP32[out >> 2] = ascent;
    HEAP32[(out >> 2) + 1] = descent;
});

EM_JS(int, web_text_width, (const char *css, const char *s, int n), {
    const c = new OffscreenCanvas(1, 1).getContext('2d');
    c.font = UTF8ToString(css);
    const text = new TextDecoder('windows-1252').decode(HEAPU8.slice(s, s + n));
    return Math.ceil(c.measureText(text).width);
});

EM_JS(void, web_text_render, (const char *css, const char *s, int n, uint8_t *mask, int w, int h, int descent, int aa), {
    const canvas = new OffscreenCanvas(w, h);
    const c = canvas.getContext('2d');
    c.font = UTF8ToString(css);
    c.fillStyle = '#fff';
    c.textBaseline = 'alphabetic';
    const text = new TextDecoder('windows-1252').decode(HEAPU8.slice(s, s + n));
    c.fillText(text, 0, h - descent);
    const px = c.getImageData(0, 0, w, h).data;
    const out = new Uint8Array(w * h);
    for (let i = 0; i < w * h; i++) {
        const a = px[i * 4 + 3];
        out[i] = aa ? a : (a >= 128 ? 255 : 0);
    }
    HEAPU8.set(out, mask);
});
// clang-format on

void *font_create(const char *face, double px, bool bold, bool italic)
{
    if (!face || !*face || !strcasecmp(face, "System") || !strcasecmp(face, "MS Sans Serif") ||
        !strcasecmp(face, "MS Shell Dlg") || !strcasecmp(face, "Fixedsys"))
        face = "Arial";
    WebFont *f = new WebFont;
    snprintf(f->css, sizeof(f->css), "%s%s%.2fpx \"%s\", Arial, sans-serif", italic ? "italic " : "",
             bold ? "bold " : "", px, face);
    return f;
}

void font_release(void *font) { delete (WebFont *)font; }

void font_metrics(void *font, int *ascent, int *descent, int *leading)
{
    int m[2] = {0, 0};
    web_text_metrics(((WebFont *)font)->css, m);
    *ascent = m[0];
    *descent = m[1];
    *leading = 0;
}

int text_width(void *font, const char *s, int n)
{
    return n > 0 ? web_text_width(((WebFont *)font)->css, s, n) : 0;
}

void text_render(void *font, const char *s, int n, uint8_t *mask, int w, int h, int descent, bool aa)
{
    if (n > 0 && w > 0 && h > 0)
        web_text_render(((WebFont *)font)->css, s, n, mask, w, h, descent, aa);
    else
        memset(mask, 0, (size_t)w * (size_t)h);
}

}  // namespace host
