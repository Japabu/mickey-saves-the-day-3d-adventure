// MfTransInit (RuntimeLib.dll 1008a430-1008a590).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfState.h"

// 0x1008a430 ?Write@MfTransInit@@UAEXAAVostream@@AAHH@Z
void MfTransInit::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "init" << endl;
}

// 0x1008a4d0 ?Clone@MfTransInit@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransInit::Clone()
{
    MfTransInit *copy = new MfTransInit;
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// Inline members this object emits:
// 0x1008a580 ??1MfTransInit@@UAE@XZ
