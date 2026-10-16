#include "CSBSongbirdNote.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x1002a530 ?MakeCSBSongbirdNote@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBSongbirdNote()
{
    IMBUnknown *obj = (ISBSongbirdNote *)new CSBSongbirdNote;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x1002a5c0 ??0CSBSongbirdNote@@QAE@XZ
CSBSongbirdNote::CSBSongbirdNote()
{
    _mRefCnt = 0;
}

// 0x1002a630 ?QueryInterface@CSBSongbirdNote@@UAEJABUMBGUID@@PAPAX@Z
long CSBSongbirdNote::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (ISBSongbirdNote *)this;
    if (iid == MBIID_ISBSongbirdNote)
        p = (ISBSongbirdNote *)this;
    if (iid == MBIID_IMBMouseable)
        p = (IMBMouseable *)this;
    if (iid == MBIID_IMBScreenPoly)
        p = (IMBScreenPoly *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002a7e0 ?AddRef@CSBSongbirdNote@@UAEKXZ
unsigned long CSBSongbirdNote::AddRef()
{
    return ++_mRefCnt;
}

// 0x1002a800 ?Release@CSBSongbirdNote@@UAEKXZ
unsigned long CSBSongbirdNote::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002a840 ?SetNote@CSBSongbirdNote@@UAEXD@Z
void CSBSongbirdNote::SetNote(char note)
{
    mNote = note;
}

// 0x1002a860 ?GetBird@CSBSongbirdNote@@UAEPAVIMBAnim@@XZ
IMBAnim *CSBSongbirdNote::GetBird()
{
    return mBird;
}

// Adjustor thunks (emitted with the vftables):

// 0x1002a880 ?GetReferencePoint@CSBSongbirdNote@@UAE?AVMBPoint3@@XZ
MBPoint3 CSBSongbirdNote::GetReferencePoint()
{
    return mRefPoint;
}

// 0x1002a8b0 ?SetReferencePoint@CSBSongbirdNote@@UAEXABVMBPoint3@@@Z
void CSBSongbirdNote::SetReferencePoint(const MBPoint3 &point)
{
    mRefPoint = point;
}

// 0x1002a8e0 ??1CSBSongbirdNote@@IAE@XZ
CSBSongbirdNote::~CSBSongbirdNote()
{
}

// 0x1002a980 ?Play@CSBSongbirdNote@@UAEXXZ
void CSBSongbirdNote::Play()
{
    if (mBird) {
        mBird->SetCurrentSequence(1);
        mBird->Play();
    }
}

// 0x1002a9b0 ?SetBird@CSBSongbirdNote@@UAEXV?$MBComPtr@VIMBAnim@@@@@Z
void CSBSongbirdNote::SetBird(MBComPtr<IMBAnim> bird)
{
    mBird = bird;
    if (mBird)
        mBird->Place(mRefPoint);
}

// 0x1002aa50 ?GetNote@CSBSongbirdNote@@UAEDXZ
char CSBSongbirdNote::GetNote()
{
    if (mBird)
        return mNote;
    return 'x';
}

// 0x1002aa80 ?Clear@CSBSongbirdNote@@UAEXXZ
void CSBSongbirdNote::Clear()
{
    if (mBird) {
        mBird->Stop();
        mBird = 0;
    }
    mNote = 'x';
}

// 0x1002ab40 ?QueryInterface@CSBSongbirdNote@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1002ab50 ?AddRef@CSBSongbirdNote@@W3AEKXZ
// 0x1002ab60 ?Release@CSBSongbirdNote@@W3AEKXZ
// 0x1002ab70 ?QueryInterface@CSBSongbirdNote@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1002ab80 ?AddRef@CSBSongbirdNote@@W7AEKXZ
// 0x1002ab90 ?Release@CSBSongbirdNote@@W7AEKXZ

// Emitted by this unit (template instances, inline members, deleting destructors):
// 0x1002aba0 ?QueryInterface@CSBSongbirdNote@@WM@AEJABUMBGUID@@PAPAX@Z
// 0x1002abb0 ?AddRef@CSBSongbirdNote@@WM@AEKXZ
// 0x1002abc0 ?Release@CSBSongbirdNote@@WM@AEKXZ

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x1002aac0 _$E25
// 0x1002ab00 _$E21
// 0x1002ab20 _$E24
