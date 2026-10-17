// PscDebugger.dll: the Piccolo script debugger. An MFC extension DLL (MFC42.DLL
// by ordinal): the debugger object (MfPdbDebugger, "PiccoloDebuggerI"), the
// breakpoint manager ("BreakpointManager"), the debugger's user interface
// thread and windows. Declarations of the Motion Factory runtime (mfutil.dll,
// PiccoloRT.dll, PiccoloConsole.dll) as far as this plugin uses them; names
// and signatures from the decorated import names, layouts from the code.
#ifndef PSCDEBUGGER_H
#define PSCDEBUGGER_H

// The DLL imports MFC42.DLL; options.txt does not define _AFXDLL for it.
#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include <string.h>
#include <stdlib.h>

// ---- mfutil.dll -------------------------------------------------------------

class MfCallBack;
class MfListItemInterface;
class MfStringTokenizerCallBack;

#include "../../mfutil/MfNullString.h"

// Reference-counted string body: vptr, data, length, allocated size, count.
// own copy: this plugin imports the constructor MfStringRef(int)
// (??0MfStringRef@@QAE@H@Z); mfutil/MfStringRef.h has it inline.
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int n);
    virtual ~MfStringRef() { delete pData; }
    MfStringRef *CopyOnWrite();
    void StripWhiteSpace();
    void Truncate(int n);
    char *pData;
    int nLength;
    int nAlloc;
    int nRefs;
};

#include "../../mfutil/MfStringSize.h"

// own copy: this plugin imports MfString::CopyOnWrite(int) and the MfStringRef
// constructor (StripWhiteSpace, Truncate); mfutil/MfString.h has both inline.
class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const MfStringSize &);
    MfString(const MfString &);
    MfString(const MfNullString &);
    MfString(const char *);
    MfString(int);
    ~MfString();
    MfString &operator=(const MfString &);
    MfString &operator=(const MfNullString &);
    MfString &operator=(const char *);
    MfString &operator<<(const MfString &);
    MfString &operator<<(const char *);
    MfString &operator<<(int);
    MfString operator+(const MfString &) const;
    int operator==(const MfString &) const;
    int operator==(const char *) const;
    int operator!=(const MfString &) const;
    int operator!=(const MfNullString &) const;
    operator const char *() const;
    operator int() const { return atoi(pRef->pData); }
    int Length() const { return pRef->nLength; }
    void StripWhiteSpace()
    {
        MakeUnique();
        pRef->StripWhiteSpace();
    }
    void Truncate(int n)
    {
        CopyOnWrite(0);
        pRef->Truncate(n);
    }
    MfString MidString(int start, int count) const;
    int CaseCompare(const MfString &) const;
    int __cdecl Format(char *const format, ...);
    void Tokenize(const MfString &, MfStringTokenizerCallBack &) const;
private:
    void MakeUnique()
    {
        if (pRef == &mfsrNullInitializer) {
            pRef = new MfStringRef(0);
            pRef->nRefs++;
        } else if (pRef->nRefs > 1) {
            pRef->nRefs--;
            pRef = pRef->CopyOnWrite();
            pRef->nRefs++;
        }
    }
    void CopyOnWrite(int);
    MfStringRef *pRef;
    static MfStringRef mfsrNullInitializer;
};

#include "../../mfutil/MfDblListItem.h"

// own copy: mfutil/MfDblList.h imports the class (and its vtable) and has
// GetFirst/GetNext/GetCurrent inline; this plugin imports those three and
// emits the vtable itself. The class is not imported (its vtable is emitted
// here), its members are.
// VC5 lays overloaded virtuals out in reverse order of declaration.
class MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    MfDblList() : pliFirst(0), pliLast(0), nCount(0) {}
    __declspec(dllimport) virtual ~MfDblList();
    __declspec(dllimport) virtual void Prepend(void *const pData);
    __declspec(dllimport) virtual void Append(void *const pData);
    __declspec(dllimport) virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli);
    __declspec(dllimport) virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *pli);
    __declspec(dllimport) virtual void Add(void *const pData);
    __declspec(dllimport) virtual void Copy(const MfDblList &list);
    __declspec(dllimport) virtual void Copy(const MfDblList &list, MfCallBack &cb);
    __declspec(dllimport) virtual void Copy(const MfDblList &list, MfListItemInterface &lii);
    __declspec(dllimport) virtual void *GetFirst(MfDblListItem *&pli) const;
    __declspec(dllimport) virtual void *GetNext(MfDblListItem *&pli) const;
    __declspec(dllimport) virtual void *GetCurrent(MfDblListItem *&pli) const;
    __declspec(dllimport) virtual void *GetPrev(MfDblListItem *&pli) const;
    __declspec(dllimport) virtual void *GetLast(MfDblListItem *&pli) const;
    __declspec(dllimport) virtual void Iterate(MfCallBack &cb) const;
    __declspec(dllimport) virtual void Iterate(MfListItemInterface &lii) const;
    __declspec(dllimport) virtual void *DeleteFirst();
    __declspec(dllimport) virtual void *DeleteLast();
    __declspec(dllimport) virtual void *Delete(MfDblListItem *&pli);
    __declspec(dllimport) virtual void Clear();
    __declspec(dllimport) virtual void Clear(MfCallBack &cb);
    __declspec(dllimport) virtual void Clear(MfListItemInterface &lii);
    __declspec(dllimport) virtual MfDblListItem *Find(void *pData) const;
    __declspec(dllimport) virtual MfDblListItem *Find(void *pData, MfCallBack &cb) const;
    __declspec(dllimport) virtual MfDblListItem *Find(void *pData, MfListItemInterface &lii) const;
    __declspec(dllimport) virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n) const;
    __declspec(dllimport) virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfCallBack &cb) const;
    __declspec(dllimport) virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfListItemInterface &lii) const;
    int GetCount() const { return nCount; }

protected:
    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};

// Tag type for the constructor that builds an object only to pass its
// MfListItemInterface.
// own copy: 4 bytes here (~MfPdbBreakpointManager copies it as a dword);
// PiccoloRT/LiiExemplar.h declares it without data.
class LiiExemplar {
public:
    ~LiiExemplar() {}
private:
    int mUnused;
};

// How MfDblList copies, visits, destroys and compares its items.
// own copy: the plugin's classes override only some slots and their vtables
// hold these default bodies; mfutil/MfListItemInterface.h has the four slots
// pure.
class MfListItemInterface {
public:
    virtual void *Copy(void *item) { return 0; }
    virtual int Iterate(void *item) { return 1; }
    virtual void Destroy(void *item) {}
    virtual int Matches(void *item, void *key) { return 0; }
};

#include "../../mfutil/MfCallBack.h"
#include "../../mfutil/MfStringTokenizerCallBack.h"

// List of MfString (0x14 bytes); constructor and destructor are inline.
// own copy: derived from this plugin's own MfDblList (mfutil/MfStringList.h
// includes mfutil/MfDblList.h).
class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList() { Clear(); }
    virtual MfString *Add(const MfString &s);
    virtual void Clear();
    void Sort(int);
    MfString *First(MfDblListItem *&pos) const
    {
        pos = pliFirst;
        return pos ? (MfString *)pos->pData : 0;
    }
    MfString *Next(MfDblListItem *&pos) const
    {
        if (pos)
            pos = pos->pliNext;
        return pos ? (MfString *)pos->pData : 0;
    }
};

#include "../../mfutil/MfHashTable.h"
#include "../../mfutil/MfIndex.h"

// Registry access. Constructor and destructor are empty inlines.
// own copy: mfutil/UtilityApp.h befriends DllMain with C++ linkage; this
// plugin's DllMain (MFC extension DLL) is extern "C".
class __declspec(dllimport) UtilityApp {
public:
    UtilityApp() {}
    ~UtilityApp() {}
    int GetProfileIntA(const MfString &section, const MfString &entry, int def);
    MfString GetProfileStringA(const MfString &section, const MfString &entry, const MfString &def);
    int WriteProfileInt(const MfString &section, const MfString &entry, int value);
    int WriteProfileStringA(const MfString &section, const MfString &entry, const MfString &value);
};

// ---- PiccoloRT.dll ----------------------------------------------------------

class MfScData;
class MfScDataHandle;
class MfScDebugInfo;
class MfAiPdbDebugger;
class MfPeMetaCustomToolbar;
class MfScVirtualMachine;

// MfAi: PiccoloRT/MfAiFunctionRegistry.h still declares its own (it cannot be
// included together with mfutil/MfAi.h).
#include "../../PiccoloRT/MfAiFunctionRegistry.h"

// A reference-counted, typed value of the script VM (8 bytes).
// own copy: PiccoloRT/MfScDataHandle.h includes PiccoloRT's own
// MfCriticalSection copy, which conflicts with mfutil/MfCriticalSection.h.
class __declspec(dllimport) MfScDataHandle {
public:
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
    unsigned short AddRef() { return ++m_refCount; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }
    int IsType(unsigned long type) const { return m_flags & type; }
    int GetInt() const { return (int)m_data; }
    void InfoShort(MfString *info);
private:
    unsigned short m_refCount;  // +0
    unsigned short m_flags;     // +2
    MfScData *m_data;           // +4
};

class MfScLinkData;
class MfStringList;

#include "../../PiccoloRT/MfScData.h"

// A script object: properties by name.
// own copy: PiccoloRT/MfScDataObject.h includes PiccoloRT's own MfString copy.
class __declspec(dllimport) MfScDataObject : public MfScData {
public:
    virtual ~MfScDataObject();
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual int SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int);
    virtual MfString GetClassName();
    virtual void GetPropertyNames(MfStringList *names, int);
    virtual void GetInstanceProperties(MfStringList *names, int);
};

// own copy: PiccoloRT/MfScDataArray.h includes PiccoloRT/MfScDataObject.h.
class __declspec(dllimport) MfScDataArray : public MfScDataObject {
public:
    virtual ~MfScDataArray();
    virtual void GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual void GetPropertyExtensions();
    virtual int SetAt(int index, MfScDataHandle *value, int noAddRef) = 0;
    virtual int GetAt(int index, MfScDataHandle **value, int addRef) = 0;
private:
    int m_0c[2];
public:
    int mCount;         // +0x14
};

// own copy: PiccoloRT/MfScDataMethod.h includes PiccoloRT/MfScDataHandle.h.
class __declspec(dllimport) MfScDataMethod : public MfScData {
public:
    virtual ~MfScDataMethod();
    int m_type;                     // +0x0c
    MfScDataHandle **m_constants;   // +0x10
    int m_numConstants;             // +0x14
    int m_numArgs;                  // +0x18
};

// A routine compiled to Piccolo bytecode (0x3c bytes).
// own copy: PiccoloRT/MfScBytecodeRoutine.h includes PiccoloRT/MfScDataMethod.h.
class __declspec(dllimport) MfScBytecodeRoutine : public MfScDataMethod {
public:
    virtual ~MfScBytecodeRoutine();
    MfScDebugInfo *DebugInfo() const { return mDebugInfo; }
    MfString &ModuleName() { return *mModuleName; }
    unsigned char *mBytecodes;      // +0x1c
    int mCodeSize;                  // +0x20
    int mFlags;                     // +0x24
    MfString *mModuleName;          // +0x28
    MfString *mModuleType;          // +0x2c
    MfString *mObjectFileName;      // +0x30
    MfScDebugInfo *mDebugInfo;      // +0x34
    MfString *mFunctionParams;      // +0x38
};

#include "../../PiccoloRT/MfScDebugInfo.h"

// own copy: the plugin reads the private aiBytecodeSize through an inline
// Size(); PiccoloRT/MfScBytecodes.h has no such inline (and is not dllimport).
class __declspec(dllimport) MfScBytecodes {
public:
    static int __cdecl IsJump(unsigned char);
    static int __cdecl IsJumpNotZero(unsigned char);
    static int __cdecl IsJumpZero(unsigned char);
    static int __cdecl IsUnconditionalJump(unsigned char);
    static int __cdecl JumpOffset(unsigned char *);
    static int Size(unsigned char bytecode) { return aiBytecodeSize[bytecode]; }
private:
    static int aiBytecodeSize[];
};

// Bytecodes the debugger plants or looks for.
enum {
    BC_INVOKEFUNC = 0x30,
    BC_INVOKEMETH = 0x31,
    BC_4C = 0x4c,           // never replaced by a breakpoint
    BC_BREAKPOINT = 0xfa
};

#include "../../PiccoloRT/MfScVmFrame.h"

// High-resolution timer (mfutil.dll).
// own copy: Start, Stop and AccumTimer are inline here (the plugin imports
// none of them) and MfPdbVmListDlg reads liFrequency; mfutil/MfHPTimer.h
// declares them out of line.
class __declspec(dllimport) MfHPTimer {
    friend class MfPdbVmListDlg;
public:
    void Start() { QueryPerformanceCounter(&m_start); }
    void Stop() { QueryPerformanceCounter(&m_stop); }
    void AccumTimer(__int64 &acc) { acc += m_stop.QuadPart - m_start.QuadPart; }
    LARGE_INTEGER m_start;          // +0x00
    LARGE_INTEGER m_stop;           // +0x08
private:
    static LARGE_INTEGER liFrequency;
};

#include "../../PiccoloRT/MfScVariableStack.h"

// A script thread's virtual machine state.
// own copy: PiccoloRT/MfScVmEnv.h includes PiccoloRT's own MfString and
// MfHPTimer copies.
// Packed to 4: the vptr is followed directly by m_4 although the class holds
// 64-bit timer fields (offsets from the code: m_stop +0x50, m_timer +0x60).
#pragma pack(push, 4)
class __declspec(dllimport) MfScVmEnv {
public:
    virtual ~MfScVmEnv();
    int FirstFrame(MfScVmFrame *frame) const;
    int NextFrame(MfScVmFrame *frame) const;
    void RestoreFrame(MfScVmFrame *frame);
    MfScVariableStack::MfScVariableErrorCode GetVariable(const MfString &name, MfScDataHandle **value) const;
    int m_4;
    unsigned char *m_ip;            // +0x08
    MfScDataHandle **m_sp;          // +0x0c
    unsigned long *m_frame;         // +0x10
    unsigned long m_args;           // +0x14
    MfScDataHandle *m_object;       // +0x18
    MfScDataHandle *m_function;     // +0x1c
private:
    int m_20[12];
public:
    int m_stop;                     // +0x50 stop executing the thread
    int m_running;                  // +0x54 the thread is executing
    int m_id;                       // +0x58
private:
    int m_5c;
public:
    MfHPTimer m_timer;              // +0x60 since the thread last started running
    __int64 m_processTime;          // +0x70 accumulated running time
};
#pragma pack(pop)

#include "../../PiccoloRT/MfAiVmManager.h"

// own copy: PiccoloRT/MfScFunctionRegistryEntry.h includes PiccoloRT's own
// MfListItemInterface copy.
class __declspec(dllimport) MfScFunctionRegistryEntry : public MfListItemInterface {
public:
    MfScFunctionRegistryEntry(MfScDataHandle *function);
    virtual void Destroy(void *item);
    virtual ~MfScFunctionRegistryEntry();
    MfScDataHandle *mData;          // +0x04
};

// The functions of one module, by name.
// own copy: PiccoloRT/MfScFunctionRegistryModule.h includes PiccoloRT's own
// MfListItemInterface and MfString copies.
class __declspec(dllimport) MfScFunctionRegistryModule : public MfListItemInterface {
public:
    MfScFunctionRegistryModule();
    virtual void Destroy(void *item);
    virtual ~MfScFunctionRegistryModule();
    virtual void AddFunction(MfScDataHandle *function);
    virtual void RemoveFunction(MfScDataHandle *function);
    MfScFunctionRegistryEntry *FindFunction(const MfString &name, MfDblListItem **pos);
    MfString mModule;               // +0x04
    MfString mModuleType;           // +0x08
    MfDblList *mFunctions;          // +0x0c
};

// The lock of the runtime's tables.
#include "../../mfutil/MfCriticalSection.h"

class MfScHandleList;

#include "../../PiccoloRT/MfCbRegistryFunctionEnumerator.h"
#include "../../PiccoloRT/MfCbRegistryModuleEnumerator.h"
#include "../../PiccoloRT/MfAiFunctionRegistry.h"

// A list of script function handles with an internal position.
// own copy: PiccoloRT/MfScHandleList.h includes PiccoloRT's own MfDblList copy.
class __declspec(dllimport) MfScHandleList : public MfDblList {
public:
    MfScHandleList() : mPos(0) {}
    virtual ~MfScHandleList();
    MfScDataHandle *First()
    {
        mPos = pliFirst;
        MfScDataHandle *handle = mPos ? (MfScDataHandle *)mPos->pData : 0;
        return handle;
    }
    MfScDataHandle *Next()
    {
        if (mPos)
            mPos = mPos->pliNext;
        MfScDataHandle *handle = mPos ? (MfScDataHandle *)mPos->pData : 0;
        return handle;
    }
private:
    MfDblListItem *mPos;    // +0x10
};

// own copy: PiccoloRT/MfScFunctionRegistry.h includes PiccoloRT's own
// MfCriticalSection copy.
class __declspec(dllimport) MfScFunctionRegistry : public MfAiFunctionRegistry {
public:
    MfScFunctionRegistry();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int Add(MfScDataHandle *function);
    virtual int Remove(MfScDataHandle *function);
    virtual int Find(const MfString &module, const MfString &name, MfScDataHandle **function);
    virtual int Find(const MfString &module, int line, MfScDataHandle **function);
    virtual int Find(const MfString &module, MfScHandleList *functions);
    virtual void EnumFunctions(MfCbRegistryFunctionEnumerator *enumerator, void *data);
    virtual void EnumModules(MfCbRegistryModuleEnumerator *enumerator, void *data);
    virtual int Clear();
    virtual int CheckOrphans();
    virtual ~MfScFunctionRegistry();
    MfScFunctionRegistryEntry *Find(const MfString &module, const MfString &name);
    MfScFunctionRegistryEntry *Find(const MfString &module, int line);
protected:
    virtual MfScFunctionRegistryModule *FindModule(const MfString &module, MfDblListItem **pos);
    void Lock() { mLock->Enter(); }
    void Unlock() { mLock->Leave(); }
    MfDblList *mModules;            // +0x04 MfScFunctionRegistryModule *
    MfCriticalSection *mLock;       // +0x08
    int mRefCount;                  // +0x0c
};

// Glyphs in the editor margin.
enum { kGlyphBreakpoint = 0, kGlyphDisabledBreakpoint = 1 };

// ---- this plugin: breakpoints -------------------------------------------------

class MfPdbBreakpoint;
class MfPdbSetBreakpoint;

// A function of the registry with the breakpoints planted in it (0xc bytes).
class MfPdbFunctionRegistryEntry : public MfScFunctionRegistryEntry {
public:
    MfPdbFunctionRegistryEntry(MfScDataHandle *function);
    virtual ~MfPdbFunctionRegistryEntry();
    MfPdbBreakpoint *Plant(MfPdbSetBreakpoint *set);
    MfPdbBreakpoint *PlantLine(int line);
    unsigned char *FindPlanted(MfPdbSetBreakpoint *set);
    void AddBreakpoint(MfPdbBreakpoint *bp);
    void RemoveBreakpoint(MfPdbBreakpoint *bp);

    MfDblList *mBreakpoints;        // +0x08 MfPdbBreakpoint *
};

// A breakpoint planted in the bytecode: the bytecode it replaced (0x14 bytes).
class MfPdbBreakpoint : public MfListItemInterface {
public:
    MfPdbBreakpoint(unsigned char *address, unsigned long data);
    virtual ~MfPdbBreakpoint();
    virtual void Destroy(void *item) { delete (MfPdbBreakpoint *)item; }
    void SetEntry(MfPdbFunctionRegistryEntry *entry);
    void ReleaseEntry(int entryGone);
    void Restore()
    {
        if (mAddress) {
            *mAddress = mBytecode;
            mAddress = 0;
            mBytecode = 0;
        }
    }

    unsigned char *mAddress;                // +0x04
    unsigned char mBytecode;                // +0x08
    unsigned long mData;                    // +0x0c flags, or the address to plant again
    MfPdbFunctionRegistryEntry *mEntry;     // +0x10
};

// A breakpoint the user set on a line of a module (0x10 bytes).
class MfPdbSetBreakpoint : public MfListItemInterface {
public:
    MfPdbSetBreakpoint(LiiExemplar) {}
    MfPdbSetBreakpoint(const MfString &module, int line, int enabled)
        : mModule(module), mLine(line), mEnabled(enabled) {}
    virtual ~MfPdbSetBreakpoint() {}
    virtual void Destroy(void *item) { delete (MfPdbSetBreakpoint *)item; }
    void ShowGlyph(int remove);

    MfString mModule;       // +0x04
    int mLine;              // +0x08
    int mEnabled;           // +0x0c
};

#include "../../PiccoloRT/MfPdbStepContext.h"

// Flags of the breakpoints planted for stepping.
enum {
    kStepJump = 0x1,
    kStepTarget = 0x2,
    kStepInvoke = 0x4,
    kStepOver = 0x1000,
    kStepInto = 0x2000,
    kStepModes = 0x3000,
    kStepPlanted = 0x10000,     // a breakpoint the user set
    kStepTemporary = 0x20000
};

#include "../../PiccoloRT/MfPdbBreakpointInfo.h"
#include "../../PiccoloRT/MfAiPdbBreakpointManager.h"

// The breakpoint manager (0x14 bytes).
class MfPdbBreakpointManager : public MfAiPdbBreakpointManager {
public:
    MfPdbBreakpointManager();
    virtual ~MfPdbBreakpointManager();

    // MfAi
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef() { return ++mRefs; }
    virtual int Release()
    {
        if (--mRefs == 0) {
            delete this;
            return 0;
        }
        return mRefs;
    }
    // MfAiPdbBreakpointManager
    virtual int AddBreakpoint(const MfString &module, int line);
    virtual int RemoveBreakpoint(const MfString &module, int line);
    virtual int ToggleBreakpoint(const MfString &module, int line);
    virtual int EnableBreakpoint(const MfString &module, int line);
    virtual int DisableBreakpoint(const MfString &module, int line);
    virtual int ToggleEnabled(const MfString &module, int line);
    virtual void RemoveAll();
    virtual void RemoveModule(const MfString &module);
    virtual void DisableAll();
    virtual void DisableModule(const MfString &module);
    virtual int PlantAll();
    virtual int UnplantAll();
    virtual int BreakpointHit(unsigned long address, int, MfScVmEnv *env);
    virtual int GetBreakpointInfo(unsigned long address, int, MfPdbBreakpointInfo *info);
    virtual int PlantModule(const MfString &module);
    virtual int Save() { return 0; }

    MfPdbSetBreakpoint *FindSetBreakpoint(const MfString &module, int line, MfDblListItem **where);
    int DeleteSetBreakpoint(MfPdbSetBreakpoint *set);
    void ToggleEnabled(MfPdbSetBreakpoint *set);
    void Plant(MfPdbSetBreakpoint *set);
    void Unplant(MfPdbSetBreakpoint *set);
    MfPdbBreakpoint *FindBreakpoint(unsigned long address);
    void DeleteBreakpoint(unsigned long address);
    void MoveSetBreakpoint(MfPdbSetBreakpoint *set, int line);
    MfPdbBreakpoint *PlantStep(MfPdbStepContext *ctx, unsigned long flags, int atNext);
    MfPdbBreakpoint *PlantStepInto(MfPdbStepContext *ctx);
    MfPdbBreakpoint *PlantJump(MfPdbStepContext *ctx, unsigned long flags);
    MfPdbBreakpoint *PlantLine(MfPdbStepContext *ctx, const MfString &module, int line);
    MfPdbBreakpoint *PlantReturn(MfPdbStepContext *ctx);
    MfPdbBreakpoint *PlantNext(int line, int next, MfPdbStepContext *ctx);
    void FillListBox(CListBox *list);
    void SaveBreakpoints();
    void LoadBreakpoints();

private:
    int mRefs;                      // +0x04
    MfDblList *mSetBreakpoints;     // +0x08 MfPdbSetBreakpoint *
    MfDblList *mBreakpoints;        // +0x0c MfPdbBreakpoint *
    MfIndex *mPlanted;              // +0x10 address -> MfPdbBreakpoint *
};

// ---- mfutil.dll: threads and events ---------------------------------------------

// A Win32 auto-reset event.
#include "../../mfutil/MfEvent.h"
#include "../../mfutil/MfThreadWork.h"
#include "../../mfutil/MfThread.h"

// own copy: mfutil/MfInterfaceRegistry.h includes mfutil/MfString.h.
class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int GetInterface(const MfString &name, MfAi **ai);
};

// "ApplicationI".
// own copy: RuntimeLib/MfAiApplication.h includes RuntimeLib/MfAi.h, which
// conflicts with the MfAi of PiccoloRT/MfAiFunctionRegistry.h.
class MfAiApplication : public MfAi {
public:
    virtual int GetApplicationType() = 0;
};

// ---- PiccoloRT.dll: the source of modules ----------------------------------------

// The source of one kind of module (by module type).
// own copy: PiccoloRT/MfAiSourceType.h has five other slots (GetType,
// ReadSource(.., int flags), WriteSource, SplitPath, IsSameModule); this
// plugin passes an MfScVmEnv * where it has the flags.
class MfAiSourceType : public MfAi {
public:
    virtual int GetModuleNames(MfStringList *names) = 0;
    virtual int LoadSource(const MfString &module, MfString *source, MfScVmEnv *env) = 0;
    virtual int SaveSource(const MfString &module, const MfString &source, MfScVmEnv *env) = 0;
    virtual int SplitModuleName(const MfString &module, MfStringList *parts) = 0;
};

// own copy: PiccoloRT/MfAiSourceResolver.h includes PiccoloRT/MfAiSourceType.h.
class MfAiSourceResolver : public MfAi {
public:
    virtual int Register(const MfString &type, MfAiSourceType *sourceType) = 0;
    virtual int Unregister(const MfString &type) = 0;
    virtual int Find(const MfString &type, MfAiSourceType **sourceType) = 0;
};

// own copy: MfPdbDebugger reads the private bIsRunning; PiccoloRT/MfScPiccolo.h
// does not befriend it and includes PiccoloRT/MfScDataObject.h.
class __declspec(dllimport) MfScPiccolo {
    friend class MfPdbDebugger;
public:
    static MfAiSourceResolver *GetSourceResolver();
private:
    static int bIsRunning;
};

// own copy: PiccoloRT/MfScVirtualMachine.h includes PiccoloRT's own
// MfCriticalSection copy.
class __declspec(dllimport) MfScVirtualMachine {
public:
    void FormatException(int code, const MfString &message, MfStringList *lines);
};

extern "C" __declspec(dllexport) int __stdcall PscInitialize(int, MfString *errors);
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *errors);

// own copy: PscInitialize and PscShutdown set the private pdbgTheDebugger;
// PiccoloRT/MfScDebugger.h does not befriend them.
class __declspec(dllimport) MfScDebugger {
    friend int __stdcall PscInitialize(int, MfString *errors);
    friend int __stdcall PscShutdown(MfString *errors);
private:
    static MfAiPdbDebugger *pdbgTheDebugger;
};

// ---- PiccoloConsole.dll: editors and toolbars -----------------------------------
// own copies: PiccoloConsole/PiccoloConsole.h declares its own mfutil classes
// and exports MfPeCodeEditor and CConsoleEdit with a hard-coded dllexport.

class CConsoleEdit;
class CConsoleOutput;
class MfAiPiccoloEditor;

class __declspec(dllimport) CConsoleOutput : public CTabCtrl {
public:
    void SwitchToWindow(int window);
    int InstallUserWnd(const MfString &name, CWnd *wnd);
};

class MfPeKbdHandler {
public:
    virtual int KbhKeyDown(int key, int flags, MfAiPiccoloEditor *editor) = 0;
};

// The code editor window.
class __declspec(dllimport) MfPeCodeEditor : public CWnd {
public:
    static void __cdecl ClearGlobalGlyphs(int glyph);
    static void __cdecl AddGlyphAtLineForModule(const MfString &module, int line, int glyph);
    static void __cdecl RemoveGlyphAtLineForModule(const MfString &module, int line, int glyph);
    static void __cdecl InstallGlobalKeyboardHandler(MfPeKbdHandler *handler);
    static void __cdecl UninstallGlobalKeyboardHandler(MfPeKbdHandler *handler);
    void InstallKeyboardHandler(MfPeKbdHandler *handler);
    void UninstallKeyboardHandler(MfPeKbdHandler *handler);
    MfAiPiccoloEditor *GetIEditor();
    void SetGlyphOnLine(int line, int glyph);
    void UnsetGlyphOnLine(int line, int glyph);
    void GotoLine(int line, int);
    void DisableSyntaxHighlight();
protected:
    virtual BOOL PreTranslateMessage(MSG *msg);
private:
    int mUnknown[3];
public:
    int mEditable;                  // +0x4c the user may type
private:
    int mUnknown2[24];              // up to 0xb0 bytes
};

class __declspec(dllimport) CConsoleEdit : public MfPeCodeEditor {
    DECLARE_DYNAMIC(CConsoleEdit)
public:
    CConsoleEdit();
    virtual ~CConsoleEdit();
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    DECLARE_MESSAGE_MAP()
public:
    int IsModified();
    void SetUnModified();
    MfString GetEntireContents();
    void SetEntireContents(const MfString &text, int);
};

// The window of an editor: the code editor and the output windows. Only the
// members the debugger reads are named.
class __declspec(dllimport) CPiccoloWnd : public CWnd {
public:
    static void __cdecl ClearDisplayedGlyphs(int glyph);
    static void __cdecl SetGlyphOnLineForModule(const MfString &module, int line, int glyph);
    static void __cdecl UnsetGlyphOnLineForModule(const MfString &module, int line, int glyph);
    static void __cdecl AddMetaToolbar(MfPeMetaCustomToolbar *toolbar);
    static int __cdecl RemoveMetaToolbar(const MfString &name, MfDblListItem **where);
    int GetPaneSize();
private:
    int mUnknown[45];
public:
    CConsoleEdit *mEditor;          // +0xf4
    CConsoleOutput *mOutput;        // +0xf8
};

class MfPeBuffModCallBack {
public:
    virtual void BmcbNotify(int) = 0;
};

// A frame window editing one file (0xe4 bytes).
class __declspec(dllimport) CPiccoloFileEditor : public CFrameWnd, public MfPeBuffModCallBack {
public:
    CPiccoloFileEditor(const MfString &fileName, const MfString &baseTitle);
    virtual ~CPiccoloFileEditor();
    virtual void ReadCurrentFile(MfString *text);
    virtual void WriteCurrentFile(MfString *text);
    virtual void CreateFrameWindow(CWnd *parent);
    CConsoleEdit *EditorWnd();
    void SetFileName(const MfString &fileName);
    void OutputClear(int window);
protected:
    virtual void BmcbNotify(int);
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnSize(UINT type, int cx, int cy);
    void StartUpdateTimer();
    void StopUpdateTimer();
    DECLARE_MESSAGE_MAP()
    int mModified;                  // +0xc4
    int mTextModified;              // +0xc8
public:
    CPiccoloWnd *mPiccoloWnd;       // +0xcc
protected:
    int mEditorHints;               // +0xd0
    int mOutputHints;               // +0xd4
public:
    MfString mFileName;             // +0xd8
protected:
    MfString mBaseTitle;            // +0xdc
    int mTimer;                     // +0xe0
};

// What a toolbar button calls back into.
class MfAiToolbarHandlers : public MfAi {
public:
    virtual int HandleCommand(int toolbar, int id, MfAiPiccoloEditor *editor) = 0;
    virtual int HandleUpdate(int toolbar, int id, CCmdUI *cmdUI) = 0;
};

class MfAiToolbarBitmapLoader : public MfAi {
public:
    virtual BOOL LoadBitmap(CToolBar *toolbar) = 0;
};

class __declspec(dllimport) MfPeMetaCustomToolbarEntry : public MfListItemInterface {
public:
    MfPeMetaCustomToolbarEntry(int type) : mType(type) {}
    virtual void Destroy(void *item);
    virtual ~MfPeMetaCustomToolbarEntry();
protected:
    int mType;          // +0x04
};

class __declspec(dllimport) MfPeMetaToolbarEntrySeparator : public MfPeMetaCustomToolbarEntry {
public:
    MfPeMetaToolbarEntrySeparator() : MfPeMetaCustomToolbarEntry(1) {}
    virtual ~MfPeMetaToolbarEntrySeparator();
};

class __declspec(dllimport) MfPeMetaToolbarEntryItem : public MfPeMetaCustomToolbarEntry {
public:
    MfPeMetaToolbarEntryItem(int id, const MfString &toolTip, MfAiToolbarHandlers *handlers);
    virtual ~MfPeMetaToolbarEntryItem();
private:
    MfString mToolTip;
    int mID;
    MfAiToolbarHandlers *mHandlers;
};

class __declspec(dllimport) MfPeMetaCustomToolbar : public MfListItemInterface {
public:
    MfPeMetaCustomToolbar(const MfString &name, int id, MfAiToolbarBitmapLoader *loader);
    virtual void Destroy(void *item);
    virtual ~MfPeMetaCustomToolbar();
    void Add(MfPeMetaCustomToolbarEntry *entry);
    MfPeMetaCustomToolbar &operator+(MfPeMetaCustomToolbarEntry *entry)
    {
        Add(entry);
        return *this;
    }
private:
    MfString mName;
    int mID;
    MfAiToolbarBitmapLoader *mLoader;
    MfDblList *mEntries;
};

// The editor interface a toolbar command gets.
class MfAiPiccoloEditor : public MfAi {
public:
    virtual int GetInterface(int id, MfAi **ai) = 0;
    virtual void GetPosition(int *column, int *line) = 0;
    virtual void GetModuleName(MfString *module) = 0;
    virtual void GetSelection(MfString *text) = 0;
    virtual void GetWordAtCursor(MfString *word, int) = 0;
};

// The windows of an editor (interface 1 of MfAiPiccoloEditor::GetInterface).
class MfAiPeWindows : public MfAi {
public:
    virtual int GetMainWindow(CWnd **wnd) = 0;
};

// ---- this plugin: the debugger ---------------------------------------------------

// The ids of the debugger's commands (WM_COMMAND to the debugger frame,
// routed to MfPdbDebugger).
enum {
    ID_PDB_GO = 0xafc8,
    ID_PDB_STOP = 0xafc9,
    ID_PDB_STEP_OVER = 0xafca,
    ID_PDB_STEP_OUT = 0xafcb,
    ID_PDB_RUN_TO_CURSOR = 0xafcc,
    ID_PDB_STEP_INTO = 0xafcd,
    ID_PDB_EDIT_BREAKPOINTS = 0xafce,
    ID_PDB_SELECT_MODULE = 0xafcf,
    ID_PDB_SAVE = 0xafd0,
    ID_PDB_VIEW_VARIABLE = 0xafd1,
    ID_PDB_VM_LIST = 0xafd2,
    ID_PDB_SHOW_BREAKPOINT = 0xaffa,
    ID_PDB_SHOW_EXCEPTION = 0xaffb,
    ID_PDB_SHOW_MESSAGE = 0xaffc
};

// The buttons of the debugger's toolbar in the editors.
enum {
    kToolbarBreakpoint = 100,
    kButtonToggle = 100,
    kButtonEnable = 101,
    kButtonDisableAll = 102,
    kButtonDisableModule = 103,
    kButtonClearAll = 104,
    kButtonClearModule = 105,
    kButtonEdit = 106
};

// What the debugger tells its listeners.
enum {
    kPdbStarted = 0,
    kPdbStopped = 1,
    kPdbException = 2,
    kPdbExceptionDone = 3,
    kPdbBreak = 4,
    kPdbContinue = 5,
    kPdbTerminate = 6
};

// What the debugger thread is showing for the script thread.
enum {
    kPdbIdle = 0,
    kPdbAtBreakpoint = 1,
    kPdbAtException = 2,
    kPdbMessage = 3
};

class MfPdbListener {
public:
    virtual void Notify(int event) = 0;
};

#include "../../PiccoloRT/MfAiPdbDebugger.h"

// Loads the bitmap of the debugger's toolbar.
class MfPdbToolbarBitmapLoader : public MfAiToolbarBitmapLoader {
public:
    MfPdbToolbarBitmapLoader(int bitmap) : mRefs(1), mBitmap(bitmap) {}
    ~MfPdbToolbarBitmapLoader() {}
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef() { return ++mRefs; }
    virtual int Release();
    virtual BOOL LoadBitmap(CToolBar *toolbar);
private:
    int mRefs;          // +0x04
    int mBitmap;        // +0x08
};

extern AFX_EXTENSION_MODULE PscDebuggerDLL;

// Makes the plugin's resources current while it lives.
class PscDebuggerContext {
public:
    PscDebuggerContext()
    {
        mOldResources = AfxGetResourceHandle();
        AfxSetResourceHandle(PscDebuggerDLL.hResource);
    }
    virtual ~PscDebuggerContext() { AfxSetResourceHandle(mOldResources); }
private:
    HINSTANCE mOldResources;
};

class MfPdbDebuggerFrame;

// A message box the debugger thread shows for the script thread.
struct MfPdbMessageBox {
    HWND mOwner;            // +0x00
    MfString mText;         // +0x04
    MfString mCaption;      // +0x08
    UINT mType;             // +0x0c
    int mResult;            // +0x10
};

// The debugger (0x90 bytes). The script threads call it through
// MfAiPdbDebugger; its own thread (DoWork) shows the debugger window.
class MfPdbDebugger : public CCmdTarget, public MfAiPdbDebugger, public MfAiToolbarHandlers,
                      public MfThreadWork, public MfPeKbdHandler {
public:
    MfPdbDebugger();
    virtual ~MfPdbDebugger();

    // MfAi
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    // MfAiPdbDebugger
    virtual int SignalBreakpoint(MfPdbStepContext *ctx, unsigned char *bytecode);
    virtual int SignalException(int code, const MfString &message, MfPdbStepContext *ctx);
    virtual void Initialize();
    virtual void Shutdown();
    virtual int IsActive();
    virtual void ProgramState(int state);
    virtual int GetInterface(int id, MfAi **ai);
    virtual void AddListener(MfPdbListener *listener);
    virtual void RemoveListener(MfPdbListener *listener);
    // MfAiToolbarHandlers
    virtual int HandleCommand(int toolbar, int id, MfAiPiccoloEditor *editor);
    virtual int HandleUpdate(int toolbar, int id, CCmdUI *cmdUI);
    // MfThreadWork
    virtual void DoWork();
    // MfPeKbdHandler
    virtual int KbhKeyDown(int key, int flags, MfAiPiccoloEditor *editor);

    void Notify(int event);
    void Continue();
    int GetVariable(const MfString &name, MfScDataHandle **value);
    int AskUser(const MfString &text, const MfString &caption, UINT type);

protected:
    afx_msg void OnShowBreakpoint();
    afx_msg void OnShowException();
    afx_msg void OnShowMessage();
    afx_msg void OnGo();
    afx_msg void OnStop();
    afx_msg void OnStepOver();
    afx_msg void OnStepInto();
    afx_msg void OnUpdateStopped(CCmdUI *cmdUI);
    afx_msg void OnRunToCursor();
    afx_msg void OnStepOut();
    afx_msg void OnEditBreakpoints();
    afx_msg void OnViewVariable();
    afx_msg void OnSelectModule();
    afx_msg void OnUpdateActive(CCmdUI *cmdUI);
    afx_msg void OnSave();
    afx_msg void OnUpdateSave(CCmdUI *cmdUI);
    afx_msg void OnVmList();
    DECLARE_MESSAGE_MAP()

public:
    int mRefs;                              // +0x30
    MfPdbDebuggerFrame *mFrame;             // +0x34
    MfThread *mThread;                      // +0x38
    int mActive;                            // +0x3c
    MfScFunctionRegistry *mRegistry;        // +0x40
    MfPdbBreakpointManager *mBreakpoints;   // +0x44
    MfDblList *mListeners;                  // +0x48 MfPdbListener *
    MfEvent *mRequest;                      // +0x4c script thread -> debugger thread
    MfEvent *mDone;                         // +0x50 the debugger window is done
    MfEvent *mResume;                       // +0x54 debugger thread -> script thread
    int mState;                             // +0x58
    MfPdbStepContext *mContext;             // +0x5c
    unsigned char mBytecode;                // +0x60
    MfString mModule;                       // +0x64 module shown in the frame
    MfString mModuleType;                   // +0x68
    MfString mStopModule;                   // +0x6c module where the program stopped
    int mStopLine;                          // +0x70
    int mStepping;                          // +0x74
    MfPdbMessageBox mMessageBox;            // +0x78
    CDialog *mDialog;                       // +0x8c
};

extern MfPdbDebugger *pdbDebugger;
extern CWinThread *pdbUIThread;
extern int pdbUIRunning;
extern HINSTANCE hPscDebugger;
extern MfEvent *pdbUIReady;

// ---- this plugin: the debugger's windows ------------------------------------------

// The debugger's user interface thread: it owns the debugger frame.
class MfPdbDebuggerUI : public CWinThread {
    DECLARE_DYNCREATE(MfPdbDebuggerUI)
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
};

// The debugger window: an editor showing the module where the program
// stopped, with a tray icon (0x194 bytes).
class MfPdbDebuggerFrame : public CPiccoloFileEditor {
public:
    MfPdbDebuggerFrame();
    virtual ~MfPdbDebuggerFrame();
    virtual BOOL OnCmdMsg(UINT id, int code, void *extra, AFX_CMDHANDLERINFO *handlerInfo);
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual void ReadCurrentFile(MfString *) {}
    virtual void WriteCurrentFile(MfString *) {}
    virtual void CreateFrameWindow(CWnd *parent);
    void Activate();
    int LoadModule(int line, MfString &module, MfAiSourceType *source, MfScVmEnv *env, int showLine);
    void ClearCurrentLine();
    void AddTrayIcon();
    void RemoveTrayIcon();
    afx_msg void OnClose();
protected:
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnExit();
    afx_msg void OnShow();
    afx_msg LRESULT OnClearOutput(WPARAM, LPARAM);
    afx_msg LRESULT OnTrayNotify(WPARAM, LPARAM message);
    DECLARE_MESSAGE_MAP()
public:
    CToolBar mToolBar;              // +0xe4
    int mCurrentLine;               // +0x184
    int mTrayIcon;                  // +0x188
    int mDebugWindow;               // +0x18c output window with the debugger's text
    CConsoleEdit *mDebugEdit;       // +0x190
};

// The debugger's function registry: its entries plant breakpoints (0x10 bytes).
class MfPdbFunctionRegistry : public MfScFunctionRegistry {
public:
    MfPdbFunctionRegistry();
    virtual int Add(MfScDataHandle *function);
    virtual ~MfPdbFunctionRegistry();
};

class MfPdbFunctionRegistryModule : public MfScFunctionRegistryModule {
public:
    virtual ~MfPdbFunctionRegistryModule();
    virtual void AddFunction(MfScDataHandle *function);
};

// The list of the breakpoints in the breakpoint dialog: owner drawn, a click
// on the glyph enables or disables the breakpoint (0x54 bytes).
class MfPdbListBoxBreakpoints : public CListBox {
public:
    MfPdbListBoxBreakpoints();
    virtual ~MfPdbListBoxBreakpoints();
    virtual void DrawItem(LPDRAWITEMSTRUCT dis);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT) {}
    virtual int CompareItem(LPCOMPAREITEMSTRUCT) { return 0; }
    void DrawGlyph(CDC *dc, LPCRECT rect, MfPdbSetBreakpoint *bp);
protected:
    afx_msg void OnLButtonDown(UINT flags, CPoint point);
    DECLARE_MESSAGE_MAP()
public:
    int mItemHeight;                // +0x40
    CBitmap mEnabled;               // +0x44
    CBitmap mDisabled;              // +0x4c
};

// The breakpoint dialog (0x134 bytes).
class MfPdbEditBreakpointsDlg : public CDialog {
public:
    enum { IDD = 0x71 };
    MfPdbEditBreakpointsDlg(CWnd *parent = NULL);
protected:
    virtual void DoDataExchange(CDataExchange *dx);
    virtual BOOL OnInitDialog();
    afx_msg void OnRemove();
    afx_msg void OnSelChangeBreakpoints();
    DECLARE_MESSAGE_MAP()
public:
    CButton mRemove;                        // +0x60
    CEdit mLocation;                        // +0xa0
    MfPdbListBoxBreakpoints mBreakpoints;   // +0xe0
};

// A module of the function registry, as the module lists keep it in their
// items (8 bytes).
struct MfPdbModuleItem {
    MfPdbModuleItem(const MfString &module, const MfString &moduleType)
        : mModule(module), mModuleType(moduleType) {}
    MfString mModule;
    MfString mModuleType;
};

// The modules of the function registry as a tree (0x44 bytes).
class MfPdbModuleNameTree : public CTreeCtrl, public MfCbRegistryModuleEnumerator {
public:
    MfPdbModuleNameTree();
    virtual ~MfPdbModuleNameTree();
    virtual int Enumerate(const MfString &module, const MfString &moduleType, void *data);
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    void FillTree();
    void DeleteItemData(HTREEITEM item);
protected:
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP()
};

// The modules of the function registry as a flat list (0x44 bytes).
class MfPdbFlatModuleList : public CTreeCtrl, public MfCbRegistryModuleEnumerator {
public:
    MfPdbFlatModuleList();
    virtual ~MfPdbFlatModuleList();
    virtual int Enumerate(const MfString &module, const MfString &moduleType, void *data);
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    void FillTree();
protected:
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP()
};

// The dialog that selects the module to show (0x130 bytes).
class MfPdbModuleSelector : public CDialog {
public:
    enum { IDD = 0x75 };
    MfPdbModuleSelector(CWnd *parent = NULL);
protected:
    virtual void DoDataExchange(CDataExchange *dx);
    virtual BOOL OnInitDialog();
    afx_msg void OnDblClkTree(NMHDR *nmhdr, LRESULT *result);
    afx_msg void OnDblClkList(NMHDR *nmhdr, LRESULT *result);
    afx_msg void OnShowTree();
    DECLARE_MESSAGE_MAP()
public:
    CButton mShowTree;              // +0x60
    MfPdbModuleNameTree mTree;      // +0xa0
    MfPdbFlatModuleList mList;      // +0xe4
    MfString mModule;               // +0x128
    MfString mModuleType;           // +0x12c
};

class MfPdbViewVariable;

// The edit field with the variable name: return evaluates, tab moves the
// focus (0x4c bytes).
class MfPdbViewVariableEdit : public CEdit {
public:
    MfPdbViewVariableEdit(MfPdbViewVariable *dialog) : mDialog(dialog), mSuperProc(0) {}
protected:
    virtual WNDPROC *GetSuperWndProcAddr();
    afx_msg void OnKeyDown(UINT ch, UINT repeat, UINT flags);
    afx_msg void OnChar(UINT ch, UINT repeat, UINT flags);
    afx_msg UINT OnGetDlgCode();
    DECLARE_MESSAGE_MAP()
public:
    MfPdbViewVariable *mDialog;     // +0x40
    WNDPROC mSuperProc;             // +0x44
    CString mText;                  // +0x48
};

class MfPdbVarDispWnd;

// A line of the variable display: a variable, a field or an array element
// (0x28 bytes).
class MfPdbVarWndItem {
public:
    // Destroys the items of a list; finds an item by name.
    class VwiListItemInterface : public MfListItemInterface {
    public:
        virtual void Destroy(void *item);
        virtual int Matches(void *name, void *item);
    };

    // Flags.
    enum {
        kNotFound = 0x1,
        kError = 0x2,
        kCollapsed = 0x10000,
        kExpanded = 0x20000,
        kNoChildren = 0x40000,
        kHasChildren = 0x80000,
        kSelected = 0x100000
    };
    // Hit test results.
    enum { kHitNothing = 0, kHitExpand = 1, kHitCollapse = 2, kHitLine = 3 };

    MfPdbVarWndItem(const MfString &name, MfScDataHandle *value, unsigned long flags, MfPdbVarDispWnd *wnd,
                    int depth, MfPdbVarWndItem *parent);
    virtual ~MfPdbVarWndItem();

    void Expand(MfPdbVarDispWnd *wnd);
    void Collapse();
    void UpdateFlags();
    MfString FormatValue(int *isString);
    int Indent() { return mDepth ? mDepth * kGlyphSize + 6 : 0; }
    int GlyphLeft() { return Indent() + (mDepth ? 0 : 6); }
    int NameChars(MfPdbVarDispWnd *wnd);
    int ValueChars(MfPdbVarDispWnd *wnd);
    void DrawGlyph(CDC *dc, CRect rect, MfPdbVarDispWnd *wnd);
    void Draw(MfPdbVarDispWnd *wnd, CDC *dc, int, LPRECT rect);
    int HitTest(CPoint point, MfPdbVarDispWnd *wnd);
    void Invalidate(MfPdbVarDispWnd *wnd);

    enum { kGlyphSize = 11 };

    MfString mName;                 // +0x04
    MfString mValue;                // +0x08
    int mLines;                     // +0x0c
    int mIsString;                  // +0x10
    MfScDataHandle *mHandle;        // +0x14
    unsigned long mFlags;           // +0x18
    int mDepth;                     // +0x1c
    MfPdbVarWndItem *mParent;       // +0x20
    MfDblList *mChildren;           // +0x24 MfPdbVarWndItem *
};

// Shows the value of a variable as a tree: name and value columns with a
// splitter between them (0x9c bytes).
class MfPdbVarDispWnd : public CWnd {
public:
    MfPdbVarDispWnd(int nameWidth, CWnd *parent);
    virtual ~MfPdbVarDispWnd();
    static void Initialize();
    int GetItemRect(MfPdbVarWndItem *item, LPRECT rect);
    int CountLines();
    void SetVariable(MfScDataHandle *value, const MfString &name, int found, int redraw);
    void Clear(int redraw);
    void Select(MfDblListItem *pos, int redraw);
    void Deselect(MfDblListItem *pos, int redraw);
    int ExpandItem(MfDblListItem *pos);
    int CollapseItem(MfDblListItem *pos);
    void UpdateScrollRange();
    void SetSplitCursor(int x, int y);
    void DrawSplitter(int x, int y, int erase, int draw);
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL PreTranslateMessage(MSG *msg);
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnLButtonDown(UINT flags, CPoint point);
    afx_msg void OnVScroll(UINT code, UINT pos, CScrollBar *bar);
    afx_msg void OnMouseMove(UINT flags, CPoint point);
    afx_msg void OnLButtonUp(UINT flags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT flags, short delta, CPoint point);
    afx_msg void OnSetFocus(CWnd *old);
    afx_msg void OnKillFocus(CWnd *focus);
    afx_msg void OnKeyDown(UINT ch, UINT repeat, UINT flags);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnChar(UINT ch, UINT repeat, UINT flags);
    DECLARE_MESSAGE_MAP()
public:
    int mNameWidth;                 // +0x40
    int mHeaderHeight;              // +0x44
    int mLineHeight;                // +0x48
    int mCharWidth;                 // +0x4c
    int mTopLine;                   // +0x50
    int mWheelDelta;                // +0x54
    int mSplitting;                 // +0x58
    int mSplitX;                    // +0x5c
    int mSplitY;                    // +0x60
    CFont mHeaderFont;              // +0x64
    CFont mFont;                    // +0x6c
    MfPdbViewVariable *mDialog;     // +0x74
    MfDblList *mItems;              // +0x78 the variables shown, MfPdbVarWndItem *
    MfDblList *mRows;               // +0x7c the visible lines, MfPdbVarWndItem *
    MfDblListItem *mSelected;       // +0x80 in mRows
    CBitmap mMinus;                 // +0x84
    CBitmap mPlus;                  // +0x8c
    HCURSOR mArrow;                 // +0x94
    HCURSOR mSplitCursor;           // +0x98
};

// The dialog that shows a variable (0x6c bytes).
class MfPdbViewVariable : public CDialog {
public:
    enum { IDD = 0x6e };
    MfPdbViewVariable(CWnd *parent = NULL);
    virtual ~MfPdbViewVariable();
    void Evaluate();
    void FocusPrevious(CWnd *from);
    void FocusNext(CWnd *from);
protected:
    virtual void DoDataExchange(CDataExchange *dx);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    DECLARE_MESSAGE_MAP()
public:
    MfPdbDebuggerFrame *mFrame;     // +0x60
    MfPdbViewVariableEdit *mEdit;   // +0x64
    MfPdbVarDispWnd *mDisplay;      // +0x68
};

extern int pdbVarDisplayState;
extern char pdbVarDisplayName[];
extern const char *pszVarWndClass;

// The dialog listing the script threads (0xa0 bytes).
class MfPdbVmListDlg : public CDialog {
public:
    enum { IDD = 0x80 };
    MfPdbVmListDlg(CWnd *parent = NULL);
    void Refresh();
protected:
    virtual void DoDataExchange(CDataExchange *dx);
    virtual BOOL OnInitDialog();
    afx_msg void OnGetDispInfo(NMHDR *nmhdr, LRESULT *result);
    afx_msg void OnStopThread();
    afx_msg void OnRefresh();
    afx_msg void OnTimer(UINT id);
    DECLARE_MESSAGE_MAP()
public:
    CListCtrl mList;                // +0x60
};

#endif
