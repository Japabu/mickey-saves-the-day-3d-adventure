// MfStringList (mfutil.dll): a MfDblList of heap MfStrings that is also a
// MfStringTokenizerCallBack. Declaration as in src/mfutil; the constructors
// are inline (callers store the imported vtables), the destructor is not.
#ifndef RUNTIMELIB_MFSTRINGLIST_H
#define RUNTIMELIB_MFSTRINGLIST_H
#include "MfDblList.h"
#include "MfString.h"

class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest);
};

class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList() { Clear(); }
    MfString *First(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetFirst(pli); }
    MfString *Next(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetNext(pli); }
    virtual MfString *Add(const MfString &s);
    virtual void Delete(const MfString &s);
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pli);
    virtual void Clear();
    virtual MfDblListItem *Search(const MfString &s) const;
    void Sort(int bCaseSensitive);
private:
    virtual int CallBack(const MfString &token, MfString &rest);
};

#endif
