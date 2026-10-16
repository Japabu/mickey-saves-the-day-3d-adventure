#pragma once
// CMBButtonGroup.h: a radio group of animated buttons; listens to their events and keeps one
// selected (MBButtonGroup.obj, declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"

class CMBButtonGroup : public IMBEventListener {
protected:
    unsigned long _mRefCnt;                             // +0x0004 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    struct SEventInfo {
        SEventInfo(long nChannel, long nClickEvent, long nRolloverEvent, long nLeaveEvent)
            : m_nChannel(nChannel), m_nClickEvent(nClickEvent), m_nRolloverEvent(nRolloverEvent),
              m_nLeaveEvent(nLeaveEvent)
        {
        }
        SEventInfo() : m_nChannel(-1), m_nClickEvent(-1), m_nRolloverEvent(-1), m_nLeaveEvent(-1) {}
        long m_nChannel;                                // +0x0000 size 0x4
        long m_nClickEvent;                             // +0x0004 size 0x4
        long m_nRolloverEvent;                          // +0x0008 size 0x4
        long m_nLeaveEvent;                             // +0x000c size 0x4
    };
    typedef MBMap<ISBAnimButton *, SEventInfo, std::less<ISBAnimButton *> > eventinfomap;
    typedef MBList<long> channellist;
    CMBButtonGroup(const CMBButtonGroup &);
    CMBButtonGroup();
    virtual ~CMBButtonGroup();                          // deleting destructor at vftable+0x10
    void AddButton(ISBAnimButton *iButton, long nChannel);
    void RemoveButton(ISBAnimButton *iButton);
    bool GetSelected(ISBAnimButton *&iButton);
    void Enable(bool bEnable);
    void ClearSelected();
    void OnEvent(long channel, long event, ISBAnimButton *iButton);
protected:
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c
    virtual void OnClickEvent(ISBAnimButton *iButton);  // vftable+0x14
    virtual void OnRolloverEvent(ISBAnimButton *iButton); // vftable+0x18
    virtual void OnLeaveEvent(ISBAnimButton *iButton);  // vftable+0x1c
    eventinfomap m_mapEventInfo;                        // +0x0008 size 0x10
    channellist m_lstChannels;                          // +0x0018 size 0xc
    ISBAnimButton *m_iCurrentSelected;                  // +0x0024 size 0x4
    long m_nEventChannel;                               // +0x0028 size 0x4
    bool m_bEnabled;                                    // +0x002c size 0x1
};
