// PscStdlib.dll: the Piccolo standard library. Five script classes (List,
// Dictionary, Logger, Math, Time), each with a class object published as a
// global variable, and the list interface PiccoloRT uses to make script
// lists. Declarations of the Motion Factory runtime (mfutil.dll,
// PiccoloRT.dll) as far as this plugin uses them; names and signatures from
// the decorated import names, layouts from the code.
#ifndef PSCSTDLIB_H
#define PSCSTDLIB_H

// ---- mfutil.dll -------------------------------------------------------------

#include "../../mfutil/MfString.h"     // first: it defines STRICT before windows.h
#include <windows.h>
#include <time.h>

// own copy: the list item interface this plugin compiles has default bodies
// (Copy, Iterate and Destroy are emitted here, HandleListSearchClass overrides
// only Matches); mfutil's header declares them pure. Its guard is defined so that
// MfDblList.h uses this declaration.
#define MFUTIL_MFLISTITEMINTERFACE_H
class MfListItemInterface {
public:
    virtual void *Copy(void *item) { return 0; }
    virtual int Iterate(void *item) { return 1; }
    virtual void Destroy(void *item) {}
    virtual int Matches(void *item, void *key) { return 0; }
};

#include "../../mfutil/MfDblList.h"
#include "../../mfutil/MfStringList.h"
#include "../../mfutil/MfDictionary.h"
#include "../../mfutil/MfSystemTime.h"
#include "../../mfutil/MfLogger.h"
#include "../../mfutil/MfThread.h"

// ---- PiccoloRT.dll: the script runtime ---------------------------------------
//
// PiccoloRT's headers include PiccoloRT's own copies of the mfutil classes
// (src/PiccoloRT/MfString.h, MfDblList.h, MfThread.h, ...), which cannot be
// declared next to mfutil's headers above. Only the PiccoloRT headers that
// include none of them are used here; the other classes are declared below as
// this plugin compiles them, until PiccoloRT's headers use mfutil's.

class MfScData;
class MfScDataHandle;
class MfScDataObject;
class MfScVmEnv;

// A script thread: its VM environment follows the MfThread part.
class MfScThread : public MfThread {
public:
    MfScVmEnv *mVmEnv;      // +0x20
};

typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

// A reference-counted, typed value of the script VM (8 bytes).
class __declspec(dllimport) MfScDataHandle {
public:
    // type flags
    enum {
        kInteger = 0x10,
        kFloat = 0x20,
        kNumber = 0x30,
        kObject = 0x200,
        kString = 0x100,
        kContainer = 0x7c0,
        kData = 0xfc0
    };
    static MfScDataHandle *__cdecl CreateData(int);
    static MfScDataHandle *__cdecl CreateData(float);
    static MfScDataHandle *__cdecl CreateData(const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(MfScDataObject *);
    static MfScDataHandle *__cdecl CreateData(MfScMethod, const MfString &, int);
    int IsType(unsigned long type) const { return m_flags & type; }
    int IsVisiting() const { return m_flags & 4; }
    void SetVisiting() { m_flags |= 4; }
    void ResetVisiting() { m_flags &= ~4; }
    void SetReadOnly() { m_flags |= 8; }
    void SetReadWrite() { m_flags &= ~8; }
    int GetInt() const { return m_long; }
    void SetInt(int i) { m_long = i; }
    float GetFloat() const { return m_float; }
    float GetNumber() const { return (m_flags & kInteger) ? (float)m_long : m_float; }
    MfScData *GetData() const { return (m_flags & kData) ? m_data : 0; }
    unsigned short AddRef() { return ++m_refCount; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }
private:
    unsigned short m_refCount;  // +0
    unsigned short m_flags;     // +2
    union {                     // +4
        float m_float;
        long m_long;
        MfScData *m_data;
    };
};

#include "../../PiccoloRT/MfScVariableStack.h"

class __declspec(dllimport) MfScVmEnv {
public:
    static MfScVariableStack::MfScVariableErrorCode __cdecl GetGlobalVariable(const MfString &, MfScDataHandle **);
    static MfScVariableStack::MfScVariableErrorCode __cdecl SetGlobalVariable(const MfString &, MfScDataHandle *, int, int);
    static MfScVariableStack::MfScVariableErrorCode __cdecl ClearGlobalVariable(const MfString &, int);
    void ThrowException(int code, const MfString &message);
};

// Raises a script exception in the calling script thread.
inline void ThrowScriptException(int code, const MfString &message)
{
    MfScThread *thread = (MfScThread *)MfThread::GetThread();
    thread->mVmEnv->ThrowException(code, message);
}

// Error codes of MfScVmEnv::ThrowException.
enum {
    kExNilParameter = 6,
    kExNotInteger = 10,
    kExNotString = 11,
    kExBadOperand = 12,
    kExInvalidObject = 15,
    kExParameterCount = 18
};

#include "../../PiccoloRT/MfScLinkData.h"

class __declspec(dllimport) MfScData {
public:
    virtual ~MfScData();
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
public:
    virtual void AddRef();
    virtual void Release();
protected:
    int m_lock;         // +4
    MfString *m_name;   // +8
};

class MfScClassInfo;
class MfScHandleDict;
class MfScPropTokenTable;
class MfScPropertyExtList;

class __declspec(dllimport) MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode {
        kPropOk = 0,
        kPropReadOnly = 1,
        kPropNotFound = 2
    };
    // constructor flags
    enum {
        kNoInstanceProperties = 0x1,
        kObject = 0x200,
        kDataObject = 0x400
    };
    MfScDataObject(unsigned int);
    virtual ~MfScDataObject();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassNameA();
    virtual void GetPropertyNames(MfStringList *, int);
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
    int LookupPropertyToken(const MfString &, MfScPropTokenTable *);
    static void __cdecl FilloutPropertyNameList(MfStringList *, MfScPropTokenTable *);
    static void __cdecl AddClassInfo(MfScClassInfo *);
protected:
    MfScHandleDict *mInstanceProperties;    // +0x0c
    unsigned int mFlags;                    // +0x10
};

class __declspec(dllimport) MfScObject : public MfScDataObject {
public:
    MfScObject() : MfScDataObject(kObject) {}
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
};

class __declspec(dllimport) MfScObjString : public MfScObject, public MfString {
private:
    int m_readOnly;
};

// List of data handles with a cursor of its own.
class __declspec(dllimport) MfScHandleList : public MfDblList {
public:
    MfScHandleList() : mCurrentPos(0) {}
    virtual ~MfScHandleList();
    virtual void Insert(void *, MfDblList::EnumListPosition, MfDblListItem *);
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&);
    virtual void Clear();
    MfScDataHandle *First()
    {
        mCurrentPos = pliFirst;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Next()
    {
        if (mCurrentPos)
            mCurrentPos = mCurrentPos->pliNext;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Current()
    {
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Prev()
    {
        if (mCurrentPos)
            mCurrentPos = mCurrentPos->pliPrev;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Last()
    {
        mCurrentPos = pliLast;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfDblListItem *&CurrentPos() { return mCurrentPos; }
private:
    MfDblListItem *mCurrentPos;     // +0x10
};

// own copy: the destructor is out of line (this plugin imports
// ??1MfScHandleDict@@UAE@XZ for its ??_GMfScHandleDict); PiccoloRT's header has
// it inline.
// Variable pool: name -> data handle; it holds a reference on each handle.
class __declspec(dllimport) MfScHandleDict : public MfDictionary {
public:
    enum HandleDictPassRef { kAddRef = 0, kPassRef = 1 };
    MfScHandleDict() : MfDictionary(0x35) {}
    virtual ~MfScHandleDict();
    MfScDataHandle *Find(const MfString &);
    void Replace(const MfString &, MfScDataHandle *, HandleDictPassRef);
    void Delete(const MfString &, MfScDataHandle **);
    void Clear();
};

// A property name and the token a class answers for it (8 bytes).
class __declspec(dllimport) MfScPropToken {
public:
    MfScPropToken(const MfString &property, int token) : m_token(token), m_property(property) {}
    const MfString &Property() const { return m_property; }
private:
    int m_token;            // +0
    MfString m_property;    // +4
};

// Dictionary property name -> MfScPropToken; it is its own delete callback.
class __declspec(dllimport) MfScPropTokenTable : public MfDictionary, public MfCallBack {
public:
    MfScPropTokenTable() : MfDictionary(0x35) {}
    virtual ~MfScPropTokenTable();
    void Add(const MfString &property, int token)
    {
        MfScPropToken *pt = new MfScPropToken(property, token);
        MfDictionary::Add(pt->Property(), pt);
    }
    virtual int CallBack(void *);
};

#include "../../PiccoloRT/MfScStdLibListInterface.h"

// own copy: RegisterListInterface is inline (this plugin imports
// pslliListInterface and slliDefault, not the function); PiccoloRT's header
// declares it out of line.
class __declspec(dllimport) MfScStdLib {
public:
    static void RegisterListInterface(MfScStdLibListInterface *slli)
    {
        if (slli)
            pslliListInterface = slli;
        else
            pslliListInterface = &slliDefault;
    }
private:
    static MfScStdLibListInterface *pslliListInterface;
    static MfScStdLibListInterface slliDefault;
};

// ---- class information (the help/browser tree) ---------------------------------

struct _TREEITEM;

// own copy: the key slot is GetKey, which MfScClassInfoItem and MfScClassInfo
// override (PiccoloRT exports ?GetKey@MfScClassInfoItem); mfutil's header names
// it GetSortKey.
class MfSortedListItem {
public:
    virtual ~MfSortedListItem() {}
    virtual MfString GetKey() const = 0;
};

class __declspec(dllimport) MfScClassInfoText {
public:
    MfScClassInfoText() {}
    virtual ~MfScClassInfoText();
protected:
    MfString m_text;
};

class __declspec(dllimport) MfScClassInfoItem : public MfSortedListItem, public MfScClassInfoText {
public:
    MfScClassInfoItem(const MfString &key) : m_c(0), m_key(key) {}
    virtual MfString GetKey() const;
protected:
    int m_c;            // +0x0c
    MfString m_key;     // +0x10
};

class __declspec(dllimport) MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &, int);
    virtual ~MfScMethodParameterInfo();
private:
    int m_8;
    int m_c;
};

class __declspec(dllimport) MfScCiMethod : public MfScClassInfoItem {
public:
    MfScCiMethod(const MfString &);
    virtual ~MfScCiMethod();
    virtual _TREEITEM *DisplayInTree(HWND, _TREEITEM *);
    virtual int GetTreeImage();
    MfScCiMethod &operator+(MfScMethodParameterInfo *);
private:
    int m_14;
};

class __declspec(dllimport) MfScCiProperty : public MfScClassInfoItem {
public:
    MfScCiProperty(const MfString &name) : MfScClassInfoItem(name) {}
    virtual ~MfScCiProperty();
    virtual _TREEITEM *DisplayInTree(HWND, _TREEITEM *);
    virtual int GetTreeImage();
};

class __declspec(dllimport) MfScClassInfo : public MfSortedListItem, public MfListItemInterface, public MfScClassInfoText {
public:
    MfScClassInfo(const MfString &);
    virtual ~MfScClassInfo();
    virtual MfString GetKey() const;
    MfScClassInfo &operator+(MfScClassInfoItem *);
private:
    MfString m_name;
    void *m_items;
};

// ---- this plugin ------------------------------------------------------------

// By-value argument of the constructors that make a class object (the
// one instance a class is published as) instead of a script instance.
class MfPlClassArg {
public:
    MfPlClassArg() {}
    ~MfPlClassArg() {}
};

#include "MfPlList.h"

// The script class "Dictionary": data handles by name.
class MfPlDictionary : public MfScObject {
public:
    MfPlDictionary();
    MfPlDictionary(MfPlClassArg);
    virtual ~MfPlDictionary();
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int passRef);
    virtual MfString GetClassNameA();

    MfScDataHandle *Dictionary(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Clear(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Set(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Get(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Delete(MfScDataHandle *self, int argc, MfScDataHandle **argv);

    static void Initialize();
    static void Shutdown();
    static MfString GetConstants();

private:
    int IsValid();

    MfScHandleDict *mDict;      // +0x14
};

// The script class "Logger": messages to the MfLogger log.
class MfPlLogger : public MfScObject {
public:
    MfPlLogger(MfPlClassArg);
    virtual ~MfPlLogger() {}
    virtual MfString GetClassNameA();

    MfScDataHandle *Log(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *SetOptions(MfScDataHandle *self, int argc, MfScDataHandle **argv);

    static void Initialize();
    static void Shutdown();
    static MfString GetConstants();

    // SetOptions
    enum { PL_ENABLE = 1, PL_DISABLE = 2 };
};

// The script class "Math".
class MfPlMath : public MfScObject {
public:
    MfPlMath(MfPlClassArg);
    virtual ~MfPlMath() {}
    virtual MfString GetClassNameA();

    MfScDataHandle *Sin(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Cos(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Tan(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Asin(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Acos(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Atan(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Sqrt(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Pow(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Abs(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Int(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Float(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Rand(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Test(MfScDataHandle *self, int argc, MfScDataHandle **argv);

    static void Initialize();
    static void Shutdown();
    static MfString GetConstants();
};

// The script class "Time": the local time.
class MfPlTime : public MfScObject {
public:
    MfPlTime(MfPlClassArg);
    virtual ~MfPlTime() {}
    virtual MfString GetClassNameA();

    MfScDataHandle *TimeStamp(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *GetDayOfWeek(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *GetDay(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *GetMonth(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *GetYear(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *GetHour(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *GetMinute(MfScDataHandle *self, int argc, MfScDataHandle **argv);

    static void Initialize();
    static void Shutdown();
};

// The list interface: script lists for PiccoloRT.
class MfPlListInterface : public MfScStdLibListInterface {
public:
    virtual int CreateList(MfDblList **list, MfScDataHandle **handle);
    virtual int GetList(MfDblList **list, MfScDataHandle *handle);
};

// Compares data handles by value (MfDblList::Find).
class HandleListSearchClass : public MfListItemInterface {
public:
    ~HandleListSearchClass() {}
    virtual int Matches(void *item, void *key);
};

// The standard library plugin entry points (PiccoloRT loads them by name).
typedef int (*MfConstInitFunction)(const MfString &name, const MfString &source, MfString *errors);
extern "C" {
__declspec(dllexport) int __stdcall PscInitialize(MfConstInitFunction constInit, MfString *errors);
__declspec(dllexport) int __stdcall PscShutdown(MfString *errors);
__declspec(dllexport) int __stdcall PscIsStandardLibrary();
}

#endif
