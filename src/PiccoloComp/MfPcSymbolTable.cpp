// PiccoloComp.dll MfPcSymbolTable.cpp (0x1000f0e0-0x1000f4a0)
#include "MfPcSymbolTable.h"
#include "MfPcIdentifier.h"
#include "MfDictionary.h"
#include "MfDblList.h"

// 0x1000f0e0 ??0MfPcSymbolTable@@QAE@XZ
MfPcSymbolTable::MfPcSymbolTable()
{
    mSymbols = new MfDictionary(2000);
    mIdentifiers = new MfDblList;
    mNumLocals = 0;
    mLevel = 0;
    mMaxLocals = 0;
}

// 0x1000f180 ??_GMfPcSymbolTable@@UAEPAXI@Z (compiler-generated)
// 0x1000f1a0 ??_GMfDictionary@@UAEPAXI@Z (new MfDictionary)

// 0x1000f1c0 ??1MfPcSymbolTable@@UAE@XZ
MfPcSymbolTable::~MfPcSymbolTable()
{
    delete mSymbols;
    delete mIdentifiers;
}

// 0x1000f1f0 ?Add@MfPcSymbolTable@@QAEPAVMfPcIdentifier@@ABVMfString@@@Z
MfPcIdentifier *MfPcSymbolTable::Add(MfString const &name)
{
    MfPcIdentifier *id = new MfPcIdentifier(name);
    MfPcIdentifier *existing = (MfPcIdentifier *)mSymbols->Add(name, id);
    if (existing) {
        delete id;
        id = existing;
    } else
        mIdentifiers->Add(id);
    return id;
}

// 0x1000f280 ?Find@MfPcSymbolTable@@QAEPAVMfPcIdentifier@@ABVMfString@@@Z
MfPcIdentifier *MfPcSymbolTable::Find(MfString const &name)
{
    return (MfPcIdentifier *)mSymbols->Find(name);
}

// 0x1000f2a0 ?Reset@MfPcSymbolTable@@QAEXXZ
void MfPcSymbolTable::Reset()
{
    mNumLocals = 0;
    mLevel = 0;
    mMaxLocals = 0;
    MfDblListItem *pli;
    MfPcIdentifier *id = (MfPcIdentifier *)mIdentifiers->GetFirst(pli);
    while (id) {
        if (id->mKind != PC_ID_GCONST && id->mKind != PC_ID_PERM && id->mKind != PC_ID_GTEMP) {
            mSymbols->Delete(id->mName);
            mIdentifiers->Delete(pli);
            id = (MfPcIdentifier *)mIdentifiers->GetCurrent(pli);
        } else
            id = (MfPcIdentifier *)mIdentifiers->GetNext(pli);
    }
}

// 0x1000f320 ?FlushLocals@MfPcSymbolTable@@QAEXXZ
void MfPcSymbolTable::FlushLocals()
{
    mNumLocals = 0;
    mLevel = 0;
    mMaxLocals = 0;
    MfDblListItem *pli;
    MfPcIdentifier *id = (MfPcIdentifier *)mIdentifiers->GetFirst(pli);
    while (id) {
        if (id->mKind != PC_ID_PERM && id->mKind != PC_ID_GTEMP) {
            mSymbols->Delete(id->mName);
            mIdentifiers->Delete(pli);
            id = (MfPcIdentifier *)mIdentifiers->GetCurrent(pli);
        } else
            id = (MfPcIdentifier *)mIdentifiers->GetNext(pli);
    }
}

// 0x1000f3a0 ?FlushGlobalTemps@MfPcSymbolTable@@QAEXXZ
void MfPcSymbolTable::FlushGlobalTemps()
{
    MfDblListItem *pli;
    MfPcIdentifier *id = (MfPcIdentifier *)mIdentifiers->GetFirst(pli);
    while (id) {
        if (id->mKind == PC_ID_GTEMP) {
            mSymbols->Delete(id->mName);
            mIdentifiers->Delete(pli);
            id = (MfPcIdentifier *)mIdentifiers->GetCurrent(pli);
        } else
            id = (MfPcIdentifier *)mIdentifiers->GetNext(pli);
    }
}

// 0x1000f410 ?EndBlock@MfPcSymbolTable@@QAEHXZ
int MfPcSymbolTable::EndBlock()
{
    if (mNumLocals > mMaxLocals)
        mMaxLocals = mNumLocals;
    int removed = 0;
    MfDblListItem *pli;
    MfPcIdentifier *id = (MfPcIdentifier *)mIdentifiers->GetFirst(pli);
    while (id) {
        if (id->mKind == PC_ID_LOCAL && id->mLevel == mLevel) {
            removed++;
            mSymbols->Delete(id->mName);
            mIdentifiers->Delete(pli);
            id = (MfPcIdentifier *)mIdentifiers->GetCurrent(pli);
        } else
            id = (MfPcIdentifier *)mIdentifiers->GetNext(pli);
    }
    mNumLocals -= removed;
    mLevel--;
    return mLevel;
}
