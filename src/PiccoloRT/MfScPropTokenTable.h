// MfScPropTokenTable: a dictionary property name -> MfScPropToken.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfDictionary.h"
#include "MfCallBack.h"
#include "MfScPropToken.h"
class MfString;

// Dictionary property name -> MfScPropToken; it is its own delete callback (0x18 bytes).
class PICCOLORT_API MfScPropTokenTable : public MfDictionary, public MfCallBack {
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
