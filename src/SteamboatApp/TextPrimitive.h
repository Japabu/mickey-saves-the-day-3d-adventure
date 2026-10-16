#pragma once
#include <string.h>
#include "BasicObject.h"

// The shared, reference-counted character buffer behind Text.
struct TextPrimitive : public BasicObject {
    TextPrimitive(const TextPrimitive &p) : e(0), alloclen(0)
    {
        verify(p.alloclen - 1);
        strcpy(e, p.e);
    }
    TextPrimitive(int n) : e(0), alloclen(0) { verify(n); }
    char *e;
    int length() const { return strlen(e); }
    void verify(int);
    int remove(int, int);
    void insert(const char *, int, int);
    void append(const char *);
    void extract(char *, int, int) const;
protected:
    virtual ~TextPrimitive();
private:
    int alloclen;
};
