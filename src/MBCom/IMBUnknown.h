// IMBUnknown: the root interface of the muffin component model (muffin/include/Com):
// reference counting and interface queries by MBGUID. No code of its own.
#ifndef IMBUNKNOWN_H
#define IMBUNKNOWN_H
#include "MBGUID.h"

// The interface id is a header constant: every unit that uses it has its own copy
// (internal linkage, in that unit's data).
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &iid, void **obj) = 0;  // vftable+0x00
    virtual unsigned long AddRef(void) = 0;                          // vftable+0x04
    virtual unsigned long Release(void) = 0;                         // vftable+0x08
};

#endif
