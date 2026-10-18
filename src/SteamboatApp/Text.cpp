#include <string.h>
#include <iostream.h>
#include <strstrea.h>
#include "Text.h"
#include "TextPrimitive.h"
#include "Vec.h"

// 0x100dd6c0 ??1TextPrimitive@@MAE@XZ
TextPrimitive::~TextPrimitive()
{
    delete [] e;
}

// 0x100dd730 ?verify@TextPrimitive@@QAEXH@Z
void TextPrimitive::verify(int n)
{
    int size = n + 1;
    if (size <= 32)
        size = 32;
    if (size > alloclen) {
        if (alloclen * 2 > size)
            size = alloclen * 2;
        char *old = e;
        if (old) {
            e = new char[size];
            e[0] = 0;
            if (*old)
                strcpy(e, old);
            delete [] old;
        } else {
            e = new char[size];
            e[0] = 0;
        }
        alloclen = size;
    }
}

// 0x100dd7d0 ?remove@TextPrimitive@@QAEHHH@Z
int TextPrimitive::remove(int pos, int n)
{
    int len = length();
    if (pos >= len)
        return 0;
    if (n != 0) {
        if (n < 0) {
            e[pos] = 0;
        } else {
            if (len - pos < n)
                n = len - pos;
            int end = len - n + 1;
            for (int i = pos; i < end; i++)
                e[i] = e[i + n];
        }
    }
    return 1;
}

// 0x100dd840 ?insert@TextPrimitive@@QAEXPBDHH@Z
void TextPrimitive::insert(const char *s, int pos, int n)
{
    int len = length();
    if (pos > len || n == 0)
        return;
    int count = (n > 0) ? n : strlen(s);
    verify(count + len);
    for (int i = len; i >= pos; i--)
        e[i + count] = e[i];
    strncpy(e + pos, s, count);
}

// 0x100dd8b0 ?append@TextPrimitive@@QAEXPBD@Z
void TextPrimitive::append(const char *s)
{
    int len = length();
    int n = strlen(s);
    verify(n + len);
    strncpy(e + len, s, n + 1);
}

// 0x100dd900 ?extract@TextPrimitive@@QBEXPADHH@Z
void TextPrimitive::extract(char *buf, int pos, int n) const
{
    int count = (n < 0) ? length() - pos : n;
    strncpy(buf, e + pos, count);
    buf[count] = 0;
}

// 0x100dd940 ??0Text@@QAE@PBD@Z
Text::Text(const char *s) : text(0)
{
    copyOnWrite();
    if (s == 0) {
        text->e[0] = 0;
        return;
    }
    text->verify(strlen(s));
    strcpy(text->e, s);
}

// 0x100dda00 ??1Text@@UAE@XZ
Text::~Text()
{
    text->unref();
}

// 0x100dda60 ??4Text@@QAEAAV0@PBD@Z
Text &Text::operator=(const char *s)
{
    if (text->numRefs() > 1)
        copyOnWrite();
    text->e[0] = 0;
    text->verify(strlen(s));
    strcpy(text->e, s);
    return *this;
}

// 0x100ddad0 ?copyOnWrite@Text@@IAEXXZ
void Text::copyOnWrite()
{
    if (text) {
        TextPrimitive *old = text;
        text = new TextPrimitive(*old);
        old->unref();
    } else {
        text = new TextPrimitive(31);
    }
    text->ref();
}

// 0x100ddbf0 ??5@YAAAVistream@@AAV0@AAVText@@@Z
istream &operator>>(istream &is, Text &t)
{
    strstreambuf buf;
    is.get(buf);
    is.ignore();
    t = buf.str();
    return is;
}

// 0x100ddcb0 ?subText@Text@@QBEHAAV1@HH@Z
int Text::subText(Text &t, int pos, int n) const
{
    int len = length();
    if (pos < 0 || pos > len || pos + n > len)
        return 0;
    if (n < 0)
        n = len - pos;
    if (t.text->numRefs() > 1)
        t.copyOnWrite();
    t.text->verify(n);
    text->extract(t.text->e, pos, n);
    return 1;
}

// 0x100ddd30 ?find@Text@@QBEHPBDH@Z
int Text::find(const char *s, int pos) const
{
    if (pos >= 0) {
        const char *str = text->e;
        if (pos < (int)strlen(str) && s) {
            const char *p = strstr(str + pos, s);
            if (p)
                return p - str;
        }
    }
    return -1;
}

// 0x100ddd80 ?find@Text@@QBEHDH@Z
int Text::find(char c, int pos) const
{
    if (pos >= 0 && pos < length()) {
        char *p = strchr(text->e + pos, c);
        if (p)
            return p - text->e;
    }
    return -1;
}

// 0x100dddd0 ?findLast@Text@@QBEHD@Z
int Text::findLast(char c) const
{
    char *p = strrchr(text->e, c);
    if (p)
        return p - text->e;
    return -1;
}

// TODO 0x100dde00 ?tokenize@Text@@QBEXAAV?$Vec@VText@@@@PBD@Z: (94% similar) Text temporaries get swapped frame slots and strlen result is spilled
void Text::tokenize(Vec<Text> &tokens, const char *delimiters) const
{
    int pos = 0;
    tokens.setLength(0);
    if (length() == 0)
        return;
    int n;
    do {
        pos += strspn(text->e + pos, delimiters);
        n = strcspn(text->e + pos, delimiters);
        if (n > 0) {
            Text token;
            subText(token, pos, n);
            tokens.append(token);
        }
        pos += n;
    } while (n > 0);
}

// EQUIV 0x100de0f0 ?replaceWith@Text@@QAEHPBDHH@Z: reviewed: register allocation only (the original keeps text in ebx, s in ebp, pos in esi and spills slen to its argument slot); same tests, remove/insert/strncpy calls and arguments
int Text::replaceWith(const char *s, int pos, int n)
{
    if (text->numRefs() > 1)
        copyOnWrite();
    int len = length();
    int slen = strlen(s);
    if (pos < 0 || pos + n > len)
        return 0;
    int count = (n < len - pos) ? n : len - pos;
    int diff = count - slen;
    if (diff > 0) {
        text->remove(pos, diff);
    } else if (diff < 0) {
        diff = -diff;
        text->insert(s, pos, diff);
        s += diff;
        pos += diff;
        slen -= diff;
    }
    strncpy(text->e + pos, s, slen);
    return 1;
}


// Matched by this code:
// 0x100d8180 ??1BasicObject@@MAE@XZ
// 0x100dd710 ??_GTextPrimitive@@MAEPAXI@Z
// 0x100de080 ?unref@BasicObject@@QBEXXZ
// 0x100de0a0 ??0Text@@QAE@XZ
// 0x100de1b0 ?reallocMem@?$Vec@VText@@@@IAEXHHH@Z
// 0x100de400 ??_EText@@UAEPAXI@Z

Text &Text::operator+=(const Text &t)
{
    return *this += (const char *)t;
}
