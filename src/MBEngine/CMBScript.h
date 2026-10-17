// CMBScript.h - declarations for MBEngine.dll CMBScript.obj: the script object that
// compiles a script stream (scriptc), runs it in a TExecutor and turns its waits into
// event-manager and game-clock waits.
#ifndef CMBSCRIPT_H
#define CMBSCRIPT_H

#include <string>
#include <map>

extern "C" __declspec(dllimport) void *MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *);
extern "C" __declspec(dllimport) void MBDebugStr(const char *, ...);

inline void *operator new(unsigned int size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

// Stand-ins (see NOTES): the colour statics are $E21..$E68 in the original and the anim
// tracker's statics start at $E72. The script runtime's static data members (TSymtab 2,
// TExecutor 1) and <string> account for 16 numbers before the colours; four more precede
// them and one more follows them. No evidence names them (CMBScript.obj's PDB data has no
// further statics or guards); declared-only static data members stand in.
struct CMBScriptStandIn1 {
    static int sUnknown1;
    static int sUnknown2;
    static int sUnknown3;
    static int sUnknown4;
};

class IMBStream;
class CMBScript;

// ---- script runtime (symtable.h, stack.h, exec.cpp) ----
enum {
    SymtabNumVariable = 0,
    SymtabStrLiteral = 1,
    SymtabStrVariable = 2,
    SymtabFunVariable = 3
};

// own copy: adds the inline String() this unit uses (SymtabStrLiteral is not declared in symtable.h)
class TSymtabNode {
    TSymtabNode *left;
    TSymtabNode *right;
    unsigned long nodeIndex;
    char name[32];
public:
    union {
        float num;
        char str[32];
    } value;
    unsigned long type;
    void *param;

    TSymtabNode(void);
    TSymtabNode(const char *theName, long index);
    ~TSymtabNode(void);
    const char *Name(void) { return name; }
    TSymtabNode *LeftSubtree(void) { return left; }
    TSymtabNode *RightSubtree(void) { return right; }
    void SetValue(float num);
    void SetValue(long num);
    void SetValue(const char *str);
    unsigned long Index(void) const { return nodeIndex; }
    void Print(void) const;
    unsigned long StreamOut(IMBStream *outFile);
    unsigned long StreamIn(const char *buffer);
    // a string literal's text is its name
    const char *String(void) {
        if (type == SymtabStrLiteral)
            return name;
        return value.str;
    }
};

// own copy: without symtable.h's $E stand-in static (sUnknownSymtabStatic)
class TSymtab {
    TSymtabNode *root;
    long nodeCnt;
    long symbolTableIndex;
    static long symbolTableCnt;
    static TSymtab *symbolTableList;
    TSymtab *next;
};

union TRuntimeStackItem {
    const char *str;
    float num;
    short addr;
    short index;
};

#include "TRuntimeStack.h"

enum {
    vmRegisterCnt = 4
};

enum {
    vmCompilerVersion = 7
};

enum {
    vmFirstOp = 0,
    vmNumber = 1,
    vmAddress = 2,
    vmSymStr = 3,
    vmIndex = 3,
    vmSymNum = 4,
    vmPop = 5,
    vmPopToRegister = 6,
    vmPushFromRegister = 7,
    vmJumpIfZero = 8,
    vmJump = 9,
    vmPrintNum = 10,
    vmPrintStr = 11,
    vmFirstMathOp = 12,
    vmNumAssign = 13,
    vmAdd = 14,
    vmSub = 15,
    vmMul = 16,
    vmDiv = 17,
    vmMod = 18,
    vmNeg = 19,
    vmExp = 20,
    vmRand = 21,
    vmSin = 22,
    vmCos = 23,
    vmSqrt = 24,
    vmBoolEq = 25,
    vmBoolNe = 26,
    vmBoolOr = 27,
    vmBoolAnd = 28,
    vmBoolNot = 29,
    vmBoolGt = 30,
    vmBoolGe = 31,
    vmBoolLt = 32,
    vmBoolLe = 33,
    vmLastMathOp = 34,
    vmFirstStringOp = 35,
    vmStrAssign = 36,
    vmStrAppend = 37,
    vmNumToStr = 38,
    vmStrCmp = 39,
    vmStrICmp = 40,
    vmLastStringOp = 41,
    vmFirstActorOp = 42,
    vmStartActorOnPathAtPoint = 43,
    vmWaitForActor = 44,
    vmStopActor = 45,
    vmCopyActor = 46,
    vmMoveActor = 47,
    vmPlaceActor = 48,
    vmLastActorOp = 49,
    vmFirstSoundOp = 50,
    vmStartMovie = 51,
    vmWaitForMovie = 52,
    vmStopMovie = 53,
    vmStartSound = 54,
    vmWaitForSound = 55,
    vmStopSound = 56,
    vmStopSoundChannel = 57,
    vmStopAllSound = 58,
    vmLastSoundOp = 59,
    vmFirstMiscOp = 60,
    vmStartScript = 61,
    vmWaitForScript = 62,
    vmStopScript = 63,
    vmStopAllScripts = 64,
    vmBackgroundScript = 65,
    vmNotify = 66,
    vmNotifyLine = 67,
    vmWaitNMilliseconds = 68,
    vmGotoScene = 69,
    vmPauseEverything = 70,
    vmLastMiscOp = 71,
    vmLastOp = 72
};

// the script virtual machine (exec.cpp); handlers[] is indexed by the vm* opcodes
class TExecutor {
public:
    char *code;
    char *cursor;
    char *codeEnd;
    TSymtabNode **arr;
    long nodeCnt;
    char ok;
    TRuntimeStack stack;
    CMBScript *imp;
    short paused;
    float registers[4];
    static void (__cdecl *handlers[73])(TExecutor *);

    TExecutor(CMBScript *obj, char *codeBlock);
    ~TExecutor(void);
    void Go(void);
    TSymtabNode *Search(const char *varname);
    bool SetScriptVariable(const char *varname, const char *value);
    bool SetScriptVariable(const char *varname, double value);
    bool GetScriptVariable(const char *varname, const char **valueDest);
    bool GetScriptVariable(const char *varname, double *valueDest);
    static void __cdecl BadOpcodeHandler(TExecutor *This);
    static void __cdecl VMNumber(TExecutor *This);
    static void __cdecl VMAddress(TExecutor *This);
    static void __cdecl VMSymStr(TExecutor *This);
    static void __cdecl VMSymNum(TExecutor *This);
    static void __cdecl VMPop(TExecutor *This);
    static void __cdecl VMPrintNum(TExecutor *This);
    static void __cdecl VMPrintStr(TExecutor *This);
    static void __cdecl VMJumpIfZero(TExecutor *This);
    static void __cdecl VMJump(TExecutor *This);
    static void __cdecl VMNumAssign(TExecutor *This);
    static void __cdecl VMAdd(TExecutor *This);
    static void __cdecl VMSub(TExecutor *This);
    static void __cdecl VMMul(TExecutor *This);
    static void __cdecl VMDiv(TExecutor *This);
    static void __cdecl VMMod(TExecutor *This);
    static void __cdecl VMNeg(TExecutor *This);
    static void __cdecl VMExp(TExecutor *This);
    static void __cdecl VMRand(TExecutor *This);
    static void __cdecl VMSin(TExecutor *This);
    static void __cdecl VMCos(TExecutor *This);
    static void __cdecl VMSqrt(TExecutor *This);
    static void __cdecl VMBoolEq(TExecutor *This);
    static void __cdecl VMBoolNe(TExecutor *This);
    static void __cdecl VMBoolOr(TExecutor *This);
    static void __cdecl VMBoolAnd(TExecutor *This);
    static void __cdecl VMBoolNot(TExecutor *This);
    static void __cdecl VMBoolGt(TExecutor *This);
    static void __cdecl VMBoolGe(TExecutor *This);
    static void __cdecl VMBoolLt(TExecutor *This);
    static void __cdecl VMBoolLe(TExecutor *This);
    static void __cdecl VMStrAssign(TExecutor *This);
    static void __cdecl VMStrAppend(TExecutor *This);
    static void __cdecl VMNumToStr(TExecutor *This);
    static void __cdecl VMStrCmp(TExecutor *This);
    static void __cdecl VMStrICmp(TExecutor *This);
    static void __cdecl VMStartActorOnPathAtPoint(TExecutor *This);
    static void __cdecl VMWaitForActor(TExecutor *This);
    static void __cdecl VMStopActor(TExecutor *This);
    static void __cdecl VMCopyActor(TExecutor *This);
    static void __cdecl VMMoveActor(TExecutor *This);
    static void __cdecl VMPlaceActor(TExecutor *This);
    static void __cdecl VMStartMovie(TExecutor *This);
    static void __cdecl VMWaitForMovie(TExecutor *This);
    static void __cdecl VMStopMovie(TExecutor *This);
    static void __cdecl VMStartSound(TExecutor *This);
    static void __cdecl VMWaitForSound(TExecutor *This);
    static void __cdecl VMStopSound(TExecutor *This);
    static void __cdecl VMStopSoundChannel(TExecutor *This);
    static void __cdecl VMStopAllSound(TExecutor *This);
    static void __cdecl VMStartScript(TExecutor *This);
    static void __cdecl VMWaitForScript(TExecutor *This);
    static void __cdecl VMStopScript(TExecutor *This);
    static void __cdecl VMStopAllScripts(TExecutor *This);
    static void __cdecl VMBackgroundScript(TExecutor *This);
    static void __cdecl VMNotify(TExecutor *This);
    static void __cdecl VMNotifyLine(TExecutor *This);
    static void __cdecl VMWaitNMilliseconds(TExecutor *This);
    static void __cdecl VMGotoScene(TExecutor *This);
    static void __cdecl VMPauseEverything(TExecutor *This);
};

// colour statics (copied from CMBFPSMonitor.h): $E21..$E68 in this unit
// own copy: this unit uses the constructors or GetShiftCount, whose inline body here differs from MBColorMask.h (store order / code shape the matches depend on)
class MBColorMask {
public:
    MBColorMask(unsigned long r, unsigned long g, unsigned long b, unsigned long a) { Set(r, g, b, a); }
    void Set(unsigned long redmask, unsigned long greenmask, unsigned long bluemask, unsigned long alphamask) {
        mRedMask = redmask;
        mBlueMask = bluemask;
        mGreenMask = greenmask;
        mAlphaMask = alphamask;
        mRedShift = GetShiftCount(redmask) + GetBitCount(redmask) - 8;
        mBlueShift = GetShiftCount(bluemask) + GetBitCount(bluemask) - 8;
        mGreenShift = GetShiftCount(greenmask) + GetBitCount(greenmask) - 8;
        mAlphaShift = GetShiftCount(alphamask) + GetBitCount(alphamask) - 8;
    }
    unsigned long mRedMask;
    unsigned long mBlueMask;
    unsigned long mGreenMask;
    unsigned long mAlphaMask;
    char mRedShift;
    char mBlueShift;
    char mGreenShift;
    char mAlphaShift;
    static unsigned char GetShiftCount(unsigned long mask) {
        unsigned char count = 0;
        while (!(mask & 1) && count < 32) {
            mask >>= 1;
            count++;
        }
        return count;
    }
    static unsigned char GetBitCount(unsigned long mask) {
        unsigned char count = 0;
        while (mask) {
            mask &= mask - 1;
            count++;
        }
        return count;
    }
};

static MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
static MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
static MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
static MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
static MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { Set(r, g, b, a); }
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        Convert(kCM_ARGB32);
    }
    unsigned long Convert(MBColorMask mask) {
        mColor = 0;
        if (mask.mRedShift > 0)
            mColor = ((unsigned long)mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor = ((unsigned long)mRed >> -mask.mRedShift) & mask.mRedMask;
        if (mask.mGreenShift > 0)
            mColor |= ((unsigned long)mGreen << mask.mGreenShift) & mask.mGreenMask;
        else
            mColor |= ((unsigned long)mGreen >> -mask.mGreenShift) & mask.mGreenMask;
        if (mask.mBlueShift > 0)
            mColor |= ((unsigned long)mBlue << mask.mBlueShift) & mask.mBlueMask;
        else
            mColor |= ((unsigned long)mBlue >> -mask.mBlueShift) & mask.mBlueMask;
        if (mask.mAlphaShift > 0)
            mColor |= ((unsigned long)mAlpha << mask.mAlphaShift) & mask.mAlphaMask;
        else
            mColor |= ((unsigned long)mAlpha >> -mask.mAlphaShift) & mask.mAlphaMask;
        return mColor;
    }
    unsigned char mRed;
    unsigned char mGreen;
    unsigned char mBlue;
    unsigned char mAlpha;
    unsigned long mColor;
};

static MBColor kWhite(255, 255, 255, 0);
static MBColor kBlack(0, 0, 0, 0);
static MBColor kRed(255, 0, 0, 0);
static MBColor kGreen(0, 255, 0, 0);
static MBColor kBlue(0, 0, 255, 0);
static MBColor kYellow(255, 255, 0, 0);
static MBColor kCyan(0, 255, 255, 0);
static MBColor kMagenta(255, 0, 255, 0);
static MBColor kGrey25(64, 64, 64, 0);
static MBColor kGrey50(128, 128, 128, 0);
static MBColor kGrey75(192, 192, 192, 0);

struct CMBScriptStandIn2 {
    static int sUnknown5;
};

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &g) const {
        return data1 == g.data1 && data2 == g.data2 && data3 == g.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)g.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)g.data4)[1];
    }
};

// PDB ldata order, CMBScript.obj 0x1008e6b8..0x1008e810
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBScript = { 0x8efbf5d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBScript = { 0x8f021290, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMemStream = { 0x89bfd660, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMemStream = { 0x89c5f320, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameClock = { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMemoryBlock = { 0x7944d1b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMemoryBlock = { 0x79496740, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical = { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager = { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager = { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener = { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager = { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObject = { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup = { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBAnim = { 0x7ec2a630, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMovie = { 0x7f5cfc20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundDescription = { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// smart pointer (CMBFPSMonitor.h): dtor stores 0 after Release; the cross-interface
// _QueryInterface(const MBComPtr<U> &) is a member template
// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) {
        IMBUnknown *unk;
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            _QueryInterface(unk);
            unk->Release();
        }
    }
    template <class U> MBComPtr(const MBComPtr<U> &p) : mInterface(0) { _QueryInterface(p); }
    MBComPtr(const MBComPtr &p) : mInterface(p.mInterface) { if (mInterface) mInterface->AddRef(); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            T *pInterface;
            long hr = unk->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr)
                Attach(0);
            else
                Attach(pInterface);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p); // emitted in another object
    template <class U> long _QueryInterface(const MBComPtr<U> &p) {
        long hr = -3;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
            return hr;
        }
        *this = 0;
        return hr;
    }
    T *mInterface;
};

#include "IMBStreamSeekValues.h"
#include "IMBStreamError.h"

#include "IMBStream.h"

#include "IMBMemoryBlock.h"

#include "IMBMemStream.h"

#include "IMBGameClock.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

class IMBStreamLocator;

#include "IMBSoundDescription.h"

// event data of the sound channel (-4)
#include "SoundEndedEvent.h"

class MBSoundData;

#include "IMBSoundManager.h"

extern "C" {
void MBPlaySound(const char *soundName, long loopCount);
void MBForceSound(const char *soundName, long channelID, long loopCount);
}

#include "MBPointStruct.h"

#include "MBPointStruct3.h"

// own copy: this unit uses the default constructor (empty), whose inline body here differs from MBPoint3.h (store order / code shape the matches depend on)
class MBPoint3 : public MBPointStruct3 {
public:
    MBPoint3(long ax, long ay, long az) { x = ax; y = ay; z = az; }
    MBPoint3(void) {}
};

class MBRect;
class IMBSurface;
class IMBObjectGroup;

#include "IMBObject.h"

// No construct/destroy: the original instantiates std::_Construct for the map's pair
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

// IMBAnim vtable (CMBAnim.h)
class MBPoint;
#include "MBAnimationProperty.h"
#include "MBSequenceProperty.h"
#include "MBFrameProperty.h"

#include "IMBAnim.h"

// event data of the animation channel (-2)
#include "MBAnimEventStruct.h"

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

// MBAnimTracker.h: the anim tracker the script finds its actors with
// own copy: AddRef is inline here: CMBScript.cpp calls it on its static tracker
class MBAnimTracker : public IMBEventListener {
public:
    MBAnimTracker(void);
    ~MBAnimTracker(void);
    void SetupTracker(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void) { return ++_mRefCnt; }
    virtual unsigned long Release(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual IMBAnim *FindAnimation(unsigned long id);
protected:
    virtual void AddAnimation(IMBAnim *anim, unsigned long id);
    virtual void RemoveAnimation(IMBAnim *anim, unsigned long id);
    typedef MBMap<unsigned long, IMBAnim *, std::less<unsigned long> > AnimList;
    AnimList mAnimList;
    bool mAlreadyListening;
};

int CompileScript(IMBStream *inStream, IMBStream *outStream, unsigned long flags);

// ---- the script object ----
#include "IMBScript.h"

// what a script broadcasts on its channel (-6)
#include "MBScriptEvent.h"

// what the script waits for (mWaitChannel says which member is valid)
union HCScriptWaitInfo {
    struct HCScriptWaitEvent {
        unsigned long abs_ms;
    } wait;
    struct HCScriptSceneEvent {
        unsigned long id;
    } node;
    struct HCScriptSoundEvent {
        unsigned long id;
        long channel;
    } sound;
    struct HCScriptMovieEvent {
        unsigned long id;
    } movie;
    struct HCScriptAnimEvent {
        unsigned long id;
        long seq;
    } anim;
    struct HCScriptScriptEvent {
        unsigned long id;
    } script;
};

class CMBScript : public IMBScript, public IMBEventListener, public IMBPeriodical {
public:
    CMBScript(void);
    ~CMBScript(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual bool Load(unsigned long id, IMBStream *inStream);
    virtual void Start(void);
    virtual void Stop(void);
    virtual void Pause(void);
    virtual void Resume(void);
    virtual bool SetScriptVariable(const char *varname, const char *value);
    virtual bool SetScriptVariable(const char *varname, double value);
    virtual bool GetScriptVariable(const char *varname, const char **valueDest);
    virtual bool GetScriptVariable(const char *varname, double *valueDest);
    virtual bool IsStarted(void) const;
    virtual bool IsDone(void) const;
    virtual bool InBackground(void) const;
    virtual long GetID(void) const;
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle(void);

    void StartActor(TSymtabNode *actor, long wait, long path, long x, long y);
    void WaitForActor(TSymtabNode *actor, long path);
    void StopActor(TSymtabNode *actor);
    void CopyActor(TSymtabNode *actor, TSymtabNode *newName, long x, long y, long zplane);
    void MoveActor(TSymtabNode *actor, float x, float y, float z);
    void PlaceActor(TSymtabNode *actor, float x, float y, float z);
    void StartMovie(TSymtabNode *movie, long wait, long flags, long x, long y, long z);
    void WaitForMovie(TSymtabNode *movie);
    void StopMovie(TSymtabNode *movie);
    void StartSound(TSymtabNode *sound, long wait, long channel, long volume, long loop, long queue, long force);
    void WaitForSound(TSymtabNode *sound);
    void StopSound(long channel);
    void StopSound(TSymtabNode *sound);
    void StopAllSound(void);
    void StartScript(TSymtabNode *script, long wait);
    void BackgroundScript(long background);
    void WaitForScript(TSymtabNode *script);
    void StopScript(TSymtabNode *script);
    void StopAllScripts(void);
    void Notify(long linenum);
    void Notify(TSymtabNode *msg);
    void WaitNMilliseconds(long ms);
    void GotoScene(TSymtabNode *scene);
    void PauseEverything(long pause);
private:
    MBComPtr<IMBGameClock> mClock;
    long mID;
    bool started;
    unsigned char ctorOK;
    bool inBackground;
    TExecutor *exec;
    MBComPtr<IMBMemoryBlock> mCodeBlock;
    bool done;
    unsigned char mIsWaiting;
    long mWaitChannel;
    HCScriptWaitInfo mWaitInfo;
    unsigned char paused;
    unsigned long pauseStartMS;
    bool Execute(void);
    bool Rewind(void);
    void Continue(void);
    void SendMessage(long channel, long msg, MBScriptEvent &event);
    void SendMessage(long channel, long msg);
    void StartListening(void);
    void StopListening(void);
};

IMBUnknown *MakeCMBScript(void);

#endif
