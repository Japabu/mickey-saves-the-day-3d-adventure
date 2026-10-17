// MfScHandleList: a list of data handles with a cursor of its own.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfDblList.h"
class MfScDataHandle;

// List of data handles with a cursor of its own.
class PICCOLORT_API MfScHandleList : public MfDblList {
public:
    MfScHandleList() : mCurrentPos(0) {}
    virtual ~MfScHandleList();
    virtual void Insert(void *, MfDblList::EnumListPosition, MfDblListItem *);
    virtual void * DeleteFirst();
    virtual void * DeleteLast();
    virtual void * Delete(MfDblListItem * &);
    virtual void Clear();
    MfScDataHandle *First()
    {
        mCurrentPos = pliFirst;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Next()
    {
        if (mCurrentPos)
            mCurrentPos = mCurrentPos->pliNext;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Current()
    {
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Prev()
    {
        if (mCurrentPos)
            mCurrentPos = mCurrentPos->pliPrev;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Last()
    {
        mCurrentPos = pliLast;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfDblListItem *&CurrentPos() { return mCurrentPos; }
private:
    MfDblListItem *mCurrentPos;     // +0x10
};
