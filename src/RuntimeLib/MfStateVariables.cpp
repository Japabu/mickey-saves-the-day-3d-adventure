// MfStateVariables (RuntimeLib.dll 100890b0-10089620): the variables a state
// declares ('vars { a, b }').
#include <iostream.h>
#include "MfStateAction.h"
#include "MfScHandleDict.h"
#include "MfStringList.h"

class MfNullString;
extern __declspec(dllimport) MfNullString NULL_STRING;

// the value a declared variable starts with
MfScDataHandle *gNoValue = (MfScDataHandle *)-1;

// 0x100890b0 ??0MfStateVariables@@QAE@PAVMfStringList@@@Z
MfStateVariables::MfStateVariables(MfStringList *names)
    : mNames(names)
{
    mType = TYPE;
}

// 0x10089130 ??1MfStateVariables@@UAE@XZ
MfStateVariables::~MfStateVariables()
{
    if (mNames)
        delete mNames;
    mNames = 0;
}

// 0x100891a0 ?Clone@MfStateVariables@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateVariables::Clone()
{
    MfStringList *names = new MfStringList;
    MfDblListItem *pli;
    for (MfString *name = mNames->First(pli); name; name = mNames->Next(pli)) {
        new MfString(*name);
        names->Add(*name);
    }
    MfStateVariables *copy = new MfStateVariables(names);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x100892c0 ?Write@MfStateVariables@@UAEXAAVostream@@AAHH@Z
void MfStateVariables::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "vars {";
    MfDblListItem *pli;
    MfString *first = (MfString *)mNames->GetFirst(pli);
    MfString *last = (MfString *)mNames->GetLast(pli);
    for (MfString *name = (MfString *)mNames->GetFirst(pli); name; name = (MfString *)mNames->GetNext(pli)) {
        if (name != first)
            os << tabs << '\t';
        os << *name;
        if (name != last)
            os << "," << endl;
    }
    os << "}" << endl;
}

// 0x10089400 ?DefineVariables@MfStateVariables@@QAEXPAVMfScHandleDict@@@Z
void MfStateVariables::DefineVariables(MfScHandleDict *variables)
{
    MfDblListItem *pli;
    for (MfString *name = mNames->First(pli); name; name = mNames->Next(pli))
        variables->Replace(*name, gNoValue, MfScHandleDict::kAddRef);
}

// 0x10089450 ?Search@MfStateVariables@@UAEXVMfString@@HHHHHHHAAPAVMfFSMMutable@@1@Z
void MfStateVariables::Search(MfString text, int, int caseSensitive, int, int, int, int, int inVariables,
                              MfFSMMutable *&found, MfFSMMutable *&foundIn)
{
    MfString s;
    mMatch = NULL_STRING;
    if (inVariables) {
        MfDblListItem *pli;
        for (MfString *name = mNames->First(pli); name; name = mNames->Next(pli)) {
            s = *name;
            if (!caseSensitive)
                s.ConvertToLower();
            if (s.ContainsSubString(text, 0) >= 0) {
                found = this;
                foundIn = this;
                mMatch = s;
                return;
            }
        }
        found = 0;
        foundIn = 0;
    }
}
