// MfStateProperty: the base of the parts of a behaviour's states (states,
// notes, actions, ports ...); MfStatePropertyList: a list of them that may
// own its items. RTTI names; vtables 100b6644 (41 slots) and 100b66ec; unit
// 10087c00-10087f50. RuntimeLib internal: the method names are descriptive,
// the slots whose role is not known yet are named after their slot.
#ifndef RUNTIMELIB_MFSTATEPROPERTY_H
#define RUNTIMELIB_MFSTATEPROPERTY_H
#include "MfFSMMutable.h"
class MfStateProperty;
class MfScVariableStack;

class MfStatePropertyList : public MfDblList {
public:
    MfStatePropertyList(int ownsItems);
    virtual ~MfStatePropertyList();
    // deletes the items it owns
    virtual void Clear();
    void AddProperty(MfStateProperty *property);
    int Contains(void *item);
    // removes item; deletes it if the list owns it and keep is not set
    void *Remove(void *item, int keep);
    void *Remove(MfDblListItem *&pli, void *item, int keep);

    int mOwnsItems;                                                             // 0x10
};

// the four integers some property slots return
struct MfStatePropertyRect {
    MfStatePropertyRect() { left = 0; right = 0; top = 0; bottom = 0; }
    int left, top, right, bottom;
};

class MfStateProperty : public MfFSMMutable {
public:
    enum { TYPE = 100 };

    MfStateProperty();
    virtual ~MfStateProperty();
    virtual void CollectAll(MfDblList *list);
    // attaches the part to the machine that runs it and resolves its references
    virtual void FormatForExecution(MfFiniteStateMachine *machine) { mMachine = machine; }   // 0x58
    virtual void SetParent(MfStateProperty *parent) { mParent = parent; }          // 0x5c
    // the part it belongs to (the state a transition leaves)
    virtual MfStateProperty *GetOwner() { return 0; }                           // 0x60
    // the script variables the part sees
    virtual MfScVariableStack *GetVariableStack() { return 0; }                 // 0x64
    virtual int Slot26(int, int) { return 0; }                                  // 0x68
    virtual void Slot27(int, int) {}                                            // 0x6c
    virtual void Slot28(int, int) {}                                            // 0x70
    virtual void Slot29(int, int, int) {}                                       // 0x74
    virtual void Slot30(int, int, int, int) {}                                  // 0x78
    virtual void Slot31(int, int, int) {}                                       // 0x7c
    virtual void Slot32(int, int, int) {}                                       // 0x80
    virtual void Slot33(int, int, int, int, int, int, int, int, int, int, int, int, int, int) {}  // 0x84
    virtual void Slot34(int, int, int, int) {}                                  // 0x88
    virtual void Slot35(int, int, int, int) {}                                  // 0x8c
    virtual void Slot36(int, int, int) {}                                       // 0x90
    // adds part to the part's properties
    virtual int AddPart(MfStateProperty *part) { return 0; }                    // 0x94
    virtual MfStatePropertyRect Slot38(int) { return MfStatePropertyRect(); }   // 0x98
    // takes part out of the part's properties; deletes it unless keep
    virtual int RemovePart(MfStateProperty *part, int keep) { return 0; }       // 0x9c
    virtual void Slot40(int) {}                                                 // 0xa0

    // copies of the properties of list become this part's properties
    void SetProperties(MfStatePropertyList *list);

    int mType;                                                                  // 0x14
    MfStateProperty *mParent;                                                   // 0x18
    MfStatePropertyList *mProperties;                                           // 0x1c
    MfStateProperty *mDisplayPos;                                               // 0x20 where the editor shows it
    int m24;                                                                    // 0x24
};

#endif
