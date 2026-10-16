#pragma once
#include "MfScData.h"
#include "MfString.h"
class CWnd;
class MfAiClassInfo;
class MfScClassInfo;
class MfScDataHandle;
class MfScHandleDict;
class MfScLinkData;
class MfScPropTokenTable;
class MfScPropertyExtList;
class MfSortedList;
class MfStringList;

// Base of the script objects: properties come from the class (token table and
// property extensions) and from a per-instance variable pool.
class __declspec(dllexport) MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode {
        kPropOk = 0,
        kPropReadOnly = 1,
        kPropNotFound = 2,
        kPropNoInstanceProperties = 3
    };
    // constructor flags
    enum {
        kNoInstanceProperties = 0x1,
        kObject = 0x200
    };
    MfScDataObject(unsigned int flags = 0);
    virtual ~MfScDataObject();
protected:
    virtual MfScLinkData * CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
public:
    virtual MfPropertyErrorCode GetProperty(MfString const &, MfScDataHandle * *);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, MfString const &, MfScDataHandle *, int);
    virtual MfString GetClassNameA() { return MfString("Object"); }
    virtual void GetPropertyNames(MfStringList *, int);
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable * GetPropTokenTable() { return 0; }
    virtual void FilloutPropertyNameList(MfStringList *) {}
    virtual MfScPropertyExtList * GetPropertyExtensions() { return 0; }

    int HasInstanceProperties() const { return mInstanceProperties != 0; }
    unsigned int Flags() const { return mFlags; }
    int LookupPropertyToken(MfString const &, MfScPropTokenTable *);
    int GetPropertyExt(MfString const &, MfScDataHandle * *);
    int SetPropertyExt(MfScDataHandle *, MfString const &, MfScDataHandle *, int);
    void GetPropertyExtNames(MfStringList *, int);

    static int __cdecl LookupPropertyToken(MfScDataObject *, MfString const &, MfScPropTokenTable *);
    static void __cdecl FilloutPropertyNameList(MfStringList *, MfScPropTokenTable *);
    static void __cdecl CopyHandleDictNamesToList(MfScHandleDict *, MfStringList *);
    static void __cdecl AddClassInfo(MfScClassInfo *);
    static void __cdecl DestroyClassInfo();
    static void __cdecl DisplayClassInfo(CWnd *);
    static void __cdecl CheckClassInfoInterface();
    static CWnd * __cdecl CreateClassTree(CWnd *, int);
    static void __cdecl SetClassInfoInterface(MfAiClassInfo *ci) { paiciCiInterface = ci; }
protected:
    static int bAddClassInfo;
    static MfAiClassInfo * paiciCiInterface;
    static MfSortedList * pslClassInfo;

    MfScHandleDict *mInstanceProperties;    // +0x0c
    unsigned int mFlags;                    // +0x10 (0x200 object, 0x400 data object)
};
