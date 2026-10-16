// Motion Factory utility library (mfutil.dll): MfStrStack (0x1000fd70-0x1000ff80).
#include "MfStrStack_unit.h"

// Its CallBack(void *) is identical to MfStringListDeletorCB::CallBack and was
// folded with it by the linker (0x1000fd20).
class MfStrStack::MfStrStackDeletorCB : public MfCallBack {
public:
    virtual int CallBack(void *a);
};

// 0x1000fd70 ??0MfStrStack@@QAE@XZ (inline constructor; this object keeps the copy)
// 0x1000fd90 ??1MfStrStack@@UAE@XZ
MfStrStack::~MfStrStack()
{
    Clear();
}

// 0x1000fde0 ??_EMfStrStack@@UAEPAXI@Z (compiler-generated)

// 0x1000fe30 ?Push@MfStrStack@@QAEXABVMfString@@@Z
void MfStrStack::Push(const MfString &s)
{
    Append(new MfString(s));
}

// 0x1000fea0 ?Pop@MfStrStack@@QAE?AVMfString@@XZ
MfString MfStrStack::Pop()
{
    MfString s;
    MfString *p = (MfString *)MfDblList::DeleteLast();
    if (p) {
        s = *p;
        delete p;
    }
    return s;
}

// 0x1000ff40 ?Clear@MfStrStack@@UAEXXZ
void MfStrStack::Clear()
{
    MfStrStackDeletorCB cb;
    MfDblList::Clear(cb);
}

int MfStrStack::MfStrStackDeletorCB::CallBack(void *a)
{
    delete (MfString *)a;
    return 1;
}

// Inline or compiler-generated members this object emits:
// 0x10005e20 ?GetFirst@MfDblList@@UBEPAXAAPAVMfDblListItem@@@Z
// 0x10005e40 ?GetNext@MfDblList@@UBEPAXAAPAVMfDblListItem@@@Z
