// Motion Factory utility library (mfutil.dll): MfStrStack, a LIFO stack of heap
// MfStrings on top of MfDblList. Reconstructed declarations (no PDB). MfStrStack.cpp
// keeps the copies of MfDblList::GetFirst/GetNext (inline in MfDblList.h).
#ifndef MFUTIL_MFSTRSTACK_H
#define MFUTIL_MFSTRSTACK_H

#include <stdlib.h>
#include "mfutil_api.h"
#include "MfString.h"
#include "MfDblList.h"

// Size 0x10. vtable 0x100178a4; Clear() overrides MfDblList::Clear (+0x54).
class MFUTIL_API MfStrStack : public MfDblList {
public:
    // Clear: CallBack(a) deletes the MfString (vtable 0x1001791c)
    class MfStrStackDeletorCB;

    MfStrStack() {}
    virtual ~MfStrStack();
    void Push(const MfString &s);
    MfString Pop();
    int Count();    // exported
    virtual void Clear();
};

#endif
