#pragma once
#include "MfAiSourceResolver.h"
#include "MfCallBack.h"
class MfDictionary;

// The source types by module type (MfScPiccolo::GetSourceResolver). Holds a
// reference on every registered type; the dictionary hands them back to
// CallBack when it is cleared.
class MfScSourceResolver : public MfAiSourceResolver, public MfCallBack {
public:
    MfScSourceResolver();
    virtual ~MfScSourceResolver();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int Register(const MfString &type, MfAiSourceType *sourceType);
    virtual int Unregister(const MfString &type);
    virtual int Find(const MfString &type, MfAiSourceType **sourceType);
    virtual int CallBack(void *data);

private:
    int mRefCount;              // +0x08
    MfDictionary *mTypes;       // +0x0c
};
