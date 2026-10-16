// MfScData, MfScDataObject, MfScObject (PiccoloRT.dll): the Piccolo values a
// MfScDataHandle refers to. Vtable order from ??_7MfScDataObject@@6B@ in
// PiccoloRT; the small accessors are inline in the original header.
// MF_SCRIPT_CLASS_DECLS (defined by MfPoBase.h, the base of RuntimeLib's own
// script classes) adds the members only those classes use. Declared for every
// unit they flip VC5's register choice in MfLiveActor::NewParentAfterUngrasp
// (any added declaration does), so they stay out of the other units.
#ifndef RUNTIMELIB_MFSCDATA_H
#define RUNTIMELIB_MFSCDATA_H
#include "MfLock.h"
#include "MfString.h"
class MfScDataHandle;
class MfScLinkData;
class MfScPropTokenTable;
class MfScPropertyExtList;
class MfScClassInfo;
class MfStringList;

class __declspec(dllimport) MfScData {
public:
    MfScData();
    MfScData(const MfScData &);
    virtual ~MfScData();
    MfScData &operator=(const MfScData &);
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
    virtual void AddRef();
    virtual void Release();
    void Lock() { mLock.Lock(-1); }
    void Unlock();
    MfString &Name() { return *mName; }
#ifdef MF_SCRIPT_CLASS_DECLS
    // the tag of the constructor that builds a class's exemplar (the object
    // the script sees as the class itself; MfPoLibrary's mangled names)
    class Exemplar {
    public:
        Exemplar() {}
        ~Exemplar() {}
    };
#endif

    MfLock mLock;                                           // 0x04
    MfString *mName;                                        // 0x08
};

class __declspec(dllimport) MfScDataObject : public MfScData {
public:
    enum MfPropertyErrorCode { MF_PROPERTY_OK };
    MfScDataObject(unsigned int flags);
    MfScDataObject(const MfScDataObject &);
    virtual ~MfScDataObject();
    MfScDataObject &operator=(const MfScDataObject &);
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
public:
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName();
    virtual void GetPropertyNames(MfStringList *, int);
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
#ifdef MF_SCRIPT_CLASS_DECLS
    static void __cdecl FilloutPropertyNameList(MfStringList *, MfScPropTokenTable *);
    int LookupPropertyToken(const MfString &, MfScPropTokenTable *);
    static int __cdecl LookupPropertyToken(MfScDataObject *, const MfString &, MfScPropTokenTable *);
    int GetPropertyExt(const MfString &, MfScDataHandle **);
    int SetPropertyExt(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void __cdecl AddClassInfo(MfScClassInfo *);
#endif
    unsigned int Flags() const { return mFlags; }
    int HasInstanceProperties() const { return mInstanceProperties != 0; }

    void *mInstanceProperties;                              // 0x0c
    unsigned int mFlags;                                    // 0x10
};

// a Piccolo object (flag 0x200)
class __declspec(dllimport) MfScObject : public MfScDataObject {
public:
    MfScObject(unsigned int flags) : MfScDataObject(flags | 0x200) {}
    MfScObject(const MfScObject &);
    // no declared destructor: the implicit one calls ~MfScDataObject (MfPoJoystick)
    MfScObject &operator=(const MfScObject &);
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
};

#endif
