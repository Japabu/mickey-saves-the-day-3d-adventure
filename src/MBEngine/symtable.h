// symtable.h - declarations for MBEngine.dll symtable.obj (script symbol table)
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <string>

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) size_t MBstrlen(const char *s);
extern "C" __declspec(dllimport) char *MBstrcpy(char *dst, const char *src);
extern "C" __declspec(dllimport) char *MBstrncpy(char *dst, const char *src, size_t n);
extern "C" __declspec(dllimport) int MBstrcmp(const char *a, const char *b);
extern "C" __declspec(dllimport) void *MBmemcpy(void *dst, const void *src, size_t n);
#include <stdio.h>

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

// header-defined interface IDs (local data of the object, PDB ldata)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

enum IMBStreamSeekValues { IMBStreamSeekDummy };
enum IMBStreamError { IMBStreamErrorDummy };

class IMBStream : public IMBUnknown {
public:
    static const MBGUID &__cdecl GetGUID(void);
    virtual bool Read(void *, unsigned long &) = 0;
    virtual bool Write(const void *, unsigned long) = 0;
    virtual bool Seek(long, IMBStreamSeekValues) = 0;
    virtual unsigned long GetPosition(void) = 0;
    virtual bool Flush(void) = 0;
    virtual bool SetLastError(IMBStreamError) = 0;
    virtual IMBStreamError GetLastError(void) = 0;
    virtual unsigned long GetStreamSize(void) = 0;
    virtual void Clone(IMBStream **) = 0;
};

enum { kNumType = 0, kNameType = 1, kStringType = 2 };

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
    const char *GetString(void) { return value.str; }
    TSymtabNode *LeftSubtree(void) { return left; }
    TSymtabNode *RightSubtree(void) { return right; }
    void SetValue(float num);
    void SetValue(long num);
    void SetValue(const char *str);
    unsigned long Index(void) const { return nodeIndex; }
    void Print(void) const;
    unsigned long StreamOut(IMBStream *outFile);
    unsigned long StreamIn(const char *buffer);

    friend class TSymtab;
};

class TSymtab {
    TSymtabNode *root;
    long nodeCnt;
    long symbolTableIndex;
    static long symbolTableCnt;
    static TSymtab *symbolTableList;
    // stand-in: the original's ctype id $E22/$E21 are one past <string> (18/19) + TSymtab's
    // 2 statics; the obj's PDB data has no further guard/static, so one more static data member
    // declaration takes it (name unknown; never defined or used)
    static long sUnknownSymtabStatic;
    TSymtab *next;
public:
    TSymtab(void);
    ~TSymtab(void);
    TSymtabNode *Search(const char *pString) const;
    TSymtabNode *Enter(const char *pString);
    TSymtabNode *Root(void) const { return root; }
    TSymtab *Next(void) const { return next; }
    int NodeCount(void) const { return nodeCnt; }
    void Print(void) const { root->Print(); }
    unsigned long StreamOut(IMBStream *outFile);
};

#endif
