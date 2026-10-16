#pragma once
#include "BasicObject.h"
#include "TextPrimitive.h"

template <class T> class Vec;
class istream;

// Copy-on-write string: Texts share one TextPrimitive until one of them
// is written to.
class Text : public BasicObject {
public:
    Text(const char *);
    Text(const Text &t) : text(t.text) { text->ref(); }
    Text(int n) : text(0)
    {
        copyOnWrite();
        text->verify(n);
    }
    Text() : text(0) { copyOnWrite(); }
    virtual ~Text();
    Text &operator=(const char *);
    Text &operator=(const Text &rhs)
    {
        text->unref();
        text = rhs.text;
        text->ref();
        return *this;
    }
    const char *getConstPtr() const { return text->e; }
    operator const char *() const { return text->e; }
    operator char *()
    {
        if (text->numRefs() > 1)
            copyOnWrite();
        return text->e;
    }
    int operator!() const;
    int operator==(const char *) const;
    int operator==(const Text &) const;
    int operator!=(const char *) const;
    int operator!=(const Text &) const;
    int operator<(const char *) const;
    int operator<(const Text &) const;
    char &operator[](int);
    const char &operator[](int) const;
    Text operator+(const char *) const;
    Text operator+(const Text &) const;
    Text &operator+=(const char *s)
    {
        if (text->numRefs() > 1)
            copyOnWrite();
        text->append(s);
        return *this;
    }
    Text &operator+=(const Text &);
    Text &operator<<(const char *);
    Text &operator<<(const Text &);
    Text &append(const char *);
    int isEmpty() const;
    int isWhiteSpace() const;
    int contains(char) const;
    int contains(const char *) const;
    int contains(const Text &) const;
    int length() const { return text->length(); }
    int subText(Text &, int, int) const;
    int find(char, int) const;
    int find(const char *, int) const;
    int find(const Text &, int) const;
    int findLast(char) const;
    void tokenize(Vec<Text> &, char) const;
    void tokenize(Vec<Text> &, const char *) const;
    int remove(int, int);
    int replaceWith(const char *, int, int);
    int replaceWith(const Text &, int, int);
    void insert(const char *, int);
    void insert(const Text &, int);
    void toUpper();
    void toLower();
    void clear()
    {
        if (text->numRefs() > 1)
            createUniquePrim();
        text->e[0] = 0;
    }
protected:
    TextPrimitive *text;
    void createUniquePrim()
    {
        text->unref();
        text = new TextPrimitive(31);
        text->ref();
    }
    void copyOnWrite();
};

istream &operator>>(istream &, Text &);

// Inline (no out-of-line copy; SearchTraverser::Visit inlines it twice).
#include <ctype.h>
inline void Text::toLower()
{
    for (char *p = *this; *p; p++)
        *p = (char)tolower(*p);
}

// Inline (no out-of-line copy; SearchTraverser::Visit).
inline int Text::contains(const char *s) const
{
    int found = find(s, 0) >= 0;
    return found;
}
