// MfStateEntryPort: an entry port on the border of a state: a transition
// from outside ends at it and goes on inside along the port's transition.
// The slot is its place on the state's border, 1001-1004 the side.
// RTTI name; vtable 100b6204, 45 slots; unit 10085ba0-10086280.
#ifndef RUNTIMELIB_MFSTATEENTRYPORT_H
#define RUNTIMELIB_MFSTATEENTRYPORT_H
#include "MfStateProperty.h"
class MfScVirtualMachine;
class MfState;
class MfStateEntryAction;
class MfStateTransition;

class MfStateEntryPort : public MfStateProperty {
public:
    enum { TYPE = 0x6e };

    MfStateEntryPort(const MfString &name, int slot, MfStatePropertyList *properties);
    virtual ~MfStateEntryPort();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual void SetPath(MfString parent);
    virtual void FormatForExecution(MfFiniteStateMachine *machine);
    virtual MfScVariableStack *GetVariableStack();
    virtual void Slot41(int) {}                                                 // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8
    virtual void Slot43(int) {}                                                 // 0xac
    virtual int Slot44() { return 1; }                                          // 0xb0

    // runs the port's script, then goes on along its transition (or into
    // the state)
    MfState *Enter(MfScVirtualMachine *vm);
    // runs the state's entry action and follows its initial transition
    MfState *EnterState(MfScVirtualMachine *vm);
    void SetSlot(int slot);

    MfStateTransition *mTransition;                                             // 0x28
    MfStateEntryAction *mAction;                                                // 0x2c
    int mSlot;                                                                  // 0x30
    int mSide;                                                                  // 0x34
};

#endif
