// PiccoloComp.dll MfPcConstantTable.cpp (0x1000b420-0x1000b810)
#include "MfPcConstantTable.h"
#include "MfDblList.h"
#include "MfScDiagnostics.h"
#include "MfString.h"
#include "MfVariant.h"

class MfScDataHandle;

// PiccoloRT.dll
class __declspec(dllimport) MfScDataHandle {   // own copy: src/PiccoloRT/MfScDataHandle.h opens windows.h, which this unit does not
public:
    static MfScDataHandle *CreateData(MfVariant const &value, int flags);
};

// 0x1000b420 ??0MfPcConstantTable@@QAE@XZ
MfPcConstantTable::MfPcConstantTable()
{
    mConstants = new MfDblList;
}

// 0x1000b460 ??_GMfPcConstantTable@@UAEPAXI@Z (compiler-generated)

// 0x1000b480 ??1MfPcConstantTable@@UAE@XZ
MfPcConstantTable::~MfPcConstantTable()
{
    Clear();
    delete mConstants;
}

// 0x1000b4a0 ?Add@MfPcConstantTable@@QAEHABVMfVariant@@@Z
int MfPcConstantTable::Add(MfVariant const &value)
{
    int index = Find(value);
    if (index < 0) {
        mConstants->Add(new MfVariant(value));
        index = mConstants->GetCount() - 1;
    }
    return index;
}

// 0x1000b530 ??_GMfVariant@@UAEPAXI@Z (new MfVariant)

// 0x1000b550 ?Find@MfPcConstantTable@@QAEHABVMfVariant@@@Z
int MfPcConstantTable::Find(MfVariant const &value)
{
    MfDblListItem *pli;
    int i = 0;
    MfVariant *constant = (MfVariant *)mConstants->GetFirst(pli);
    while (i < mConstants->GetCount() && constant && !(value == *constant)) {
        i++;
        constant = (MfVariant *)mConstants->GetNext(pli);
    }
    if (i >= mConstants->GetCount())
        return -1;
    return i;
}

// 0x1000b5c0 ?Clear@MfPcConstantTable@@QAEXXZ
void MfPcConstantTable::Clear()
{
    MfPcConstantTableCB cb;
    mConstants->Clear(cb);
}

// 0x1000b610 ??1MfPcConstantTableCB@MfPcConstantTable@@UAE@XZ (inline)

// 0x1000b620 ?CallBack@MfPcConstantTableCB@MfPcConstantTable@@UAEHPAX@Z
int MfPcConstantTable::MfPcConstantTableCB::CallBack(void *p)
{
    delete (MfVariant *)p;
    return 1;
}

// 0x1000b640 ?FillConstantPool@MfPcConstantTable@@QAEXPAPAVMfScDataHandle@@@Z
void MfPcConstantTable::FillConstantPool(MfScDataHandle **pool)
{
    MfDblListItem *pli;
    int i = 0;
    MfVariant *constant = (MfVariant *)mConstants->GetFirst(pli);
    while (i < mConstants->GetCount() && constant) {
        pool[i] = MfScDataHandle::CreateData(*constant, 1);
        i++;
        constant = (MfVariant *)mConstants->GetNext(pli);
    }
}

// 0x1000b6a0 ?Dump@MfPcConstantTable@@QAEXXZ
void MfPcConstantTable::Dump()
{
    if (MfScDiagnostics::Active(4)) {
        MfString line(MfStringSize(0x200));
        MfDblListItem *pli;
        int i = 0;
        MfVariant *constant = (MfVariant *)mConstants->GetFirst(pli);
        while (i < mConstants->GetCount() && constant) {
            line.Format("CPP[%d] = %s (", i, (char *)MfString(*constant));
            switch (constant->Type()) {
            case 1: line << "int"; break;
            case 2: line << "unsigned int"; break;
            case 3: line << "float"; break;
            case 4: line << "double"; break;
            case 5: line << "string"; break;
            default: line << "unknown"; break;
            }
            line << ")";
            MfScDiagnostics::PrintNl(4, line);
            i++;
            constant = (MfVariant *)mConstants->GetNext(pli);
        }
    }
}
