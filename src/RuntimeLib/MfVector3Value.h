// MfVector3Value (MfActorLib.dll): a typed value holding a vector.
#ifndef RUNTIMELIB_MFVECTOR3VALUE_H
#define RUNTIMELIB_MFVECTOR3VALUE_H
#include "MfTypedValue.h"
#include "MfVector3.h"

class __declspec(dllimport) MfVector3Value : public MfTypedValue {
public:
    MfVector3Value();
    MfVector3Value(const MfVector3 &);
    MfVector3Value(const MfVector3Value &);
    virtual ~MfVector3Value();
    operator MfVector3 &() { return mValue; }
    operator const MfVector3 &() const { return mValue; }

    MfVector3 mValue;                                       // 0x0c
};

#endif
