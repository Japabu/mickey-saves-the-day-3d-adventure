#pragma once
#include "MfDblList.h"
class MfScDataHandle;

// PiccoloRT.dll: a list of data handles with a cursor of its own (0x14 bytes).
class __declspec(dllimport) MfScHandleList : public MfDblList {
public:
    MfScHandleList() : mCurrentPos(0) {}
    virtual ~MfScHandleList();
    virtual void Insert(void *, MfDblList::EnumListPosition, MfDblListItem *);
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&);
    virtual void Clear();
    MfScDataHandle *First() { return (MfScDataHandle *)MfDblList::First(mCurrentPos); }
    MfScDataHandle *Next() { return (MfScDataHandle *)MfDblList::Next(mCurrentPos); }
private:
    MfDblListItem *mCurrentPos;     // +0x10
};
