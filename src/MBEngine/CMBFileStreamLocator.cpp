// MBEngine.dll .\MBEngineRelease\CMBFileStreamLocator.obj
#include "CMBFileStreamLocator.h"

// 0x1000e380 ?MakeCMBFileStreamLocator@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBFileStreamLocator(void)
{
    IMBUnknown *p = new CMBFileStreamLocator;
    if (p)
        p->AddRef();
    return p;
}

// 0x1000e3b0 ??0CMBFileStreamLocator@@QAE@XZ
CMBFileStreamLocator::CMBFileStreamLocator(void)
{
    _mRefCnt = 0;
}

// 0x1000e3d0 ?QueryInterface@CMBFileStreamLocator@@UAEJABUMBGUID@@PAPAX@Z
long CMBFileStreamLocator::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBUnknown *)this;
    if (iid == MBIID_IMBStreamLocator)
        p = (IMBStreamLocator *)this;
    if (iid == MBIID_IMBFileStreamLocator)
        p = (IMBFileStreamLocator *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000e4d0 ?AddRef@CMBFileStreamLocator@@UAEKXZ
unsigned long CMBFileStreamLocator::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000e4e0 ?Release@CMBFileStreamLocator@@UAEKXZ
unsigned long CMBFileStreamLocator::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1000e510 ??1CMBFileStreamLocator@@QAE@XZ
CMBFileStreamLocator::~CMBFileStreamLocator(void)
{
}

// 0x1000e530 ?GetStream@CMBFileStreamLocator@@UAEJPAPAVIMBStream@@@Z
long CMBFileStreamLocator::GetStream(IMBStream **stream)
{
    *stream = 0;
    if (mFile)
        MBOpenFile(mFile, 5, stream);
    return 0;
}

// 0x1000e560 ?GetID@CMBFileStreamLocator@@UAEKXZ
unsigned long CMBFileStreamLocator::GetID(void)
{
    unsigned long id = 0;
    if (mFile)
        id = mFile->Hash();
    return id;
}

// 0x1000e570 ?SetFile@CMBFileStreamLocator@@UAEJPAVIMBFileSpec@@@Z
long CMBFileStreamLocator::SetFile(IMBFileSpec *file)
{
    mFile = file;
    return 0;
}

// 0x1000e5a0 _$E20 (original $E20) / 0x1000e5c0 $E19: std::ctype<unsigned short>::id atexit registration,
// emitted by including <fstream> (header); here they come out as $E19/$E18 (numbering gap, see .todo)


// Matched by this code:
// 0x1000e5c0 _$E19
