#pragma once
#include "MfDblList.h"
class MfString;

// mfutil.dll
class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest);
};

// mfutil.dll: list of MfString (0x14 bytes).
class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList() { Clear(); }
    virtual MfString *Add(const MfString &s);                     // +0x70
    virtual void Delete(const MfString &s);                       // +0x74
    virtual void *DeleteFirst();                                  // +0x40
    virtual void *DeleteLast();                                   // +0x44
    virtual void *Delete(MfDblListItem *&pli);                    // +0x48
    virtual void Clear();                                         // +0x54
    virtual MfDblListItem *Search(const MfString &s) const;       // +0x78
    void Sort(int bCaseSensitive);
    // Inline iteration over the strings (not exported by mfutil.dll).
    MfString *FirstString(MfDblListItem *&pli) const
    {
        MfString *s = (MfString *)First(pli);
        return s;
    }
    MfString *NextString(MfDblListItem *&pli) const
    {
        MfString *s = (MfString *)Next(pli);
        return s;
    }
private:
    virtual int CallBack(const MfString &token, MfString &rest);
};
