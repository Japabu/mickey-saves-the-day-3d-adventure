// MfPoWorld: the Piccolo class object "World" (RuntimeLib.dll 10078e30-
// 1007bcd0, class name from the RTTI): loads, saves, deletes and teleports
// actors and spaces through the application's world (g_paiWorld); the
// world's own properties are visible as instance properties.
#ifndef RUNTIMELIB_MFPOWORLD_H
#define RUNTIMELIB_MFPOWORLD_H
#include "MfPoBase.h"
class MfAiWorld;
class MfScPropertyExtList;

// the application's world (MfPoLibrary)
extern __declspec(dllimport) MfAiWorld *g_paiWorld;

class MfPoWorld : public MfPoBase {
public:
    virtual ~MfPoWorld();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void Initialize();
    static void Shutdown();
    static MfString InitConstants();
private:
    MfPoWorld(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pexlExtensions; }
    MfScDataHandle *PMUpdateViews(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMLoadActor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDeleteActor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSaveActor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMTeleportActor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMLoadSpace(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDeleteSpace(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSaveSpace(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMStop(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMReset(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMEnableToolMouseBehavior(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDisableToolMouseBehavior(MfScDataHandle *, int, MfScDataHandle **);

    // set while the exemplar constructor registers the methods
    static int bConstructing;                               // 100f5a64
    static MfPoWorld *poExemplar;                           // 100f5a68
    static MfScPropertyExtList *pexlExtensions;             // 100f5a6c
    static MfScPropTokenTable *pttPropTokens;               // 100f5a70
};

#endif
