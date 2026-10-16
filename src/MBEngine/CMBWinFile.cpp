// MBEngine.dll .\MBEngineRelease\CMBWinFile.obj
#include "CMBWinFile.h"

// 0x1000c8f0 ?MakeCMBWinFile@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWinFile(void)
{
    IMBUnknown *p = (IMBStream *)new CMBWinFile;
    if (p)
        p->AddRef();
    return p;
}

// 0x1000c920 ??0CMBWinFile@@QAE@XZ
CMBWinFile::CMBWinFile(void) : mFile(0), mError(0)
{
    _mRefCnt = 0;
}

// 0x1000c960 ?QueryInterface@CMBWinFile@@UAEJABUMBGUID@@PAPAX@Z
long CMBWinFile::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBStream *)this;
    if (iid == MBIID_IMBStream)
        p = (IMBStream *)this;
    if (iid == MBIID_IMBFile)
        p = (IMBFile *)this;
    if (iid == MBIID_IMBWinFile)
        p = (IMBWinFile *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000cab0 ?AddRef@CMBWinFile@@UAEKXZ
unsigned long CMBWinFile::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000cac0 ?Release@CMBWinFile@@UAEKXZ
unsigned long CMBWinFile::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1000caf0 ??1CMBWinFile@@QAE@XZ
CMBWinFile::~CMBWinFile(void)
{
    Close();
}

// 0x1000cb30 ?GetFileHandle@CMBWinFile@@UAEPAXXZ
void *CMBWinFile::GetFileHandle(void)
{
    return mFile;
}

// 0x1000cb40 ?Open@CMBWinFile@@UAE_NPBVIMBFileSpec@@K@Z
bool CMBWinFile::Open(const IMBFileSpec *spec, unsigned long flags)
{
    Close();
    unsigned long access = 0;
    if (flags & 1)
        access = GENERIC_READ;
    if (flags & 2)
        access |= GENERIC_WRITE;
    unsigned long creation;
    if (flags & 4)
        creation = OPEN_EXISTING;
    else
        creation = (flags & 8) ? CREATE_ALWAYS : OPEN_ALWAYS;
    mFile = CreateFileA(spec->GetPath().c_str(), access, FILE_SHARE_READ, 0, creation,
                        FILE_ATTRIBUTE_NORMAL, 0);
    if (mFile == INVALID_HANDLE_VALUE) {
        mFile = 0;
        SetLastError(kIMBStreamOpenError);
    }
    mSpec.CreateObject(MBIID_CMBWinFileSpec);
    mSpec->SetPath(spec->GetPath().c_str());
    return mFile != 0;
}

// 0x1000ccd0 ?SetSize@CMBWinFile@@UAE_NK@Z
bool CMBWinFile::SetSize(unsigned long size)
{
    unsigned long pos = GetPosition();
    Seek(size, kIMBStreamSeekStart);
    BOOL result = SetEndOfFile(mFile);
    Seek(pos, kIMBStreamSeekStart);
    return result == TRUE ? true : false;
}

// 0x1000cd20 ?GetFileSpec@CMBWinFile@@UAEXPAPAVIMBFileSpec@@@Z
void CMBWinFile::GetFileSpec(IMBFileSpec **fileSpec)
{
    if (mSpec)
        mSpec->AddRef();
    *fileSpec = mSpec;
}

// 0x1000cd50 ?Close@CMBWinFile@@UAE_NXZ
bool CMBWinFile::Close(void)
{
    if (mFile)
        CloseHandle(mFile);
    mFile = 0;
    return true;
}

// 0x1000cd70 ?Clone@CMBWinFile@@UAEXPAPAVIMBStream@@@Z
void CMBWinFile::Clone(IMBStream **target)
{
    *target = 0;
    CMBWinFile *clone = new CMBWinFile;
    IMBFileSpec *tempFileSpec;
    GetFileSpec(&tempFileSpec);
    MBComPtr<IMBFileSpec> fileSpec(tempFileSpec, false);
    if (clone) {
        if (clone->Open(fileSpec, 5) && clone->Seek(GetPosition(), kIMBStreamSeekStart)) {
            clone->QueryInterface(MBIID_IMBStream, (void **)target);
            return;
        }
        delete clone;
    }
}

// 0x1000ce30 ?Write@CMBWinFile@@UAE_NPBXK@Z
bool CMBWinFile::Write(const void *ptr, unsigned long nBytes)
{
    if (mFile && ptr) {
        DWORD bytesWritten;
        if (WriteFile(mFile, ptr, nBytes, &bytesWritten, 0) && bytesWritten == nBytes)
            return true;
        SetLastError(kIMBStreamWriteError);
        return false;
    }
    SetLastError(kIMBStreamWriteError);
    return false;
}

// EQUIV 0x1000ce90 ?Read@CMBWinFile@@UAE_NPAXAAK@Z: reviewed: register choice and block order only (the same three outcomes after ReadFile)
bool CMBWinFile::Read(void *ptr, unsigned long &nBytes)
{
    if (mFile && ptr) {
        DWORD bytesRead;
        BOOL ok = ReadFile(mFile, ptr, nBytes, &bytesRead, 0);
        nBytes = bytesRead;
        if (ok) {
            if (bytesRead)
                return true;
            SetLastError(kIMBStreamEOF);
            return false;
        }
    } else
        nBytes = 0;
    SetLastError(kIMBStreamReadError);
    return false;
}

// 0x1000cf00 ?Seek@CMBWinFile@@UAE_NJW4IMBStreamSeekValues@@@Z
bool CMBWinFile::Seek(long offset, IMBStreamSeekValues origin)
{
    if (!mFile) {
        SetLastError(kIMBStreamSeekError);
        return false;
    }
    int seekOrigin;
    switch (origin) {
    case kIMBStreamSeekStart:
        seekOrigin = FILE_BEGIN;
        break;
    case kIMBStreamSeekEnd:
        seekOrigin = FILE_END;
        break;
    case kIMBStreamSeekCurrent:
        seekOrigin = FILE_CURRENT;
        break;
    }
    if (SetFilePointer(mFile, offset, 0, seekOrigin) == 0xffffffff) {
        SetLastError(kIMBStreamSeekError);
        return false;
    }
    return true;
}

// 0x1000cf70 ?Flush@CMBWinFile@@UAE_NXZ
bool CMBWinFile::Flush(void)
{
    return true;
}

// 0x1000cf80 ?SetLastError@CMBWinFile@@UAE_NW4IMBStreamError@@@Z
bool CMBWinFile::SetLastError(IMBStreamError err)
{
    mError = err;
    return true;
}

// 0x1000cf90 ?GetLastError@CMBWinFile@@UAE?AW4IMBStreamError@@XZ
IMBStreamError CMBWinFile::GetLastError(void)
{
    IMBStreamError err = (IMBStreamError)mError;
    SetLastError(kIMBStreamNoError);
    return err;
}

// 0x1000cfa0 ?GetPosition@CMBWinFile@@UAEKXZ
unsigned long CMBWinFile::GetPosition(void)
{
    if (!mFile)
        return 0xffffffff;
    return SetFilePointer(mFile, 0, 0, FILE_CURRENT);
}

// 0x1000cfc0 ?GetStreamSize@CMBWinFile@@UAEKXZ
unsigned long CMBWinFile::GetStreamSize(void)
{
    unsigned long size = 0xffffffff;
    unsigned long pos = GetPosition();
    if (pos != 0xffffffff && Seek(0, kIMBStreamSeekEnd)) {
        size = GetPosition();
        Seek(pos, kIMBStreamSeekStart);
    }
    return size;
}


// Matched by this code:
// 0x10006c10 ?Release@?$MBComPtr@VIMBFileSpec@@@@QAEXXZ
// 0x1000d000 _$E20
// 0x1000d020 _$E19
// 0x1000d030 ?QueryInterface@CMBWinFile@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1000d040 ?AddRef@CMBWinFile@@W3AEKXZ
// 0x1000d050 ?Release@CMBWinFile@@W3AEKXZ
// 0x1000d060 ?QueryInterface@CMBWinFile@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1000d070 ?AddRef@CMBWinFile@@W7AEKXZ
// 0x1000d080 ?Release@CMBWinFile@@W7AEKXZ
