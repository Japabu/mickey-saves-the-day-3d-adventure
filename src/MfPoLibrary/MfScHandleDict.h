// MfScHandleDict (PiccoloRT.dll): a pool of named variables, name -> data
// handle; it holds a reference on each handle.
#pragma once
#include "MfDictionary.h"
class MfScDataHandle;
class MfScHandleDictIterator;
class MfString;

class __declspec(dllimport) MfScHandleDict : public MfDictionary {
public:
    enum HandleDictPassRef { kAddRef = 0, kPassRef = 1 };
    MfScHandleDict() {}
    virtual ~MfScHandleDict();
    MfScDataHandle *Find(const MfString &);
    int Add(const MfString &, MfScDataHandle *, HandleDictPassRef);
    void Replace(const MfString &, MfScDataHandle *, HandleDictPassRef);
    void Delete(const MfString &, MfScDataHandle **);
    void Clear();
    void EnumHandles(MfScHandleDictIterator &);
};
