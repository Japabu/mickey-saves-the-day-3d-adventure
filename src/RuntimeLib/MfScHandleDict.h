// MfScHandleDict (PiccoloRT.dll): a pool of named variables, name -> data
// handle; it holds a reference on each handle. The constructor is inline
// (callers store the imported vtable).
#ifndef RUNTIMELIB_MFSCHANDLEDICT_H
#define RUNTIMELIB_MFSCHANDLEDICT_H
#include "MfDictionary.h"
class MfScDataHandle;
class MfString;

class __declspec(dllimport) MfScHandleDict : public MfDictionary {
public:
    enum HandleDictPassRef { kAddRef = 0, kPassRef = 1 };
    MfScHandleDict() : MfDictionary(53) {}
    virtual ~MfScHandleDict();
    MfScDataHandle *Find(const MfString &);
    int Add(const MfString &, MfScDataHandle *, HandleDictPassRef);
    void Replace(const MfString &, MfScDataHandle *, HandleDictPassRef);
    void Delete(const MfString &, MfScDataHandle **);
    void Clear();
};

#endif
