// text.cpp - the host's fonts through CoreText (macOS). Windows faces map to the macOS
// fonts of the same name (Arial, Arial Black, Times New Roman, Comic Sans MS, Verdana,
// Courier New are on every Mac); the system faces become Arial.
#include "../host.h"

#include <CoreText/CoreText.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <vector>

namespace host {

void *font_create(const char *face, double px, bool bold, bool italic)
{
    if (!face || !*face || !strcasecmp(face, "System") || !strcasecmp(face, "MS Sans Serif") ||
        !strcasecmp(face, "MS Shell Dlg") || !strcasecmp(face, "Fixedsys"))
        face = "Arial";
    CFStringRef name = CFStringCreateWithCString(0, face, kCFStringEncodingWindowsLatin1);
    CTFontRef f = CTFontCreateWithName(name, px, 0);
    CFRelease(name);
    CTFontSymbolicTraits traits = (bold ? kCTFontBoldTrait : 0) | (italic ? kCTFontItalicTrait : 0);
    if (traits) {
        CTFontRef styled = CTFontCreateCopyWithSymbolicTraits(f, px, 0, traits, traits);
        if (styled) {
            CFRelease(f);
            f = styled;
        }
    }
    return (void *)f;
}

void font_release(void *font)
{
    if (font)
        CFRelease((CTFontRef)font);
}

void font_metrics(void *font, int *ascent, int *descent, int *leading)
{
    CTFontRef f = (CTFontRef)font;
    *ascent = (int)ceil(CTFontGetAscent(f));
    *descent = (int)ceil(CTFontGetDescent(f));
    *leading = (int)ceil(CTFontGetLeading(f));
}

static CTLineRef line_of(void *font, const char *s, int n)
{
    CFStringRef str = CFStringCreateWithBytes(0, (const UInt8 *)s, n, kCFStringEncodingWindowsLatin1, false);
    if (!str)
        return 0;
    // the glyphs take the context's fill colour (the default would be black: an empty mask)
    CFStringRef keys[] = {kCTFontAttributeName, kCTForegroundColorFromContextAttributeName};
    CFTypeRef values[] = {(CTFontRef)font, kCFBooleanTrue};
    CFDictionaryRef attrs = CFDictionaryCreate(0, (const void **)keys, (const void **)values, 2,
                                               &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFAttributedStringRef as = CFAttributedStringCreate(0, str, attrs);
    CTLineRef line = CTLineCreateWithAttributedString(as);
    CFRelease(as);
    CFRelease(attrs);
    CFRelease(str);
    return line;
}

int text_width(void *font, const char *s, int n)
{
    if (n <= 0)
        return 0;
    CTLineRef line = line_of(font, s, n);
    if (!line)
        return 0;
    int w = (int)ceil(CTLineGetTypographicBounds(line, 0, 0, 0));
    CFRelease(line);
    return w;
}

void text_render(void *font, const char *s, int n, uint8_t *mask, int w, int h, int descent, bool aa)
{
    std::vector<uint8_t> buf((size_t)w * (size_t)h);
    CGColorSpaceRef gray = CGColorSpaceCreateDeviceGray();
    CGContextRef cg = CGBitmapContextCreate(buf.data(), (size_t)w, (size_t)h, 8, (size_t)w, gray, kCGImageAlphaNone);
    CGColorSpaceRelease(gray);
    if (!cg)
        return;
    CGContextSetShouldAntialias(cg, aa);
    CGContextSetShouldSmoothFonts(cg, false);
    CGContextSetGrayFillColor(cg, 1.0, 1.0);
    CGContextSetTextPosition(cg, 0, descent);
    if (CTLineRef line = line_of(font, s, n)) {
        CTLineDraw(line, cg);
        CFRelease(line);
    }
    CGContextRelease(cg);
    // The bitmap context's memory holds the top row first.
    memcpy(mask, buf.data(), buf.size());
}

}  // namespace host
