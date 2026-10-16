// MfRootObj: the base of RuntimeLib's message-driven objects (actors,
// activities, spaces, the world). A message is dispatched through mMsgTable,
// indexed by the message ID; InitMsgTable fills the table. The class is not
// exported; its name comes from the vtable names ??_7MfActor@@6BMfRootObj@@@.
#ifndef RUNTIMELIB_MFROOTOBJ_H
#define RUNTIMELIB_MFROOTOBJ_H
#include "MfMsg.h"
#include "MfString.h"

class MfRootObj {
public:
    typedef void (MfRootObj::*MsgHandler)(MfMsg &msg);
    enum { TYPE = 'Root' };

    MfRootObj();
    ~MfRootObj();
    virtual void CatchMsg(MfMsg &msg);
    virtual MfString GetName() { return NULL_STRING; }
    virtual void DefaultMsgHandler(MfMsg &) {}
    virtual int Type() { return TYPE; }
    virtual int IsType(int type) { return type == TYPE; }
protected:
    virtual void InitMsgTable();

public:
    void Register();
    void Unregister();

    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;
    static int sNextID;

    MsgHandler *mMsgTable;                                  // 0x04
    int mID;                                                // 0x08
    long mBusy;                                             // 0x0c
    int mRegistered;                                        // 0x10
};

#endif
