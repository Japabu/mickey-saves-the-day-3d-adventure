#pragma once
#include "MfDictionary.h"
class MfScDataHandle;
class MfScHandleDictIterator;
class MfString;

// Variable pool: name -> data handle; it holds a reference on each handle.
class __declspec(dllexport) MfScHandleDict : public MfDictionary {
public:
    enum HandleDictPassRef { kAddRef = 0, kPassRef = 1 };
    MfScHandleDict() : MfDictionary(0x35) {}
    virtual ~MfScHandleDict() { Clear(); }
    MfScDataHandle * Find(MfString const &);
    int Add(MfString const &, MfScDataHandle *, HandleDictPassRef);
    void Replace(MfString const &, MfScDataHandle *, HandleDictPassRef);
    void Delete(MfString const &, MfScDataHandle * *);
    void Clear();
    void EnumHandles(MfScHandleDictIterator &);
};
