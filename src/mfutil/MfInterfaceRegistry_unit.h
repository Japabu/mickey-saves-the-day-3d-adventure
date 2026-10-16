// MfInterfaceRegistry (mfutil.dll 0x10004760-0x100049a0): static registry of MfAi
// interfaces in an MfDictionary keyed by interface name. Layouts from the code.
#ifndef MFUTIL_MFINTERFACEREGISTRY_H
#define MFUTIL_MFINTERFACEREGISTRY_H

#define MFUTIL_API __declspec(dllexport)

// vtable 0x100172d4: ??_E, CallBack(3), CallBack(2), CallBack(1)
class MFUTIL_API MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *p1) { return 0; }
    virtual int CallBack(void *p1, void *p2) { return 0; }
    virtual int CallBack(void *p1, void *p2, void *p3) { return 0; }
};

// size 4 (pointer to the shared MfStringRef); only what this unit uses
class MFUTIL_API MfString {
public:
    MfString(const char *s);
    MfString(const MfString &s);
    ~MfString();
private:
    void *m_ref;
};

enum MfHTErrorCode {
    MFHT_EC_NO_ERROR,
    MFHT_EC_ALREADY_IN_TABLE,
    MFHT_EC_NOT_IN_TABLE
};

// size 0x14; vtable 0x10017384: ??_E, Search, DeleteDataItem, GetKey, GetData
class MFUTIL_API MfHashTable {
public:
    class MfChainNode;
    MfHashTable(int nBuckets = 0x35, int nSize = 4);
    virtual ~MfHashTable();
    void Iterate(MfCallBack &cb);

protected:
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfChainNode **ppPrev) = 0;
    void ClearItems(MfCallBack *cb);
    virtual void DeleteDataItem(void *pItem, MfCallBack *cb) = 0;
    virtual void *GetKey(void *pItem) = 0;
    virtual void *GetData(void *pItem) = 0;

    int nNumberOfBuckets;        // +4
    int nBucketSize;             // +8
    void **apBuckets;            // +0xc
    MfChainNode **apmfcnChain;   // +0x10
};

// vtable 0x1001739c
class MFUTIL_API MfDictionary : public MfHashTable {
public:
    MfDictionary(int nBuckets = 0x35);
    virtual ~MfDictionary();
    void *Add(const MfString &key, void *pData);
    void *Replace(const MfString &key, void *pData);
    void *Delete(const MfString &key);
    void *Find(const MfString &key);
    void Clear(MfCallBack *cb = 0) { ClearItems(cb); }

protected:
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfChainNode **ppPrev);
    virtual void DeleteDataItem(void *pItem, MfCallBack *cb);
    virtual void *GetKey(void *pItem);
    virtual void *GetData(void *pItem);
};

// Abstract interface (slot names guessed from use): slot 0 writes the interface
// name into a buffer, slot 1 adds a reference, slot 2 releases one.
class MFUTIL_API MfAi {
public:
    virtual void GetName(char *pszName, int nSize, int *p1, int *p2) = 0;
    virtual void AddRef() = 0;
    virtual void Release() = 0;
};

class MFUTIL_API MfInterfaceRegistry {
public:
    static int AddInterface(MfAi *pai);
    static int GetInterface(const MfString &name, MfAi **ppai);
    static int RemoveInterface(const MfString &name);
    static void Shutdown();
private:
    static void Initialize();
    static MfDictionary *pdictRegistry;
};

#endif
