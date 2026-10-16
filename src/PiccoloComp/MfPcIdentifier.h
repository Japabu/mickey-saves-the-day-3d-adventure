#pragma once
#include "MfPcParserItem.h"
#include "MfString.h"
class MfVariant;

// Kinds of identifiers (MfPcIdentifier::mKind).
enum {
    PC_ID_UNDEFINED = 0,
    PC_ID_LOCAL = 1,            // local variable
    PC_ID_PARAM = 2,            // function parameter
    PC_ID_LABEL = 3,
    PC_ID_GCONST = 4,           // global constant
    PC_ID_LCONST = 5,           // local constant
    PC_ID_PERM = 6,             // permanent global
    PC_ID_GTEMP = 7,            // temporary global
    PC_ID_GVAR = 8              // global variable
};

// An entry of the symbol table (0x2c bytes).
class MfPcIdentifier : public MfPcParserItem {
public:
    MfPcIdentifier(MfString const &name);
    virtual ~MfPcIdentifier();
    void SetValue(MfVariant const &value);
    int FrameOffset();
    MfString Describe();
    int IsConstant() const
    {
        return mKind == PC_ID_GCONST || mKind == PC_ID_LCONST || mKind == PC_ID_PERM || mKind == PC_ID_GTEMP;
    }
    int IsPermanent() const { return mKind == PC_ID_PERM || mKind == PC_ID_GTEMP; }

    MfString mName;             // +0x0c
    int mKind;                  // +0x10 PC_ID_*
    int mIsArray;               // +0x14
    int mArraySize;             // +0x18 elements of an array
    int mLocalIndex;            // +0x1c
    int mLevel;                 // +0x20 code level of a local
    int mParamIndex;            // +0x24
    MfVariant *mValue;          // +0x28 constants only
};
