#pragma once
// CSBSongbirdNote.h: a note of the songbird activity, a screen poly with a bird animation
// (declaration from SteamboatApp.pdb).
#include "CMBScreenPoly.h"

class CSBSongbirdNote : public ISBSongbirdNote, public CMBScreenPoly {
public:
    CSBSongbirdNote(const CSBSongbirdNote &);
    CSBSongbirdNote();
protected:
    unsigned long _mRefCnt;                             // +0x006c size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual char GetNote();                             // vftable+0x0c
    virtual void SetNote(char note);                    // vftable+0x10
    virtual IMBAnim *GetBird();                         // vftable+0x14
    virtual void SetBird(const MBComPtr<IMBAnim> bird); // vftable+0x18
    virtual MBPoint3 GetReferencePoint();               // vftable+0x20
    virtual void SetReferencePoint(const MBPoint3 &point); // vftable+0x24
    virtual void Play();                                // vftable+0x1c
    virtual void Clear();                               // vftable+0x28
protected:
    ~CSBSongbirdNote();
    MBComPtr<IMBAnim> mBird;                            // +0x0070 size 0x4
    MBPoint3 mRefPoint;                                 // +0x0074 size 0xc
    char mNote;                                         // +0x0080 size 0x1
};

