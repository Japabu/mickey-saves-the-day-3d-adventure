#pragma once
#include "MfDictionary.h"
#include "MfCallBack.h"
#include "MfScPropToken.h"
class MfString;

// Dictionary property name -> MfScPropToken; it is its own delete callback (0x18 bytes).
class __declspec(dllexport) MfScPropTokenTable : public MfDictionary, public MfCallBack {
public:
    MfScPropTokenTable() : MfDictionary(0x35) {}
    virtual ~MfScPropTokenTable();
    MfScPropToken * Find(MfString const &name) { return (MfScPropToken *)MfDictionary::Find(name); }
    void Add(MfScPropToken *);
    void Add(MfString const &property, int token)
    {
        MfScPropToken *t = new MfScPropToken(property, token);
        MfDictionary::Add(t->Property(), t);
    }
    void Clear();
    void Delete(MfString const &);
    virtual int CallBack(void *);
};
