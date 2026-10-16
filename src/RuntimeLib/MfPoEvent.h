// MfPoEvent (MfPoLibrary.dll): the Piccolo object of an event (a type name and
// the message handler of the receiver). Virtuals from MfPoLibrary's exports.
#ifndef RUNTIMELIB_MFPOEVENT_H
#define RUNTIMELIB_MFPOEVENT_H
#include "MfScData.h"
class MfPiccoloMessageHandler;

class __declspec(dllimport) MfPoEvent : public MfScObject {
public:
    MfPoEvent(const MfString &type, MfPiccoloMessageHandler *handler = 0);
    virtual ~MfPoEvent();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual void GetInstanceProperties(MfStringList *, int);
    const MfString &Type() { return mType; }
private:
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
public:
    MfString mType;                                         // 0x14
    MfPiccoloMessageHandler *mHandler;                      // 0x18
};

#endif
