// MfStringHeap (mfutil.dll 0x1000f860-0x1000fa10).
#include "MfStringHeap.h"

// 0x1000f860 ??0MfStringHeap@@QAE@XZ
MfStringHeap::MfStringHeap()
{
}

// 0x1000f890 ??1MfStringHeap@@UAE@XZ
// 0x1000f900 ??_EMfStringHeap@@UAEPAXI@Z (compiler-generated)
// 0x1000fa00 ??_EMfStringHeap@@WBA@AEPAXI@Z (compiler-generated this-adjustor thunk)
MfStringHeap::~MfStringHeap()
{
}

// 0x1000f950 ?AllocString@MfStringHeap@@QAEPAVMfString@@XZ
MfString *MfStringHeap::AllocString()
{
    MfString s(NULL_STRING);
    return MfStringList::Add(s);
}

// 0x1000f9b0 ?ReleaseString@MfStringHeap@@QAEXPAVMfString@@@Z
void MfStringHeap::ReleaseString(MfString *s)
{
    MfDblListItem *pli = MfDblList::Find(s);
    if (pli) {
        MfDblList::Delete(pli);
        delete s;
    }
}

// 0x1000f9f0 ?ReleaseAll@MfStringHeap@@QAEXXZ
void MfStringHeap::ReleaseAll()
{
    MfStringList::Clear();
}
