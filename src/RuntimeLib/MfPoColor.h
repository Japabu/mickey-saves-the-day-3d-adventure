// MfPoColor (MfPoLibrary.dll): the Piccolo object of a colour. Only what
// RuntimeLib uses.
#ifndef RUNTIMELIB_MFPOCOLOR_H
#define RUNTIMELIB_MFPOCOLOR_H
#include "MfPoBase.h"
#include "MfColor.h"

class __declspec(dllimport) MfPoColor : public MfPoBase {
public:
    virtual ~MfPoColor();
    MfColor &Color() { return mColor; }

    MfColor mColor;                                         // 0x14
};

#endif
