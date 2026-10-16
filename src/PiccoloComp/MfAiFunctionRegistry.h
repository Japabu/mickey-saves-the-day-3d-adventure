#pragma once
#include "MfAi.h"
class MfScDataHandle;
class MfScHandleList;
class MfString;

// The registry of loaded script functions (PiccoloRT.dll). VC5 lays
// overloaded virtuals out in reverse order of declaration.
class MfAiFunctionRegistry : public MfAi {
public:
    virtual int Add(MfScDataHandle *function) = 0;
    virtual int Remove(MfScDataHandle *function) = 0;
    virtual int Find(MfString const &module, MfString const &name, MfScDataHandle **function) = 0;
    virtual int Find(MfString const &module, int line, MfScDataHandle **function) = 0;
    virtual int Find(MfString const &module, MfScHandleList *functions) = 0;
};
