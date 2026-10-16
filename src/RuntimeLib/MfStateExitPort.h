// MfStateExitPort: an exit port on the border of a state: a transition
// inside ends at it and goes on outside along the port's transition. The
// slot is its place on the state's border, 1001-1004 the side. RTTI name;
// vtable 100b6384, 45 slots; unit 100868b0-10086f00.
#ifndef RUNTIMELIB_MFSTATEEXITPORT_H
#define RUNTIMELIB_MFSTATEEXITPORT_H
#include "MfStateProperty.h"
class MfScVirtualMachine;
class MfState;
class MfStateEntryAction;
class MfStateTransition;

class MfStateExitPort : public MfStateProperty {
public:
    enum { TYPE = 0x6d };

    MfStateExitPort(MfString name, int slot, MfStatePropertyList *properties);
    virtual ~MfStateExitPort();
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
    virtual int Slot44() { return 0; }                                          // 0xb0

    // runs the port's script and the state's exit action, then goes on
    // along the port's transition
    MfState *Exit(MfScVirtualMachine *vm);
    void SetSlot(int slot);

    MfStateTransition *mTransition;                                             // 0x28
    MfStateEntryAction *mAction;                                                // 0x2c
    int mSlot;                                                                  // 0x30
    int mSide;                                                                  // 0x34
};

#endif
