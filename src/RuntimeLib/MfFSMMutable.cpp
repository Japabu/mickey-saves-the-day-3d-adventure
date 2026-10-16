// MfFSMMutable (RuntimeLib.dll 10082260-10082700).
#include "MfFSMMutable.h"
#include "MfFiniteStateMachine.h"
#include "MfFSMList.h"
#include "MfStringList.h"

// 0x10082260 ??0MfFSMMutable@@QAE@XZ
MfFSMMutable::MfFSMMutable()
    : mMachine(0)
{
}

// 0x100822e0 ??1MfFSMMutable@@UAE@XZ
MfFSMMutable::~MfFSMMutable()
{
    if (mMachine)
        mMachine->PartDeleted(this);
}

// 0x10082350 ?SetName@MfFSMMutable@@UAEXPBD@Z
void MfFSMMutable::SetName(const char *name)
{
    mName = name;
    if (mMachine)
        mMachine->UpdatePaths();
}

// 0x10082380 ?SetPath@MfFSMMutable@@UAEXVMfString@@@Z
void MfFSMMutable::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << GetName();
}

// 0x100823f0 ?SetFullPath@MfFSMMutable@@QAEXVMfString@@@Z
void MfFSMMutable::SetFullPath(MfString path)
{
    mPath = path;
}

// 0x10082440 ?Find@MfFSMMutable@@UAEPAV1@VMfString@@PAVMfStringList@@PAVMfDblListItem@@H@Z
MfFSMMutable *MfFSMMutable::Find(MfString path, MfStringList *names, MfDblListItem *pos, int depth)
{
    MfString *name = pos ? (pos->GetNext() ? (MfString *)pos->GetNext()->GetData() : 0) : 0;
    if (name && GetName() == *name && path == mPath)
        return this;
    return 0;
}

// 0x100824f0 ?SplitPath@MfFSMMutable@@SAXPAVMfStringList@@VMfString@@@Z
void MfFSMMutable::SplitPath(MfStringList *names, MfString path)
{
    path.Tokenize("\\", *names);
    names->DeleteFirst();
}

// 0x10082580 ?Search@MfFSMMutable@@UAEXVMfString@@HHHHHHHAAPAV1@1@Z
void MfFSMMutable::Search(MfString text, int, int caseSensitive, int, int what, int, int, int,
                          MfFSMMutable *&found, MfFSMMutable *&foundIn)
{
    MfString name;
    if (what == 1) {
        name = mName;
        if (!caseSensitive)
            name.ConvertToLower();
        if (name.ContainsSubString(text, 0) >= 0) {
            found = this;
            foundIn = this;
            return;
        }
    }
    found = 0;
    foundIn = 0;
}
