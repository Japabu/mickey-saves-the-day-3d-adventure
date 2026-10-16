// MfScObjString (PiccoloRT.dll): the Piccolo string object (0x1c bytes); the
// string itself is the second base.
#pragma once
#include "MfScObject.h"

class __declspec(dllimport) MfScObjString : public MfScObject, public MfString {
public:
    MfScObjString(int readOnly = 0);
    virtual ~MfScObjString();
    int IsReadOnly() const { return m_readOnly; }
    // the string itself (MfPoLibrary reads string arguments through it)
    MfString *String() { return this; }
private:
    int m_readOnly;                                         // 0x18
};
