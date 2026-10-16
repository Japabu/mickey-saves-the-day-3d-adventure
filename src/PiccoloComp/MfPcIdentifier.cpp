// PiccoloComp.dll MfPcIdentifier.cpp (0x1000ce50-0x1000d270)
#include "MfPcIdentifier.h"
#include "MfVariant.h"

// 0x1000ce50 ??0MfPcIdentifier@@QAE@ABVMfString@@@Z
MfPcIdentifier::MfPcIdentifier(MfString const &name)
    : mName(name), mKind(PC_ID_UNDEFINED), mIsArray(0), mArraySize(0), mLocalIndex(0), mLevel(0),
      mParamIndex(0), mValue(0)
{
}

// 0x1000cec0 ??_GMfPcIdentifier@@UAEPAXI@Z (compiler-generated)

// 0x1000cee0 ??1MfPcIdentifier@@UAE@XZ
MfPcIdentifier::~MfPcIdentifier()
{
    delete mValue;
}

// 0x1000cf50 ?SetValue@MfPcIdentifier@@QAEXABVMfVariant@@@Z
void MfPcIdentifier::SetValue(MfVariant const &value)
{
    if (!mValue)
        mValue = new MfVariant(value);
    else
        *mValue = value;
}

// 0x1000cff0 ?FrameOffset@MfPcIdentifier@@QAEHXZ
int MfPcIdentifier::FrameOffset()
{
    if (mKind == PC_ID_LOCAL)
        return -1 - mLocalIndex;
    if (mKind == PC_ID_PARAM)
        return mParamIndex + 7;
    return -1;
}

// 0x1000d020 ?Describe@MfPcIdentifier@@QAE?AVMfString@@XZ
MfString MfPcIdentifier::Describe()
{
    MfString text;
    MfString kind;
    text << mName << ": ";
    switch (mKind) {
    case PC_ID_UNDEFINED:
        text << "UNDEFINED";
        break;
    case PC_ID_LOCAL:
        text << "LOCAL #" << mLocalIndex << "@" << mLevel;
        break;
    case PC_ID_PARAM:
        text << "PARAM #" << mParamIndex;
        break;
    case PC_ID_LABEL:
        text << "LABEL";
        break;
    case PC_ID_GCONST:
        kind = "GCONST";
        goto value;
    case PC_ID_LCONST:
        kind = "LCONST";
        goto value;
    case PC_ID_PERM:
        kind = "PERM";
        goto value;
    case PC_ID_GTEMP:
        kind = "GTEMP";
        goto value;
    case PC_ID_GVAR:
        kind = "GVAR";
    value:
        if (mValue) {
            text << kind << '(';
            switch (mValue->Type()) {
            case 1: text << "int"; break;
            case 2: text << "unsigned int"; break;
            case 3: text << "float"; break;
            case 4: text << "double"; break;
            case 5: text << "string"; break;
            default: text << "unknown"; break;
            }
            text << ')' << " = " << MfString(*mValue);
        } else
            text << kind << " (uninitialized)";
        break;
    }
    return text;
}
