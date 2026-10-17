// MfStack (mfutil.dll, 0x1000bde0-0x1000bee0)
#include "MfStack.h"

// 0x1000bde0 ??0MfStack@@QAE@H@Z
MfStack::MfStack(int nMax) : nMax(nMax)
{
}

// 0x1000be00 ??1MfStack@@UAE@XZ  (compiler generated)

// 0x1000be10 ??_FMfStack@@QAEXXZ  (compiler generated default constructor closure)
// 0x1000be20 ??_EMfStack@@UAEPAXI@Z  (compiler generated)

// 0x1000be70 ?Push@MfStack@@QAEHPAX@Z
int MfStack::Push(void *pData)
{
    if (IsFull() == 1)
        return 0;
    Prepend(pData);
    return 1;
}

// 0x1000bea0 ?Pop@MfStack@@QAEPAXXZ
void *MfStack::Pop()
{
    return DeleteFirst();
}

// 0x1000beb0 ?IsFull@MfStack@@QAEHXZ
int MfStack::IsFull()
{
    if (nMax == -1)
        return 0;
    return GetCount() >= nMax;
}

// 0x1000bed0 ?IsEmpty@MfStack@@QAEHXZ
int MfStack::IsEmpty()
{
    return GetCount() == 0;
}


// Also matched by this code:
// runtimelib.dll 0x1007dc50 ??0MfStack@@QAE@H@Z
// runtimelib.dll 0x10087df0 ??0MfStack@@QAE@H@Z
// runtimelib.dll 0x1008a890 ??0MfStack@@QAE@H@Z