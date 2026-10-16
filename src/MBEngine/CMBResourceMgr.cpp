// MBEngine.dll unit CMBResourceMgr (.\MBEngineRelease\CMBResourceMgr.obj)
#include "CMBResourceMgr.h"

// 0x1000eee0 ?MakeCMBResourceMgr@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBResourceMgr(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton(new CMBResourceMgr);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x1000ef50 ??0CMBResourceMgr@@QAE@XZ
CMBResourceMgr::CMBResourceMgr(void)
{
    _mRefCnt = 0;
}

// 0x1000ef90 ?QueryInterface@CMBResourceMgr@@UAEJABUMBGUID@@PAPAX@Z
long CMBResourceMgr::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBResourceFileList)
        p = static_cast<IMBResourceFileList *>(this);
    if (iid == MBIID_IMBResourceFile)
        p = static_cast<IMBResourceFile *>(this);
    if (iid == MBIID_IMBUnknown)
        p = static_cast<IMBUnknown *>(this);
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000f090 ?AddRef@CMBResourceMgr@@UAEKXZ
unsigned long CMBResourceMgr::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000f0a0 ?Release@CMBResourceMgr@@UAEKXZ
unsigned long CMBResourceMgr::Release(void)
{
    unsigned long count = --_mRefCnt;
    if (_mRefCnt == 0)
        delete this;
    return count;
}

// 0x1000f0d0 ??1CMBResourceMgr@@QAE@XZ
CMBResourceMgr::~CMBResourceMgr(void)
{
    for (MBList<MBComPtr<IMBResourceFile> >::iterator it = mResFileList.begin(); it != mResFileList.end(); ++it)
        (*it)->Release();
}

// 0x1000f160 ?LoadResource@CMBResourceMgr@@UAEXKKPAPAVIMBMemoryBlock@@@Z
void CMBResourceMgr::LoadResource(unsigned long resID, unsigned long type, IMBMemoryBlock **resourceData)
{
    *resourceData = 0;
    for (MBList<MBComPtr<IMBResourceFile> >::iterator it = mResFileList.begin(); it != mResFileList.end(); ++it) {
        (*it)->LoadResource(resID, type, resourceData);
        if (*resourceData)
            return;
    }
}

// 0x1000f1b0 ?GetStream@CMBResourceMgr@@UAEXKKPAPAVIMBStream@@@Z
void CMBResourceMgr::GetStream(unsigned long resID, unsigned long type, IMBStream **stream)
{
    *stream = 0;
    for (MBList<MBComPtr<IMBResourceFile> >::iterator it = mResFileList.begin(); it != mResFileList.end(); ++it) {
        (*it)->GetStream(resID, type, stream);
        if (*stream)
            return;
    }
}

// EQUIV 0x1000f200 ?AddFile@CMBResourceMgr@@UAEXPAVIMBResourceFile@@W4MBResFileListAddLoc@@K@Z: reviewed: the original calls list::insert out of line on the push_back path, VC6 inlines it; registers and slots differ, the calls and their order agree. Was: AddFile: push_back path matches up to register/stack-slot allocation, but the original calls list::insert out of line for push_front while VC6 here inlines it (inlining decision not reproduced) (80% similar)
void CMBResourceMgr::AddFile(IMBResourceFile *file, MBResFileListAddLoc where, unsigned long priority)
{
    RemoveFile(file);
    switch (where) {
    case kResFileListFront:
        mResFileList.push_front(file);
        break;
    default:
        mResFileList.push_back(file);
        break;
    }
    file->AddRef();
}

// 0x1000f2d0 ?RemoveFile@CMBResourceMgr@@UAEXPAVIMBResourceFile@@@Z
void CMBResourceMgr::RemoveFile(IMBResourceFile *file)
{
    for (MBList<MBComPtr<IMBResourceFile> >::iterator it = mResFileList.begin(); it != mResFileList.end(); ++it) {
        if (*it == file) {
            file->Release();
            mResFileList.erase(it);
            return;
        }
    }
}

// TODO 0x1000f330 ?insert@?$list@V?$MBComPtr@VIMBResourceFile@@@@V?$MBAllocator@V?$MBComPtr@VIMBResourceFile@@@@@@@std@@QAE?AViterator@12@V312@ABV?$MBComPtr@VIMBResourceFile@@@@@Z: std::list<MBComPtr<IMBResourceFile>,MBAllocator<...>>::insert: template instance, only emitted out of line when AddFile's push_front call is not inlined (see 1000f200)
// 0x1000f3a0 ??0?$MBComPtr@VIMBResourceFile@@@@QAE@ABV0@@Z  (template instance, header)
// 0x1000f3c0 _$E20 (original $E20) / 0x1000f3e0 $E19: std::ctype<unsigned short>::id atexit registration, emitted by including <fstream> (header)


// Matched by this code:
// 0x1000f3e0 _$E19
