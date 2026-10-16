// MfPoHFSM: the Piccolo object of a behaviour state machine ("HFSM";
// RuntimeLib.dll 1006c270-1006cd30, class name from the RTTI). The script of
// a choice point names the transition to take through it.
#ifndef RUNTIMELIB_MFPOHFSM_H
#define RUNTIMELIB_MFPOHFSM_H
#include "MfPoBase.h"
class MfFiniteStateMachine;

class MfPoHFSM : public MfPoBase {
public:
    MfPoHFSM(MfFiniteStateMachine *machine);
    virtual ~MfPoHFSM();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void Initialize();
    static void Shutdown();
private:
    MfPoHFSM(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMSetChoiceTransition(MfScDataHandle *, int, MfScDataHandle **);

    MfFiniteStateMachine *mMachine;                         // 0x14

    static MfPoHFSM *poExemplar;                            // 100f59e8
    static MfScPropTokenTable *pttPropTokens;               // 100f59ec
};

#endif
