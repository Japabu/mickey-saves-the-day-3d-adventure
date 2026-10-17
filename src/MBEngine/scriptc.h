// scriptc.h - declarations for MBEngine.dll scriptc.obj: the script compiler, a
// bison parser (yyparse) over a flex scanner (yylex) that writes the code block a
// TExecutor runs (exec.cpp).
#ifndef SCRIPTC_H
#define SCRIPTC_H

#include "MBStl.h"
#include <string> // evidence: ctype<unsigned short>::id $E35/$E36

// Numbering stand-in: the original has one more numbered item ahead of gBackPatchStack ($E17)
// than <string> and the two TSymtab statics give; name unknown.
struct ScriptcNumbering {
    static int sUnknownScriptcNumbering;
};
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <deque>
#include <map>
#include <stack>
#include <vector>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) void __cdecl MBDebugStr(const char *fmt, ...);
extern "C" __declspec(dllimport) unsigned long __cdecl MBHashString(const char *);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// PDB ldata order, scriptc.obj 0x1008b968..0x1008b998
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameClock = { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// smart pointer (CMBGameEngine.h)
// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
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

#include "IMBStreamSeekValues.h"
#include "IMBStreamError.h"

#include "IMBStream.h"

#include "IMBGameClock.h"

// the containers of the muffin code; no construct/destroy (std::_Construct is instantiated)
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

template <class T> class MBStack : public std::stack<T, std::deque<T, MBAllocator<T> > > {
};

#include "../mbruntime/CMBCriticalSection.h"

// ---- script symbol table (symtable.h) ----
enum {
    SymtabNumVariable = 0,
    SymtabStrLiteral = 1,
    SymtabStrVariable = 2,
    SymtabFunVariable = 3
};

#include "TSymtabNode.h"

// own copy: without symtable.h's $E stand-in static (sUnknownSymtabStatic)
class TSymtab {
    TSymtabNode *root;
    long nodeCnt;
    long symbolTableIndex;
    static long symbolTableCnt;
    static TSymtab *symbolTableList;
    TSymtab *next;
public:
    TSymtab(void);
    ~TSymtab(void);
    TSymtabNode *Search(const char *pString) const;
    TSymtabNode *Enter(const char *pString);
    TSymtabNode *Root(void) const { return root; }
    unsigned long StreamOut(IMBStream *outFile);
};

// ---- the virtual machine's opcodes (exec.cpp runs them) ----
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

// ---- code generation ----
// an address written as a placeholder, patched when its block closes
struct BackpatchNode {
    long pos;
    long where;
    long targetBlock;
};

struct BackpatchContext {
    long nesting;
    long blockCnt;
    MBVector<BackpatchNode> list;
};

struct FunctionStruct {
    short offset;
};

void HardExit(const char *str);
unsigned long SafeTell(IMBStream *inFile);
void SafeSeek(IMBStream *inFile, long offset, IMBStreamSeekValues origin);
int CompileScript(IMBStream *inStream, IMBStream *outStream, unsigned long options);
void WriteNumRef(float num);
void WriteNumSymRef(TSymtabNode *tptr);
void WriteStrSymRef(TSymtabNode *tptr);
void AddBackpatch(long where);
const char *UniqueName(void);
void StartFunctionBlock(const char *name);
void EndFunctionBlock(void);
void CallFunction(const char *name);

// ---- parser interface (the bison y.tab.h) ----
union YYSTYPE {
    float val;
    TSymtabNode *tptr;
};
extern YYSTYPE yylval;

#define NUMBER 258
#define NUMVAR 259
#define STRVAR 260
#define STRING 261
#define EOL 262
#define END_OF_FILE 263
#define START 264
#define AND 265
#define WAIT 266
#define FOR 267
#define ANIM 268
#define ON 269
#define SEQUENCE 270
#define AT 271
#define POINT 272
#define OF 273
#define MILLISECONDS 274
#define YIELD 275
#define STOP 276
#define MOVIE 277
#define WITH 278
#define END 279
#define PAUSE 280
#define SOUND 281
#define GOTO 282
#define NODE 283
#define CHANNEL 284
#define ALL 285
#define SCRIPT 286
#define SCRIPTS 287
#define CREATE 288
#define COPY 289
#define NAMED 290
#define NEW 291
#define NOTIFY 292
#define RESUME 293
#define EVERYTHING 294
#define TO 295
#define RANDOM 296
#define PRINT 297
#define SIN 298
#define COS 299
#define SQRT 300
#define IF 301
#define THEN 302
#define ELSE 303
#define REPEAT 304
#define WHILE 305
#define NOT 306
#define OR 307
#define EQ 308
#define DOWN 309
#define GT 310
#define GE 311
#define LT 312
#define LE 313
#define NE 314
#define MOVE 315
#define FOREGROUND 316
#define BACKGROUND 317
#define VOLUME 318
#define STRINGKW 319
#define STRCMP 320
#define STRICMP 321
#define QUEUE 322
#define FORCE 323
#define PLACE 324
#define BY 325
#define FLAGS 326
#define FUNCTION 327
#define LOOPED 328

int yyparse(void);
int yylex(void);
int yyerror(char *s);

// ---- flex buffer ----
#include "yy_buffer_state.h"

#endif
