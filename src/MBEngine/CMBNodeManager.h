// CMBNodeManager.h - declarations for MBEngine.dll CMBNodeManager.obj
#ifndef CMBNODEMANAGER_H
#define CMBNODEMANAGER_H

// copied from CMBView.h (colour statics first: $E1..$E48 precede every STL number)
typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); } // ??_G MBComPtr<IMBNode>: test esi before MBfree_release

#include "../MBCom/MBGUID.h"

// Color constants: internal-linkage objects defined in the colour header; each
// TU that includes it gets 16 dynamic initializers ($E1..$E47, PDB ldata).
#include "MBColorMask.h"

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

#include "../MBEngine/MBColor.h"

const MBColor kWhite(255, 255, 255, 0);
const MBColor kBlack(0, 0, 0, 0);
const MBColor kRed(255, 0, 0, 0);
const MBColor kGreen(0, 255, 0, 0);
const MBColor kBlue(0, 0, 255, 0);
const MBColor kYellow(255, 255, 0, 0);
const MBColor kCyan(0, 255, 255, 0);
const MBColor kMagenta(255, 0, 255, 0);
const MBColor kGrey25(64, 64, 64, 0);
const MBColor kGrey50(128, 128, 128, 0);
const MBColor kGrey75(192, 192, 192, 0);

#include "MBPointStruct.h"

#include "../MBEngine/MBPoint.h"

#include "MBSize.h"

#include "../MBEngine/MBRect.h"


#include <string>    // evidence: ctype<unsigned short> id pair ($E69/$E70)
#include <list>
#include <deque>
#include <queue>
#include <typeinfo.h>

// Stand-ins (see NOTES): the PDB numbering has the Make guard at $S64 and the ctype pair at
// $E69/$E70; this header without them gives $S62 and $E67/$E68, so two numbered items
// precede MakeCMBNodeManager. No evidence names them (no extra statics/guards in the
// obj's PDB data); declared-only static data members stand in.
struct MBNodeManagerStandIn {
    static int sUnknown1;
    static int sUnknown2;
};

// PDB ldata order in CMBNodeManager.obj (0x1008f528..0x1008f5b8)
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical =
    { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager =
    { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNode =
    { 0x795ecbe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNodeManager =
    { 0x7984ff90, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener =
    { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);
extern "C" IMBUnknown *MBComUtilAddSingleton(IMBUnknown *unk);
extern "C" __declspec(dllimport) void MBDebugStr(const char *format, ...);

// allocator without construct/destroy: deque::push_back calls the out-of-line std::_Construct
template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

template <class T> class MBQueue : public std::queue<T, std::deque<T, MBAllocator<T> > > {
};

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(int) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    MBComPtr(const MBComPtr &ptr) : mInterface(ptr.mInterface) { AddRef(); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    operator bool(void) const { return mInterface != 0; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    MBComPtr &operator=(const MBComPtr &p) { return *this = p.mInterface; }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void AddRef(void) {
        if (mInterface) mInterface->AddRef();
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p); // emitted in another object
    T *mInterface;
};

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

#include "IMBNode.h"

#include "IMBNodeManager.h"

enum NodeActionType {
    kNodeActionNone = 0,
    kNodeActionPop = 1,
    kNodeActionPush = 2,
    kNodeActionClear = 3
};

class NodeAction {
public:
    NodeAction(NodeActionType type, MBComPtr<IMBNode> node) : mType(type), mNode(node) {}
    NodeAction(const NodeAction &rhs) : mType(rhs.mType), mNode(rhs.mNode) {}
    NodeActionType mType;        // +0x00
    MBComPtr<IMBNode> mNode;     // +0x04
};

class CMBNodeManager : public IMBNodeManager, public IMBPeriodical, public IMBEventListener {
public:
    CMBNodeManager(void);
    ~CMBNodeManager(void);
protected:
    unsigned long _mRefCnt;                  // +0x0c
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual IMBNode *GetActiveNode(void) const;
    virtual void QueuePush(MBComPtr<IMBNode> node);
    virtual void QueuePop(void);
    virtual void QueueSwap(MBComPtr<IMBNode> node);
    virtual void QueueClear(void);
    virtual void Idle(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    void ExecutePush(MBComPtr<IMBNode> node);
    void ExecutePop(void);
    void ExecuteClear(void);
protected:
    MBList<MBComPtr<IMBNode> > mNodes;       // +0x10
    MBQueue<NodeAction> mActions;            // +0x1c
};

IMBUnknown *MakeCMBNodeManager(void);

#endif
