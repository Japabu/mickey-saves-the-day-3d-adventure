#pragma once
#include "MfStringList.h"
class MfString;

// mfutil.dll: a list of heap MfStrings handed out by AllocString. Only the
// members are imported: the constructor is inline and PiccoloComp has its own
// copy of the vtable.
class MfStringHeap : public MfStringList {   // own copy: inline constructor and a vtable of this DLL's own; src/mfutil/MfStringHeap.h is fully exported
public:
    MfStringHeap() {}
    __declspec(dllimport) virtual ~MfStringHeap();
    __declspec(dllimport) MfString *AllocString();
    __declspec(dllimport) void ReleaseString(MfString *s);
    __declspec(dllimport) void ReleaseAll();
};
