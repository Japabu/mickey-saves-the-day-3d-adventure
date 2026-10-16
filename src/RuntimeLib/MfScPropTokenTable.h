// MfScPropToken, MfScPropTokenTable (PiccoloRT.dll): the dictionary property
// name -> token of a script class; the table is its own delete callback (0x18
// bytes). The constructors and the Add members are inline. (As in
// src/MfPoLibrary.)
#ifndef RUNTIMELIB_MFSCPROPTOKENTABLE_H
#define RUNTIMELIB_MFSCPROPTOKENTABLE_H
#include "MfString.h"
#include "MfDictionary.h"
#include "MfDblList.h"

class __declspec(dllimport) MfScPropToken {
public:
    MfScPropToken(const MfString &property, int token) : m_token(token), m_property(property) {}
    ~MfScPropToken() {}
    const MfString &Property() const { return m_property; }
    int Token() const { return m_token; }
private:
    int m_token;                                            // 0x00
    MfString m_property;                                    // 0x04
};

class __declspec(dllimport) MfScPropTokenTable : public MfDictionary, public MfCallBack {
public:
    MfScPropTokenTable() : MfDictionary(0x35) {}
    virtual ~MfScPropTokenTable();
    MfScPropToken *Find(const MfString &property) { return (MfScPropToken *)MfDictionary::Find(property); }
    void Add(MfScPropToken *token) { MfDictionary::Add(token->Property(), token); }
    void Add(const MfString &property, int token) { Add(new MfScPropToken(property, token)); }
    void Clear();
    void Delete(const MfString &);
    virtual int CallBack(void *);
};

#endif
