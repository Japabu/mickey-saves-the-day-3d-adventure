#pragma once
#include "MfDblList.h"
class MfString;

// mfutil.dll: a stack of strings (0x10 bytes); the constructor is inline here.
class __declspec(dllimport) MfStrStack : public MfDblList {   // own copy: src/mfutil/MfStrStack.h includes src/mfutil/MfDblList.h, which conflicts with the MfDblList kept here
public:
    MfStrStack() {}
    virtual ~MfStrStack();
    void Push(MfString const &s);
    MfString Pop();
    virtual void Clear();
};
