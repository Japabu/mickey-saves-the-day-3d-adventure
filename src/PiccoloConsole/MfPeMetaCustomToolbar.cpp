// PiccoloConsole.dll 0x1001a410-0x1001a70f: MfPeMetaCustomToolbar, the
// description of a custom tool bar a plugin installs, and its button entries.
#include "PiccoloConsole.h"

// 0x1001a410 ??0MfPeMetaCustomToolbar@@QAE@ABVMfString@@HPAVMfAiToolbarBitmapLoader@@@Z
MfPeMetaCustomToolbar::MfPeMetaCustomToolbar(const MfString &name, int id, MfAiToolbarBitmapLoader *loader)
    : mName(name), mID(id), mLoader(loader)
{
    mEntries = new MfDblList;
    if (mLoader)
        mLoader->AddRef();
}

// 0x1001a4b0 ??0MfPeMetaCustomToolbar@@QAE@VLiiExemplar@@@Z
MfPeMetaCustomToolbar::MfPeMetaCustomToolbar(LiiExemplar)
    : mID(0), mEntries(0), mLoader(0)
{
}

// TODO 0x1001a510 ??1MfPeMetaCustomToolbar@@UAE@XZ: 84% - the exemplar is made with the LiiExemplar constructor in the original; inlining it here (LiiExemplar has a destructor) crashes the compiler (C1001)
MfPeMetaCustomToolbar::~MfPeMetaCustomToolbar()
{
    if (mEntries) {
        MfPeMetaCustomToolbarEntry exemplar;
        mEntries->Clear(exemplar);
        delete mEntries;
    }
    if (mLoader)
        mLoader->Release();
}

// 0x1001a5b0 ?CreateInstance@MfPeMetaCustomToolbar@@QAEPAVMfPeCustomToolbar@@PAVCPiccoloWnd@@AAH@Z
MfPeCustomToolbar *MfPeMetaCustomToolbar::CreateInstance(CPiccoloWnd *wnd, int &nextCommand)
{
    return new MfPeCustomToolbar(this, mEntries, mLoader, wnd, nextCommand);
}

// 0x1001a630 ?Add@MfPeMetaCustomToolbar@@QAEXPAVMfPeMetaCustomToolbarEntry@@@Z
void MfPeMetaCustomToolbar::Add(MfPeMetaCustomToolbarEntry *entry)
{
    mEntries->Add(entry);
}

// 0x1001a640 ??0MfPeMetaToolbarEntryItem@@QAE@HABVMfString@@PAVMfAiToolbarHandlers@@@Z
MfPeMetaToolbarEntryItem::MfPeMetaToolbarEntryItem(int id, const MfString &toolTip, MfAiToolbarHandlers *handlers)
    : mToolTip(toolTip), mID(id), mHandlers(handlers)
{
    mHandlers->AddRef();
}

// 0x1001a6b0 ??1MfPeMetaToolbarEntryItem@@UAE@XZ
MfPeMetaToolbarEntryItem::~MfPeMetaToolbarEntryItem()
{
    mHandlers->Release();
}
