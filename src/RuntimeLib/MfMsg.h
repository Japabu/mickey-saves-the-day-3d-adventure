// MfMsg: a message delivered to an MfRootObj; mID selects the handler in the
// receiver's message table. A message can carry a reference-counted
// MfMsgDataBlock. Class names from the RTTI type descriptors; layouts from
// the constructors at 10063d10-10064870.
#ifndef RUNTIMELIB_MFMSG_H
#define RUNTIMELIB_MFMSG_H
class MfMsgDataBlock;
class MfRootObj;

// message IDs (index into the receivers' message tables)
enum {
    MSG_DESTROY_ACTOR = 9,
    MSG_STATIC_ACTOR_CHANGE = 12,
    MSG_DESTROY_SPACE = 49,
    MSG_JOYSTICK_EVENT = 54,
    MSG_SOUND_FINISHED = 56,
    MSG_TELEPORT_ACTOR = 60,
    NUM_MSGS = 62
};

class MfMsg {
public:
    MfMsg();
    MfMsg(int id, MfRootObj *from, MfRootObj *to);
    MfMsg(int id, MfRootObj *from, MfRootObj *to, int fromID, int toID);
    MfMsg(int id, int arg1, int arg2, MfRootObj *from, MfRootObj *to);
    virtual ~MfMsg();
    void SetData(MfMsgDataBlock *data);

    int mID;                                                // 0x04
    MfRootObj *mFrom;                                       // 0x08
    MfRootObj *mTo;                                         // 0x0c
    int mFromID;                                            // 0x10
    int mToID;                                              // 0x14
    int m18;                                                // 0x18
    int mArg1;                                              // 0x1c
    int mArg2;                                              // 0x20
    int m24;                                                // 0x24
    int m28;                                                // 0x28
    int mToSelf;                                            // 0x2c
    int mIsTimer;                                           // 0x30
    int mTimerID;                                           // 0x34
    int m38;                                                // 0x38
    union {                                                 // 0x3c
        void *mObject;                                      // the object the message is about
        int mEventID;                                       // or the ID of an event message
    };
    int m40;                                                // 0x40
    MfMsgDataBlock *mData;                                  // 0x44
};

// The data a message carries, shared by reference count between the copies
// of a message (MfMsg::SetData takes a reference, ~MfMsg drops it).
class MfMsgDataBlock {
public:
    MfMsgDataBlock() { mRefCount = 0; }
    virtual ~MfMsgDataBlock() { FreeContents(); }
    // frees what the block owns before the receiver discards it
    virtual void FreeContents() {}

    long mRefCount;                                         // 0x04
};

#endif
