// CMBWinFileSpec.h - declarations for MBEngine.dll CMBWinFileSpec.obj
#ifndef CMBWINFILESPEC_H
#define CMBWINFILESPEC_H

#include "MBFileSystemWin.h"

extern "C" __declspec(dllimport) int MBstricmp(const char *, const char *);
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *);

extern const char *kMBDirectorySeparator;
extern const char *kMBExtensionSeparator;

class CMBWinFileSpec : public IMBFileSpec {
public:
    CMBWinFileSpec(void);
    ~CMBWinFileSpec(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void SetPath(const char *path);
    virtual void SetPath(const IMBFileSpec *parent, const char *path);
    virtual MBString GetName(void) const;
    virtual MBString GetExtension(void) const;
    virtual MBString GetPath(void) const;
    virtual MBString GetTitle(void) const;
    virtual unsigned long GetType(void) const;
    virtual void GetParent(IMBFileSpec **fileSpec) const;
    virtual unsigned long Hash(void) const;
protected:
    void Update(void);
    MBString GetRoot(void) const;
    MBString mPath;
    WIN32_FIND_DATAA mFileInfo;
    bool mFoundFile;
};

IMBUnknown *MakeCMBWinFileSpec(void);

#endif
