// MfImageHandler: image handler registry of MfImageLib (MfImageLib.dll, VC5, no PDB).
#ifndef MFIMAGEHANDLER_H
#define MFIMAGEHANDLER_H

#include <windows.h>

// mfutil.dll
class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    ~MfString();
    MfString &operator=(const MfString &s);
    operator const char *() const;
    void SplitPath(MfString *drive, MfString *dir, MfString *name, MfString *ext) const;
    int CaseContainsSubString(const MfString &s, int start) const;
private:
    char *m_str;
};

class MfDblListItem {
public:
    void *pData;            // +0
    MfDblListItem *pliPrev; // +4
    MfDblListItem *pliNext; // +8
};

// MfImageLib's view of mfutil's MfDblList: the constructor is inline here
// (stores the imported vtable), the destructor/Insert/Clear are imported.
// The inline helpers stand for whatever inline accessors the original header had.
class __declspec(dllimport) MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    MfDblList() : pliFirst(0), pliLast(0), nCount(0) {}
    virtual ~MfDblList();                                                    // +0x00
    virtual void Prepend(void *const pData);                                 // +0x04
    virtual void Append(void *const pData);                                  // +0x08
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli); // +0x0c
    virtual void Clear();

    void AppendItem(void *p) { Insert(p, LP_AFTER, pliLast); }
    void *First(MfDblListItem *&pli) const { pli = pliFirst; return pli ? pli->pData : 0; }
    void *Next(MfDblListItem *&pli) const
    {
        if (pli) pli = pli->pliNext;
        return pli ? pli->pData : 0;
    }

    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};

enum MfImageError {
    MfImageOk = 0,
    MfImageUnsupported = 1,
    MfImageCantOpen = 2,
    MfImageBadFile = 3
};

enum MfImageOp {
    MfImageRead = 0,
    MfImageWrite = 1
};

class __declspec(dllexport) MfImage;

// 0x14 bytes; implicit copy constructor, operator= and destructor (exported)
struct __declspec(dllexport) MfImageParams {
    MfImageParams() : image(0), alignment(0), flags(0x1ffff), field_10(0) {}
    MfImage *image;  // 0x00
    MfString path;   // 0x04
    int alignment;   // 0x08
    int flags;       // 0x0c
    int field_10;    // 0x10
};

// no data, no virtual destructor (DllMain deletes with plain operator delete)
class MfImageHandler {
public:
    virtual const MfString &GetExtensions() = 0;
    virtual bool CanHandle(MfImageOp op) = 0;
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params) = 0;
};

class MfBmpHandler : public MfImageHandler {
public:
    virtual const MfString &GetExtensions();
    virtual bool CanHandle(MfImageOp op);
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params);
};

class MfTgaHandler : public MfImageHandler {
public:
    virtual const MfString &GetExtensions();
    virtual bool CanHandle(MfImageOp op);
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params);
};

// 0x240 bytes (libjpeg state; layout belongs to the MfJpgHandler unit)
class MfJpgHandler : public MfImageHandler {
public:
    virtual const MfString &GetExtensions();
    virtual bool CanHandle(MfImageOp op);
    virtual MfImageError Handle(MfImageOp op, MfImageParams &params);
private:
    char m_state[0x23c];
};

__declspec(dllexport) MfImageHandler *MfFindImageHandler(MfImageOp op, const MfString &path);

#endif
