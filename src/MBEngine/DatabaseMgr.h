// DatabaseMgr.h - the engine's resource databases: one Database per zip or directory, keyed by name hash.
// Declared first by the includer (still one copy per unit in wave 1): MBComPtr, MBMultimap, MBString.
#ifndef MBENGINE_DATABASEMGR_H
#define MBENGINE_DATABASEMGR_H

class IMBFileSpec;
class IMBResourceFile;

class DatabaseMgr {
    class Database {
    public:
        Database(IMBResourceFile *resFile);
        unsigned long AddDBRef(void);
        unsigned long ReleaseDBRef(void);
    protected:
        MBComPtr<IMBResourceFile> mResFile;
        unsigned long mCount;
    };
    typedef MBMultimap<unsigned long, Database *, std::less<unsigned long> > DatabaseMap;
public:
    bool Initialize(const MBString &cdPath, const MBString &hdPath);
    bool AddDatabase(const char *name);
    bool RemoveDatabase(const char *name);
    ~DatabaseMgr(void)
    {
        for (DatabaseMap::iterator it = theMap.begin(); it != theMap.end(); it++)
            delete (*it).second;
        theMap.erase(theMap.begin(), theMap.end());
    }
    DatabaseMap theMap;
    MBComPtr<IMBFileSpec> mCDPath;
    MBComPtr<IMBFileSpec> mHDPath;
protected:
    bool AddZipDatabase(IMBFileSpec *parentPath, const char *name);
    bool AddDirDatabase(IMBFileSpec *parentPath, const char *name);
};

#endif
