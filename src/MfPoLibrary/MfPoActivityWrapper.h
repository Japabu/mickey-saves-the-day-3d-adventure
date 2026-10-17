// MfPoActivityWrapper: the script class "ActivityWrapper": it takes over the
// actors' SetActivity/AddActivity to run activity specifications written in
// Piccolo.
#ifndef MFPOLIBRARY_MFPOACTIVITYWRAPPER_H
#define MFPOLIBRARY_MFPOACTIVITYWRAPPER_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
#include "MfScObject.h"
#include "MfScDataHandle.h"
#include "MfPoActorNode.h"
class MfAiActivitySpec;

class MFPO_API MfPoActivityWrapper : public MfScObject {
public:
    MfPoActivityWrapper();
    MfPoActivityWrapper(MfScData::Exemplar);
    virtual ~MfPoActivityWrapper();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    int IsSet() const { return bSet; }
    void SetOptions(int options) { iOptions = options; }
    void SetTheGetActivityFlag(int flag) { bGetActivity = flag; }
    int GetTheGetActivityFlag() const { return bGetActivity; }
protected:
    MfPoActivityWrapper(ExemplarChild);
    virtual MfString GetClassName();
    void ExceptionMsg(MfString msg);
    void SetSpecObject(MfAiActivitySpec *spec, int shared);
    MfScDataHandle *EMSetActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *EMAddActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *EMInstantiatePluginActivity(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoActivityWrapper *poExemplar;
private:
    MfAiActivitySpec *paisSpec;                             // 0x14
    int iOptions;                                           // 0x18
    int bGetActivity;                                       // 0x1c
    int bSet;                                               // 0x20
    MfScMethod pmSetActivity;                           // 0x24 Actor.SetActivity before the wrapper
    MfScMethod pmAddActivity;                           // 0x28 Actor.AddActivity before the wrapper
};

#endif
