// PiccoloConsole.dll 0x10010e80-0x1001116f: MfPeCustomToolbar, a custom tool
// bar in an editor window.
#include "PiccoloConsole.h"

// EQUIV 0x10010e80 ??0MfPeCustomToolbar@@QAE@PAVMfPeMetaCustomToolbar@@PAVMfDblList@@PAVMfAiToolbarBitmapLoader@@PAVCPiccoloWnd@@AAH@Z: reviewed: register choice and the order of an independent spill and push
MfPeCustomToolbar::MfPeCustomToolbar(MfPeMetaCustomToolbar *meta, MfDblList *entries,
                                     MfAiToolbarBitmapLoader *loader, CPiccoloWnd *wnd, int &nextCommand)
    : mMeta(meta), mItems(0), mCount(0), mFirstCommand(0), mToolBar(0)
{
    MfDblListItem *pli;
    MfPeMetaCustomToolbarEntry *entry;

    mToolBar = new CPiccoloToolBar;
    int count = 0;
    for (entry = (MfPeMetaCustomToolbarEntry *)entries->GetFirst(pli); entry;
         entry = (MfPeMetaCustomToolbarEntry *)entries->GetNext(pli)) {
        if (entry->mType == MfPeMetaCustomToolbarEntry::ITEM)
            count++;
    }
    mItems = new MfPeMetaToolbarEntryItem *[count];
    mCount = count;
    int item = 0;
    for (entry = (MfPeMetaCustomToolbarEntry *)entries->GetFirst(pli); entry;
         entry = (MfPeMetaCustomToolbarEntry *)entries->GetNext(pli)) {
        if (entry->mType == MfPeMetaCustomToolbarEntry::ITEM)
            mItems[item++] = entry->ToItem();
    }
    mToolBar->Create(wnd, WS_CHILD | WS_VISIBLE | CBRS_TOP, AFX_IDW_TOOLBAR);
    mToolBar->SetBarStyle(mToolBar->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    loader->LoadBitmap(mToolBar);
    mFirstCommand = nextCommand;
    UINT *ids = new UINT[entries->GetCount()];
    UINT *id = ids;
    for (entry = (MfPeMetaCustomToolbarEntry *)entries->GetFirst(pli); entry;
         entry = (MfPeMetaCustomToolbarEntry *)entries->GetNext(pli)) {
        if (entry->mType == MfPeMetaCustomToolbarEntry::ITEM) {
            *id = nextCommand;
            nextCommand++;
        } else {
            *id = ID_SEPARATOR;
        }
        id++;
    }
    mToolBar->SetButtons(ids, entries->GetCount());
    delete ids;
}

// 0x10011030 ??_GMfPeCustomToolbar@@UAEPAXI@Z

// 0x10011050 ??0MfPeCustomToolbar@@QAE@VLiiExemplar@@@Z
MfPeCustomToolbar::MfPeCustomToolbar(LiiExemplar)
    : mMeta(0), mItems(0), mCount(0), mFirstCommand(0)
{
}

// 0x10011070 ??1MfPeCustomToolbar@@UAE@XZ
MfPeCustomToolbar::~MfPeCustomToolbar()
{
    if (mItems && mCount > 0)
        delete mItems;
}

// 0x10011090 ?Destroy@MfPeCustomToolbar@@UAEXPAX@Z
void MfPeCustomToolbar::Destroy(void *data)
{
    delete (MfPeCustomToolbar *)data;
}

// The same code as MfPeCustomToolbar::Destroy, which is kept.
void MfPeMetaCustomToolbar::Destroy(void *data)
{
    delete (MfPeMetaCustomToolbar *)data;
}

void MfPeMetaCustomToolbarEntry::Destroy(void *data)
{
    delete (MfPeMetaCustomToolbarEntry *)data;
}

// 0x100110b0 ?HandleCommand@MfPeCustomToolbar@@QAEHHPAVMfAiPiccoloEditor@@@Z
int MfPeCustomToolbar::HandleCommand(int command, MfAiPiccoloEditor *editor)
{
    int handled = 0;
    if (command >= mFirstCommand && command < mFirstCommand + mCount)
        handled = mItems[command - mFirstCommand]->HandleCommand(mMeta->mID, editor);
    return handled;
}

// 0x100110f0 ?HandleUpdate@MfPeCustomToolbar@@QAEHHPAVCCmdUI@@@Z
int MfPeCustomToolbar::HandleUpdate(int command, CCmdUI *cmdUI)
{
    int handled = 0;
    if (command >= mFirstCommand && command < mFirstCommand + mCount)
        handled = mItems[command - mFirstCommand]->HandleUpdate(mMeta->mID, cmdUI);
    return handled;
}

// 0x10011130 ?GetToolTip@MfPeCustomToolbar@@QAEHHAAVMfString@@@Z
int MfPeCustomToolbar::GetToolTip(int command, MfString &toolTip)
{
    int found = 0;
    if (command >= mFirstCommand && command < mFirstCommand + mCount) {
        toolTip = mItems[command - mFirstCommand]->mToolTip;
        found = 1;
    }
    return found;
}
