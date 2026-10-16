// MfTransitionProperty: the base of the parts of a behaviour's transitions
// (destination, trigger, action ...), and the parts. RTTI names; vtable
// 100b6afc (24 slots) and the subclasses' vtables; units 100896d0-1008be50.
// RuntimeLib internal: the method names are descriptive. A part names its
// target (a state, port or choice point) and finds it when the transition
// is attached to a machine (FormatForExecution).
#ifndef RUNTIMELIB_MFTRANSITIONPROPERTY_H
#define RUNTIMELIB_MFTRANSITIONPROPERTY_H
#include "MfFSMMutable.h"
#include "MfMsgTypeTable.h"
class MfScDataHandle;
class MfState;
class MfStateProperty;
class MfStateTransition;

class MfTransPropList : public MfDblList {
public:
    MfTransPropList(int ownsItems);
    virtual ~MfTransPropList();
    // deletes the items it owns
    virtual void Clear();
    // removes item; deletes it if the list owns it and keep is not set
    void *Remove(void *item, int keep);
    int Contains(void *item);

    int mOwnsItems;                                                             // 0x10
};

class MfTransitionProperty : public MfFSMMutable {
public:
    MfTransitionProperty();
    virtual ~MfTransitionProperty();
    virtual void FormatForExecution(MfFiniteStateMachine *machine) { mMachine = machine; }   // 0x58
    virtual void Slot23(int, int) {}                                            // 0x5c

    int mType;                                                                  // 0x14
    MfStateTransition *mTransition;                                             // 0x18
    MfFSMMutable *m1c;                                                          // 0x1c
};

// the destination of a transition in a choice point ('dest_choice_pt')
class MfTransChoicePt : public MfTransitionProperty {
public:
    enum { TYPE = 9 };

    MfTransChoicePt(MfString state, MfString choicePoint);
    virtual ~MfTransChoicePt();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void FormatForExecution(MfFiniteStateMachine *machine);

    MfString mChoicePoint;                                                      // 0x20
    MfString mState;                                                            // 0x24
    MfString mTargetPath;                                                       // 0x28
    MfStateProperty *mTarget;                                                   // 0x2c
};

// the destination state of a transition ('dest_state')
class MfTransDestState : public MfTransitionProperty {
public:
    enum { TYPE = 3 };

    MfTransDestState(MfString state);
    virtual ~MfTransDestState();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual MfString &GetName() { return mState; }
    virtual void FormatForExecution(MfFiniteStateMachine *machine);

    MfState *mTarget;                                                           // 0x20
    MfString mState;                                                            // 0x24
    MfString mTargetPath;                                                       // 0x28
};

// the destination of a transition through an entry port ('enterby')
class MfTransEnterBy : public MfTransitionProperty {
public:
    enum { TYPE = 8 };

    MfTransEnterBy(MfString state, MfString port);
    virtual ~MfTransEnterBy();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void FormatForExecution(MfFiniteStateMachine *machine);

    MfString mState;                                                            // 0x20
    MfString mTargetPath;                                                       // 0x24
    MfString mPort;                                                             // 0x28
    MfStateProperty *mTarget;                                                   // 0x2c
};

// a transition that leaves through an exit port ('exitby')
class MfTransExitBy : public MfTransitionProperty {
public:
    enum { TYPE = 7 };

    MfTransExitBy(MfString port);
    virtual ~MfTransExitBy();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void FormatForExecution(MfFiniteStateMachine *machine);

    MfString mPort;                                                             // 0x20
    MfString mTargetPath;                                                       // 0x24
    MfStateProperty *mTarget;                                                   // 0x28
};

// the initial transition ('init')
class MfTransInit : public MfTransitionProperty {
public:
    enum { TYPE = 6 };

    MfTransInit() { mType = TYPE; }
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);
};

// the kind of a transition ('trans_type': jump, self_internal, self_external)
class MfTransitionType : public MfTransitionProperty {
public:
    enum { TYPE = 5 };
    enum { JUMP = 0, SELF_INTERNAL = 1, SELF_EXTERNAL = 2 };

    MfTransitionType(const MfString &kind);
    virtual ~MfTransitionType();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);

    MfString mKindName;                                                         // 0x20
    int mKind;                                                                  // 0x24
};

// the action of a transition ('trig_action {!*!...!*!}'; vtable 100b6c54,
// unit 1008a9e0-1008b280)
class MfTransTrigAction : public MfTransitionProperty {
public:
    enum { TYPE = 2 };

    MfTransTrigAction(const MfString &source);
    virtual ~MfTransTrigAction();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual void SetPath(MfString parent);
    virtual MfString *GetScript() { return &mSource; }
    virtual void SetScript(const MfString &source) { mSource = source; }
    virtual void Search(MfString text, int, int caseSensitive, int, int what, int inScripts, int, int,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn);

    // is there more than the braces?
    int HasScript();

    MfString mSource;                                                           // 0x20
    MfScDataHandle *mFunction;                                                  // 0x24
};

// a trigger of a transition: the event it waits for (by name) and a guard
// condition ('trig_by <event> { trig_guard {!*!...!*!} }'; vtable 100b6ccc,
// unit 1008b280-1008be20)
class MfTransTriggeredBy : public MfTransitionProperty {
public:
    enum { TYPE = 1 };

    MfTransTriggeredBy(const MfString &event, MfString *guard);
    virtual ~MfTransTriggeredBy();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual void SetPath(MfString parent);
    virtual MfString *GetScript() { return mGuard; }
    virtual void SetScript(const MfString &source) { SetGuard(source); }
    virtual void Search(MfString text, int, int caseSensitive, int, int, int inScripts, int inNames, int,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn);

    void SetGuard(MfString guard);
    // is there a guard with more than the braces?
    int HasGuard();
    // the message type of the event
    void LookupEvent();

    MfString *mGuard;                                                           // 0x20
    MfMsgTypeInfo mEvent;                                                       // 0x24 the message type
    MfScDataHandle *mCondition;                                                 // 0x2c the compiled guard
};

// every trigger there is
extern MfDblList *gTriggers;

class Mf2DVectorList;

// a connector line of the editor's drawing ('visual_con')
class MfVisualConnector : public MfTransitionProperty {
public:
    enum { TYPE = 4 };

    MfVisualConnector(double x, double y, Mf2DVectorList *points);
    virtual ~MfVisualConnector();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);

    double mX, mY;                                                              // 0x20
    Mf2DVectorList *mPoints;                                                    // 0x30
};

#endif
