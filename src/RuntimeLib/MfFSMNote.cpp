// MfFSMNote, MfFSMNoteContents (RuntimeLib.dll 10082700-10082c00).
#include <iostream.h>
#include "MfFSMNote.h"

// 0x10082700 ??0MfFSMNote@@QAE@VMfString@@PAVMfStatePropertyList@@@Z
MfFSMNote::MfFSMNote(MfString name, MfStatePropertyList *properties)
    : mContents(0)
{
    mName = name;
    mType = TYPE;
    mProperties = properties;
}

// 0x100827a0 ??1MfFSMNote@@UAE@XZ
MfFSMNote::~MfFSMNote()
{
}

// 0x100827b0 ?Clone@MfFSMNote@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfFSMNote::Clone()
{
    MfStatePropertyList *properties = new MfStatePropertyList(0);
    MfFSMNote *copy = new MfFSMNote(mName, properties);
    copy->SetProperties(mProperties);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10082880 ?FormatForExecution@MfFSMNote@@UAEXPAVMfFiniteStateMachine@@@Z
void MfFSMNote::FormatForExecution(MfFiniteStateMachine *machine)
{
    mMachine = machine;
    MfDblListItem *pli;
    MfStateProperty *p;
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        switch (p->mType) {
        case 0x6a:
            mDisplayPos = p;
            break;
        case 0x73:
            mContents = p;
            break;
        default:
            cout << "MfFSMNote::FormatForExecution(): Unknown note part:" << p->mType << endl;
            break;
        }
    }
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->FormatForExecution(mMachine);
}

// 0x10082920 ??0MfFSMNoteContents@@QAE@VMfString@@@Z
MfFSMNoteContents::MfFSMNoteContents(MfString text)
    : mText(text)
{
    mType = TYPE;
}

// 0x100829b0 ??1MfFSMNoteContents@@UAE@XZ
MfFSMNoteContents::~MfFSMNoteContents()
{
}

// 0x10082a00 ?Clone@MfFSMNoteContents@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfFSMNoteContents::Clone()
{
    MfFSMNoteContents *copy = new MfFSMNoteContents(mText);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10082a90 ?Write@MfFSMNoteContents@@UAEXAAVostream@@AAHH@Z
void MfFSMNoteContents::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (mode == 1 || mText.Length() < 3)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    if (mText.Length() < 2)
        return;
    os << tabs;
    os << "note_contents" << endl;
    os << tabs;
    os << "{!*!";
    os << mText;
    os << "!*!}";
    os << endl;
}

// 0x10082b90 ?SetPath@MfFSMNoteContents@@UAEXVMfString@@@Z
void MfFSMNoteContents::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << "NoteContents";
}
