// MfScPropTokenTable (PiccoloRT.dll): the dictionary property name ->
// MfScPropToken of a script class; it is its own delete callback (0x18
// bytes). The constructor and the Add members are inline; VC5 calls the
// imported copies once a function has used up its inline budget.
#pragma once
#include "MfDictionary.h"
#include "MfCallBack.h"
#include "MfScPropToken.h"

class __declspec(dllimport) MfScPropTokenTable : public MfDictionary, public MfCallBack {
public:
    MfScPropTokenTable() {}
    virtual ~MfScPropTokenTable();
    MfScPropToken *Find(const MfString &property) { return (MfScPropToken *)MfDictionary::Find(property); }
    void Add(MfScPropToken *token) { MfDictionary::Add(token->Property(), token); }
    void Add(const MfString &property, int token) { Add(new MfScPropToken(property, token)); }
    void Clear();
    void Delete(const MfString &);
    virtual int CallBack(void *);
};
