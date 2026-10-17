// MfPoMouse: the Piccolo object of the mouse ("Mouse"): routes mouse events to
// actors, reports the last picked actor and floor.
#ifndef MFPOLIBRARY_MFPOMOUSE_H
#define MFPOLIBRARY_MFPOMOUSE_H
#include "MfPoLibrary_api.h"
#include "MfPoBase.h"

class MFPO_API MfPoMouse : public MfPoBase {
public:
    MfPoMouse();
    virtual ~MfPoMouse();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
private:
    MfPoMouse(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMSetMouseRoute(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMClearMouseRoute(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMEnableToolMouseBehavior(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDisableToolMouseBehavior(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoMouse *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
};

#endif
