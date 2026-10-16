// MfScHandleList (PiccoloRT.dll): a list of data handles with a cursor of
// its own. The constructor and the cursor functions are inline.
#ifndef RUNTIMELIB_MFSCHANDLELIST_H
#define RUNTIMELIB_MFSCHANDLELIST_H
#include "MfDblList.h"
class MfScDataHandle;

class __declspec(dllimport) MfScHandleList : public MfDblList {
public:
    MfScHandleList() : mCurrentPos(0) {}
    virtual ~MfScHandleList();
    virtual void Insert(void *data, EnumListPosition pos, MfDblListItem *pli);
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pli);
    virtual void Clear();
    MfScDataHandle *First()
    {
        mCurrentPos = pliFirst;
        return mCurrentPos ? (MfScDataHandle *)mCurrentPos->pData : 0;
    }
    MfScDataHandle *Next()
    {
        MfDblListItem *&pos = mCurrentPos;
        if (pos)
            pos = pos->pliNext;
        return pos ? (MfScDataHandle *)pos->pData : 0;
    }

    MfDblListItem *mCurrentPos;                                                 // 0x10
};

#endif
