// MfPoKeyboard: the Piccolo object of the keyboard ("Keyboard"): routes key events to
// actors, reports the key state.
#ifndef MFPOLIBRARY_MFPOKEYBOARD_H
#define MFPOLIBRARY_MFPOKEYBOARD_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
#include "MfPoBase.h"

class MFPO_API MfPoKeyboard : public MfPoBase {
public:
    MfPoKeyboard();
    virtual ~MfPoKeyboard();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
private:
    MfPoKeyboard(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMSetKeyRoute(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetKeyChangeRoute(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetKeyPressPollRate(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMEnableAuthorToolKeys(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDisableAuthorToolKeys(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoKeyboard *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
};

#endif
