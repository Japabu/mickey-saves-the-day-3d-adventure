// MfScObjString (PiccoloRT.dll): the Piccolo string object (0x1c bytes); the
// string itself is the second base (at 0x14). As in src/MfPoLibrary.
#ifndef RUNTIMELIB_MFSCOBJSTRING_H
#define RUNTIMELIB_MFSCOBJSTRING_H
#include "MfScData.h"

class __declspec(dllimport) MfScObjString : public MfScObject, public MfString {
public:
    MfScObjString(int readOnly = 0);
    virtual ~MfScObjString();
    int IsReadOnly() const { return m_readOnly; }
private:
    int m_readOnly;                                         // 0x18
};

#endif
