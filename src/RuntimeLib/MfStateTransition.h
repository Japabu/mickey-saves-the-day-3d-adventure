// MfStateTransition: a transition of a behaviour. Its parts (destination,
// trigger, action, type ...) are MfTransitionProperty objects in
// mTransProperties, sorted into the members by FormatForExecution. RTTI
// name; vtable 100b6764, 56 slots; unit 10087f50-10089110.
#ifndef RUNTIMELIB_MFSTATETRANSITION_H
#define RUNTIMELIB_MFSTATETRANSITION_H
#include "MfStateProperty.h"
#include "MfTransitionProperty.h"
class MfScVirtualMachine;
class MfState;
class MfTransTrigAction;

class MfStateTransition : public MfStateProperty {
public:
    enum { TYPE = 0x69 };

    MfStateTransition(MfString name, MfTransPropList *properties);
    virtual ~MfStateTransition();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual void SetPath(MfString parent);
    virtual MfFSMMutable *Find(MfString path, MfStringList *names, MfDblListItem *pos, int depth);
    virtual void CollectAll(MfDblList *list);
    virtual void FormatForExecution(MfFiniteStateMachine *machine);
    virtual void SetParent(MfStateProperty *parent);
    virtual MfStateProperty *GetOwner() { return mSource; }
    virtual MfScVariableStack *GetVariableStack();
    virtual void Slot41(int, int) {}                                            // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8
    virtual int Slot43(int, int) { return 0; }                                  // 0xac
    virtual void Slot44(MfString) {}                                            // 0xb0
    virtual int Slot45(int) { return 0; }                                       // 0xb4
    virtual void Slot46(int) {}                                                 // 0xb8
    virtual void Slot47(int) {}                                                 // 0xbc
    virtual void Slot48(int) {}                                                 // 0xc0
    virtual void Slot49(int, int, int) {}                                       // 0xc4
    virtual void Slot50() {}                                                    // 0xc8
    virtual void Slot51(int, int, int) {}                                       // 0xcc
    virtual void Slot52(int, int) {}                                            // 0xd0
    virtual void Slot53(int, int) {}                                            // 0xd4
    virtual int Slot54() { return 0; }                                          // 0xd8
    virtual void Slot55(int) {}                                                 // 0xdc

    // copies of the parts of list become the transition's parts
    void SetTransProperties(MfTransPropList *list);
    // the transition and the state it starts from
    void SetSourceState(MfStateProperty *state);
    // the compiled action, 0 if none
    MfScDataHandle *GetActionFunction();
    // the state, port or choice point the transition leads to
    MfStateProperty *GetTarget();
    // jump, self_internal or self_external
    int GetKind();
    void SetKind(int kind);
    // fires the transition; returns the state it ends in
    MfState *Fire(MfScVirtualMachine *vm);
    // goes on along the transition from an entry port
    MfState *Take(MfScVirtualMachine *vm);                  // 10088450
    // evaluates the script of part (a condition)
    int Evaluate(MfTransTriggeredBy *trigger);

    MfVisualConnector *mConnector;                                              // 0x28
    MfTransPropList *mTransProperties;                                          // 0x2c
    MfTransDestState *mDest;                                                    // 0x30
    MfTransExitBy *mExitBy;                                                     // 0x34
    MfTransEnterBy *mEnterBy;                                                   // 0x38
    MfTransChoicePt *mChoicePt;                                                 // 0x3c
    MfTransPropList mTriggers;                                                  // 0x40
    MfTransitionType *mTransType;                                               // 0x54
    MfTransInit *mInitial;                                                      // 0x58 an initial transition (to a substate)
    MfTransTrigAction *mAction;                                                 // 0x5c
    MfStateProperty *mSource;                                                   // 0x60
    int m64, m68;                                                               // 0x64
    int mListening;                                                             // 0x6c
    int m70;                                                                    // 0x70
};

#endif
