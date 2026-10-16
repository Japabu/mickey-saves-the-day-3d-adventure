// MfScDataObject (PiccoloRT.dll): a Piccolo value with properties and
// methods. Members from the SteamboatApp PDB.
#pragma once
#include "MfScData.h"
class CWnd;
class MfAiClassInfo;
class MfScClassInfo;
class MfScHandleDict;
class MfScPropTokenTable;
class MfScPropertyExtList;
class MfSortedList;
class MfStringList;

class __declspec(dllimport) MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode {
        PEC_NO_ERROR = 0,
        PEC_PERMISSION_DENIED = 1,
        PEC_NO_SUCH_PROPERTY = 2,
        PEC_NO_PROPERTY_TABLE = 3,
        PEC_INVALID_PROPERTY_TYPE = 4,
        PEC_INVALID_HANDLE = 5
    };
    enum {
        SUF_NO_PROPERTIES = 1
    };
    MfScDataObject(unsigned int);
    virtual ~MfScDataObject();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName();
    virtual void GetPropertyNames(MfStringList *, int);
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    static void __cdecl FilloutPropertyNameList(MfStringList *, MfScPropTokenTable *);
    static int __cdecl LookupPropertyToken(MfScDataObject *, const MfString &, MfScPropTokenTable *);
    int LookupPropertyToken(const MfString &, MfScPropTokenTable *);
    static void __cdecl CopyHandleDictNamesToList(MfScHandleDict *, MfStringList *);
    int HasInstanceProperties() const;
    virtual MfScPropertyExtList *GetPropertyExtensions();
    int GetPropertyExt(const MfString &, MfScDataHandle **);
    int SetPropertyExt(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    void GetPropertyExtNames(MfStringList *, int);
    static void __cdecl AddClassInfo(MfScClassInfo *);
    static void __cdecl DisplayClassInfo(CWnd *);
    static void __cdecl DestroyClassInfo();
    static CWnd *__cdecl CreateClassTree(CWnd *, int);
    static void __cdecl SetClassInfoInterface(MfAiClassInfo *);
    static void __cdecl CheckClassInfoInterface();
    unsigned int Flags() const;
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
    MfScHandleDict *pdictProperties;                        // 0x0c
    unsigned int uiFlags;                                   // 0x10
    static MfSortedList *pslClassInfo;
    static MfAiClassInfo *paiciCiInterface;
    static int bAddClassInfo;
};
