// MfStateChoicePoint: a choice point of a behaviour; its script picks the
// transition to leave by (by name, through the machine's mChoice), else
// the one named "default" is taken. RTTI name; vtable 100b5fdc, 44 slots;
// unit 10084630-10084df0.
#ifndef RUNTIMELIB_MFSTATECHOICEPOINT_H
#define RUNTIMELIB_MFSTATECHOICEPOINT_H
#include "MfStateProperty.h"
class MfScVirtualMachine;
class MfStateEntryAction;
class MfStateTransition;

class MfStateChoicePoint : public MfStateProperty {
public:
    enum { TYPE = 0x6f };

    MfStateChoicePoint(const MfString &name, MfStatePropertyList *properties);
    virtual ~MfStateChoicePoint();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual int CanDelete() { return 1; }
    // may a transition lead from the choice point to target?
    virtual int CanConnectTo(MfStateProperty *target);
    virtual void SetPath(MfString parent);
    virtual MfString *GetScript();
    virtual void SetScript(const MfString &source);
    virtual void FormatForExecution(MfFiniteStateMachine *machine);
    // the variables of the state it is in
    virtual MfScVariableStack *GetVariableStack();
    virtual void Slot41(int) {}                                                 // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8
    virtual void Slot43(int) {}                                                 // 0xac

    // runs the script and returns the transition it picked
    MfStateTransition *Choose(MfScVirtualMachine *vm);

    MfStatePropertyList mTransitions;                                           // 0x28
    MfStateTransition *mChosen;                                                 // 0x3c
    MfStateTransition *mDefault;                                                // 0x40 the "default" transition
    MfStateEntryAction *mAction;                                                // 0x44 the script
};

#endif
