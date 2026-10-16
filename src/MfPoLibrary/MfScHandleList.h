// MfScHandleList and MfPlList (PscStdlib.dll): the standard library's script
// list. The list interface's GetList hands out the MfScHandleList, a list of
// data handles with a cursor of its own (only its inline iteration is used
// here); MfPlList is the list object (only its type is used here: typeid).
// Declarations as in src/plugins/PscStdlib/PscStdlib.h.
#pragma once
#include "MfDblList.h"
#include "MfScDataObject.h"

class MfScDataHandle;

class __declspec(dllimport) MfScHandleList : public MfDblList {
public:
    virtual ~MfScHandleList();
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
private:
    MfDblListItem *mCurrentPos;     // +0x10
};

class __declspec(dllimport) MfPlList : public MfScDataObject {
public:
    virtual ~MfPlList();
};
