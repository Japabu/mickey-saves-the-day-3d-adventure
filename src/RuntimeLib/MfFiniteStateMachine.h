// MfFiniteStateMachine: a loaded behaviour, a hierarchical finite state
// machine (RTTI name; vtable 100b59d4, 35 slots; unit 1007dda0-1007f9d0).
// RuntimeLib internal: the method names are descriptive, the slots whose
// role is not known yet are named after their slot.
#ifndef RUNTIMELIB_MFFINITESTATEMACHINE_H
#define RUNTIMELIB_MFFINITESTATEMACHINE_H
#include "MfDblList.h"
#include "MfStateProperty.h"
#include "MfString.h"
class MfFSMManager;
class MfMsg;
class MfFSMMutable;
class MfScVariableStack;
class MfStringList;
class MfScHandleDict;
class MfState;
class MfStateTransition;
class MfPcErrorCallBack;
class MfScDataHandle;

class MfFiniteStateMachine {
public:
    enum { MAX_LISTENERS = 20 };

    // the root state is a copy of root, or a new state 'Root' with properties
    MfFiniteStateMachine(const MfString &file, MfStatePropertyList *properties, MfState *root);
    virtual ~MfFiniteStateMachine();                                            // 0x00
    // loads the compiled scripts from the behaviour's object file if it is newer
    // than the behaviour file, else compiles them; returns the number of failures
    virtual int Compile(MfPcErrorCallBack *errors, int *error);                 // 0x04
    // prepares the machine to run: sorts the parts, finds the start state, makes the variables
    virtual void Initialize(MfScVariableStack *outer);                          // 0x08
    // enters the start state; 0 if it is running already or has none
    virtual int Start();                                                        // 0x0c
    // makes state the current one; returns the previous one
    virtual MfState *SetCurrentState(MfState *state);                           // 0x10
    virtual void Slot5(int, int) {}                                             // 0x14
    virtual void Slot6(int) {}                                                  // 0x18
    // transitions triggered by events listen to the machine
    virtual int AddListener(MfStateTransition *listener);                       // 0x1c
    virtual void RemoveListener(MfStateTransition *listener);                   // 0x20
    virtual void Slot9(int, int, int) {}                                        // 0x24
    virtual int Slot10() { return 0; }                                          // 0x28
    virtual void Slot11(int) {}                                                 // 0x2c
    virtual int Slot12() { return 0; }                                          // 0x30
    virtual void Slot13(int) {}                                                 // 0x34
    virtual void Slot14(int, int) {}                                            // 0x38
    virtual int Slot15() { return 0; }                                          // 0x3c
    virtual void Slot16(int) {}                                                 // 0x40
    virtual void Slot17() {}                                                    // 0x44
    virtual void Slot18() {}                                                    // 0x48
    virtual void Slot19() {}                                                    // 0x4c
    virtual void Slot20(int, int) {}                                            // 0x50
    // tells the user (the editor shows a message box)
    virtual void ShowMessage(int type, MfString text) {}                        // 0x54
    virtual void Slot22(int, int, int) {}                                       // 0x58
    virtual void Slot23(int) {}                                                 // 0x5c
    virtual void Slot24(int) {}                                                 // 0x60
    virtual int Slot25() { return 0; }                                          // 0x64
    virtual void Slot26() {}                                                    // 0x68
    virtual void Slot27() {}                                                    // 0x6c
    virtual void Slot28() {}                                                    // 0x70
    virtual void Slot29(int) {}                                                 // 0x74
    virtual void Slot30(int) {}                                                 // 0x78
    // removes the substates, transitions, ports and choice points of the root
    virtual void RemoveStructure();                                             // 0x7c
    // the editor's camera settings of the behaviour
    virtual void SetCameraSettings(MfStateProperty *settings) {}                // 0x80
    // a part of the behaviour goes away
    virtual void PartDeleted(MfFSMMutable *part) {}                             // 0x84
    // a part was added to the behaviour
    virtual void PartAdded(MfFSMMutable *part) {}                               // 0x88

    MfString GetChoice() { return mChoice; }
    // the branch the running choice point takes (MfPoHFSM.cpp, 1006cc40)
    int SetChoice(MfString choice);
    // is the behaviour played back from a recording? (1007f950)
    int IsPlayingBack();
    // the parts' names changed: updates their paths (1007f740)
    void UpdatePaths();
    // deletes the queued events
    void ClearEvents();
    // fires the first transition of the current state (or of its ancestors)
    // that the event triggers
    MfStateTransition *HandleEvent(MfMsg *msg);
    // runs the exit actions, the transition's action and the entry actions
    // and makes the state the transition ends in the current one
    void FireTransition(MfStateTransition *transition);   // 1007e790
    // collects the actions of transition from state into mEvents; returns the
    // state it ends in, 0 if it does not end in a state
    MfState *CollectActions(MfState *state, MfStateTransition *transition);   // 1007ea90
    // adds state and its substates to mProperties
    void CollectStates(MfState *state);
    void ClearListeners();
    // the number of states below the root
    int NumStates();
    MfString GetFile() { return mFile; }
    void Link();
    // reads the behaviour's compiled object file ('.pobj') (1007f4d0)
    void LoadCompiled();
    // compiles the parts into the object file file; returns the number of failures
    int WriteObjectFile(MfPcErrorCallBack *errors, int *error, MfString file);    // 1007efd0
    void Unload();
    MfFSMMutable *Find(MfString path, MfStringList *names, MfDblListItem *pos, int depth);

    MfState *mCurrent;                                                          // 0x04
    MfState *mStart;                                                            // 0x08 the state the machine starts in
    MfState *mRoot;                                                             // 0x0c
    MfStateProperty *mPlayBack;                                                 // 0x10 the 'playback' part
    MfStateTransition *mFiring;                                                 // 0x14 the transition being checked or fired
    MfState *mExecuting;                                                        // 0x18 the state whose script runs
    MfString mFile;                                                             // 0x1c
    MfStatePropertyList mProperties;                                            // 0x20
    MfString mChoice;                                                           // 0x34 the branch a choice point's script picked
    MfDblList mEvents;                                                          // 0x38 the actions the firing transition runs (MfFSMEvent)
    MfScHandleDict *mHandles;                                                   // 0x48
    MfScVariableStack *mVarStack;                                               // 0x4c
    MfFSMManager *mManager;                                                     // 0x50
    MfStateTransition *mListeners[MAX_LISTENERS];                               // 0x54
    int mNumListeners;                                                          // 0xa4
    int mListening;                                                             // 0xa8
    int mac, mb0, mb4, mb8;                                                     // 0xac
};

// a script action a transition runs: the exit action of a state it leaves,
// its own action, or the entry action of a state it enters (the destructor
// is emitted in the message unit, 100649d0)
struct MfFSMEvent {
    MfFSMEvent(MfScDataHandle *function, MfScVariableStack *stack, MfState *state)
        : mFunction(function), mStack(stack), mState(state) {}
    ~MfFSMEvent();

    MfScDataHandle *mFunction;                                                  // 0x00
    MfScVariableStack *mStack;                                                  // 0x04 the variables it runs with
    MfString mName;                                                             // 0x08
    MfState *mState;                                                            // 0x0c the state it belongs to, 0 for the transition's action
};

#endif
