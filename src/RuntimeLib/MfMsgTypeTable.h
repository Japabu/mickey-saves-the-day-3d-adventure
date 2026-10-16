// MfMsgTypeTable: the names of the message types (behaviour scripts send
// events by name). Not exported; the names are descriptive.
#ifndef RUNTIMELIB_MFMSGTYPETABLE_H
#define RUNTIMELIB_MFMSGTYPETABLE_H
class MfDblList;
class MfString;

// a message type and its sub type (MfMsg::mID, MfMsg::m38)
struct MfMsgTypeInfo {
    MfMsgTypeInfo() {}
    MfMsgTypeInfo(int t, int s) : type(t), sub(s) {}

    int type;                                               // 0x00
    int sub;                                                // 0x04
};

// The tables are shared by all instances (the first one fills them, the last
// one frees them); unit 100648d0-10067bd0.
class MfMsgTypeTable {
public:
    MfMsgTypeTable();
    ~MfMsgTypeTable();
    MfMsgTypeInfo Lookup(const MfString &name);
    MfString *Name(MfMsgTypeInfo *info);                    // 0 if unknown
protected:
    // the predefined names (100649e0)
    void Init();
    // the type of a name, a new event type for an unknown name
    MfMsgTypeInfo *Find(const MfString &name);
public:
    MfDblList *mInfos;                                      // 0x00 the MfMsgTypeInfos it allocated
    int mNextSub;                                           // 0x04 the next event sub type
    int mFirstSub;                                          // 0x08 1000
};

extern MfMsgTypeTable *g_msgTypes;

#endif
