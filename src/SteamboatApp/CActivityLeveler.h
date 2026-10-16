#pragma once
// CActivityLeveler.h: the level selector of the activities (two buttons and a level display;
// ActivityLeveler.obj, declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"

class CActivityLeveler : public IMBEventListener {
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    enum state {
        kIdle = 0,
        kStartChange = 1,
        kEndChange = 2
    };
    struct exception {
        exception(long line, long error) : nLine(line), nError(error) {}
        long nLine;                                     // +0x0000 size 0x4
        long nError;                                    // +0x0004 size 0x4
    };
    CActivityLeveler(const CActivityLeveler &);
    CActivityLeveler(long nChannel, long nEvent);
    virtual ~CActivityLeveler();                        // deleting destructor at vftable+0x10
    long Create(MBPoint3 ptOrigin, long nStartingLevel);
    void Destroy();
    void Place(const MBPoint3 &ptOrigin);
    void SetLevel(long nLevel);
    void SetLevelRange(long nMinLevel, long nMaxLevel);
    long GetLevel();
    long GetMinLevel() { return m_nMinLevel; }
    long GetMaxLevel() { return m_nMaxLevel; }
protected:
    void Place();
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c
    void ShowButtons(bool bShow);
    void OnNextLevel();
    void OnPrevLevel();
    void OnLevelChange();
    void OnLevelChangeComplete();
    bool IsActivityLevelerAnim(IMBAnim *pAnim);
    void ChangeState(state eState);
private:
    long m_nCurrentLevel;                               // +0x0008 size 0x4
    long m_nLastLevel;                                  // +0x000c size 0x4
    long m_nMinLevel;                                   // +0x0010 size 0x4
    long m_nMaxLevel;                                   // +0x0014 size 0x4
    state m_eState;                                     // +0x0018 size 0x4
    IMBAnim *m_arrayLevelAnims[10];                     // +0x001c size 0x28
    ISBAnimButton *m_pLevelUpButton;                    // +0x0044 size 0x4
    ISBAnimButton *m_pLevelDownButton;                  // +0x0048 size 0x4
    MBPoint3 m_ptOrigin;                                // +0x004c size 0xc
    long m_nChannel;                                    // +0x0058 size 0x4
    long m_nEvent;                                      // +0x005c size 0x4
};
