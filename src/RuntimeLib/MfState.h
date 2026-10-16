// MfState: a state of a behaviour, with its substates, transitions, ports,
// choice points and script parts (RTTI name; vtable 100b5ddc, 43 slots; unit
// 10082c00-100846b0, with the start_state and begin_hist markers
// MfStartProperty and MfStateBeginHistory). RuntimeLib internal: the method
// names are descriptive, slots whose role is not known yet are named after
// their slot.
#ifndef RUNTIMELIB_MFSTATE_H
#define RUNTIMELIB_MFSTATE_H
#include "MfStateProperty.h"
#include "MfTransitionProperty.h"
#include "MfStateTransition.h"
class MfScDataHandle;
class MfScHandleDict;
class MfScVirtualMachine;
class MfStateEntryAction;
class MfStateExitAction;
class MfStateFunctionDef;
class MfStateInitVar;
class MfStateVariables;
class MfStateTransition;

class MfState : public MfStateProperty {
public:
    enum { TYPE = 0x65 };

    MfState(MfString name, MfStatePropertyList *properties);
    virtual ~MfState();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual int CountNamed(MfString name, MfFSMMutable *except);
    virtual int CanDelete();
    virtual void SetPath(MfString parent);
    virtual void Slot17(int);
    virtual void FormatForExecution(MfFiniteStateMachine *machine);
    virtual MfScVariableStack *GetVariableStack();
    virtual int AddPart(MfStateProperty *part);
    virtual int RemovePart(MfStateProperty *part, int keep);
    virtual void Slot41(int) {}                                                 // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8

    // the compiled entry and exit actions, 0 if none
    MfScDataHandle *GetEntryFunction();
    MfScDataHandle *GetExitFunction();
    // makes child the active substate
    void SetActiveChild(MfState *child);
    // child carries the begin_hist marker
    void SetHistoryStart(MfState *child);
    // makes this state the active one of each of its ancestors
    void Activate();
    // runs the entry action
    MfState *RunEntryAction(MfScVirtualMachine *vm);
    // enters the state: the entry action, then the substate to go on in;
    // returns the innermost state entered below this one
    MfState *Enter(MfScVirtualMachine *vm);
    // enters the state and follows the initial transition
    MfState *EnterInitial(MfScVirtualMachine *vm);
    // runs the init_var part of the state and its substates
    void RunInitVar(MfScVirtualMachine *vm);
    // the state and its ancestors up to stop (or the root), innermost first
    int GetAncestors(MfStateProperty **path, MfState *stop, int &count, int max);
    // adds or removes the start_state marker
    void SetStartState(int start);
    // the transition the state starts in (only one is allowed)
    void SetInitialTransition(MfStateTransition *transition);
    // sorts the state properties into the lists again
    void SortSubstates();
    // makes a script variable stack on outer for the state and its substates
    void InitVariables(MfScVariableStack *outer);
    // detaches part from the lists without touching mProperties
    int UnlinkPart(MfStateProperty *part, int keep);
    // removes the substates, transitions, ports and choice points
    void RemoveStructure();
    MfState *FirstSubstate();
    MfState *ResetActiveChild();

    // the substates, transitions, ports and choice points by name
    MfState *FindState(MfString name);                      // 100839e0
    MfStateProperty *FindExitPort(MfString name);           // 10083aa0
    MfStateProperty *FindEntryPort(MfString name);          // 10083b60
    MfStateProperty *FindChoicePoint(MfString name);        // 10083c20

    MfState *mActiveChild;                                                      // 0x28
    MfState *mHistoryStart;                                                     // 0x2c
    MfStateTransition *mInitialTransition;                                      // 0x30
    MfStatePropertyList mStates;                                                // 0x34
    MfStatePropertyList mTransitions;                                           // 0x48
    MfStatePropertyList mExitPorts;                                             // 0x5c
    MfStatePropertyList mEntryPorts;                                            // 0x70
    MfStatePropertyList mChoicePoints;                                          // 0x84
    MfStateProperty *mStartState;                                               // 0x98 start_state
    MfStateProperty *mBeginHistory;                                             // 0x9c begin_hist
    MfStateEntryAction *mEntryAction;                                           // 0xa0
    MfStateExitAction *mExitAction;                                             // 0xa4
    MfStateInitVar *mInitVar;                                                   // 0xa8
    MfStateFunctionDef *mFunctionDef;                                           // 0xac
    MfStateVariables *mStateVariables;                                          // 0xb0
    int mFormatted;                                                             // 0xb4
    int mb8, mbc;                                                               // 0xb8
    MfScVariableStack *mVarStack;                                               // 0xc0
    MfScHandleDict *mVariables;                                                 // 0xc4
};

// the start_state marker of a state (vtable 100b5e8c)
class MfStartProperty : public MfStateProperty {
public:
    enum { TYPE = 0x6c };

    MfStartProperty() { mType = TYPE; }
    virtual MfFSMMutable *Clone();
};

// the begin_hist marker of a state (vtable 100b5f34)
class MfStateBeginHistory : public MfStateProperty {
public:
    enum { TYPE = 0x71 };

    MfStateBeginHistory() { mType = TYPE; }
    virtual MfFSMMutable *Clone();
};

#endif
