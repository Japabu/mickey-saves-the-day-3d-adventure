// PiccoloRT.dll 0x1000bc00-0x1000bd73: MfScHandleList, a list of data handles
// that holds a reference on each of them.
#include "MfScHandleList.h"
#include "MfScDataHandle.h"
#include "MfCallBack.h"

// 0x1000a5c0 ??0MfScHandleList@@QAE@XZ (inline, header)
// 0x1000a5e0 ?First@MfScHandleList@@QAEPAVMfScDataHandle@@XZ (inline, header)
// 0x1000a5f0 ?Next@MfScHandleList@@QAEPAVMfScDataHandle@@XZ (inline, header)
// 0x1000a610 ?Current@MfScHandleList@@QAEPAVMfScDataHandle@@XZ (inline, header)
// 0x1000a620 ?Prev@MfScHandleList@@QAEPAVMfScDataHandle@@XZ (inline, header)
// 0x1000a640 ?Last@MfScHandleList@@QAEPAVMfScDataHandle@@XZ (inline, header)
// 0x1000a650 ?CurrentPos@MfScHandleList@@QAEAAPAVMfDblListItem@@XZ (inline, header)

// Releases the handles of a list that is being cleared.
class MfScHandleListReleaser : public MfCallBack {
public:
    virtual int CallBack(void *data);
};

// 0x1000bc00 ??1MfScHandleList@@UAE@XZ
MfScHandleList::~MfScHandleList()
{
    Clear();
}

// 0x1000bc50 ?Insert@MfScHandleList@@UAEXPAXW4EnumListPosition@MfDblList@@PAVMfDblListItem@@@Z
void MfScHandleList::Insert(void *item, MfDblList::EnumListPosition pos, MfDblListItem *where)
{
    if (item) {
        ((MfScDataHandle *)item)->AddRef();
        MfDblList::Insert(item, pos, where);
    }
}

// 0x1000bc70 ?DeleteFirst@MfScHandleList@@UAEPAXXZ
void *MfScHandleList::DeleteFirst()
{
    MfDblListItem *first = pliFirst;
    MfScDataHandle *handle = (MfScDataHandle *)MfDblList::DeleteFirst();
    if (handle)
        handle->Release();
    if (first == mCurrentPos)
        mCurrentPos = pliFirst;
    return 0;
}

// 0x1000bca0 ?DeleteLast@MfScHandleList@@UAEPAXXZ
void *MfScHandleList::DeleteLast()
{
    MfDblListItem *last = pliLast;
    MfScDataHandle *handle = (MfScDataHandle *)MfDblList::DeleteLast();
    if (handle)
        handle->Release();
    if (last == mCurrentPos)
        mCurrentPos = 0;
    return 0;
}

// 0x1000bcd0 ?Delete@MfScHandleList@@UAEPAXAAPAVMfDblListItem@@@Z
void *MfScHandleList::Delete(MfDblListItem *&pos)
{
    MfDblListItem *item = pos;
    MfScDataHandle *handle = (MfScDataHandle *)MfDblList::Delete(pos);
    if (handle)
        handle->Release();
    if (item == mCurrentPos)
        mCurrentPos = pos;
    return 0;
}

// 0x1000bd10 ?Clear@MfScHandleList@@UAEXXZ
void MfScHandleList::Clear()
{
    MfScHandleListReleaser releaser;
    MfDblList::Clear(releaser);
}

// 0x1000bd50 ?CallBack@MfScHandleListReleaser@@UAEHPAX@Z
int MfScHandleListReleaser::CallBack(void *data)
{
    MfScDataHandle *handle = (MfScDataHandle *)data;
    if (handle && handle != pdhUndefined)
        handle->Release();
    return 1;
}
