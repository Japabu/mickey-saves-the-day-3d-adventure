#include "CMBButtonGroup.h"
#include <algorithm>

// 0x100330d0 ??0CMBButtonGroup@@QAE@XZ
CMBButtonGroup::CMBButtonGroup() : m_iCurrentSelected(0), m_nEventChannel(0)
{
}

// 0x100331c0 ?QueryInterface@CMBButtonGroup@@UAEJABUMBGUID@@PAPAX@Z
long CMBButtonGroup::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10033260 ?AddRef@CMBButtonGroup@@UAEKXZ
unsigned long CMBButtonGroup::AddRef()
{
    return ++_mRefCnt;
}

// 0x10033280 ?Release@CMBButtonGroup@@UAEKXZ
unsigned long CMBButtonGroup::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100332b0 ??_GCMBButtonGroup@@UAEPAXI@Z

// 0x100332f0 ??1CMBButtonGroup@@UAE@XZ
CMBButtonGroup::~CMBButtonGroup()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(this);
}

// 0x100335d0 ?AddButton@CMBButtonGroup@@QAEXPAVISBAnimButton@@J@Z
void CMBButtonGroup::AddButton(ISBAnimButton *iButton, long nChannel)
{
    long channel = nChannel == -1 ? iButton->GetBroadcastChannel() : nChannel;
    m_mapEventInfo.insert(eventinfomap::value_type(iButton, SEventInfo(channel, iButton->GetClickMessage(),
        iButton->GetRolloverMessage(), iButton->GetLeaveMessage())));
    // The channel list holds one entry per button: listen to a channel with its first button.
    if (m_lstChannels.end() == std::find(m_lstChannels.begin(), m_lstChannels.end(), channel)) {
        MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
        if (eventManager)
            eventManager->AddListener(channel, this);
    }
    m_lstChannels.push_back(channel);
}

// 0x10033780 ?RemoveButton@CMBButtonGroup@@QAEXPAVISBAnimButton@@@Z
void CMBButtonGroup::RemoveButton(ISBAnimButton *iButton)
{
    eventinfomap::iterator it = m_mapEventInfo.find(iButton);
    if (m_mapEventInfo.end() != it) {
        channellist::iterator itChannel = std::find(m_lstChannels.begin(), m_lstChannels.end(), (*it).second.m_nChannel);
        if (itChannel != m_lstChannels.end()) {
            m_lstChannels.erase(itChannel);
            if (std::find(m_lstChannels.begin(), m_lstChannels.end(), (*it).second.m_nChannel) == m_lstChannels.end()) {
                MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
                if (eventManager)
                    eventManager->RemoveListener((*it).second.m_nChannel, this);
            }
        }
        m_mapEventInfo.erase(it);
    }
}

// 0x10033900 ?ListenToEvent@CMBButtonGroup@@MAEXJJPAX@Z
void CMBButtonGroup::ListenToEvent(long channel, long event, void *data)
{
    OnEvent(channel, event, (ISBAnimButton *)data);
}

// 0x10033930 ?OnEvent@CMBButtonGroup@@QAEXJJPAVISBAnimButton@@@Z
void CMBButtonGroup::OnEvent(long channel, long event, ISBAnimButton *iButton)
{
    if (m_bEnabled) {
        eventinfomap::iterator it = m_mapEventInfo.find(iButton);
        if (it != m_mapEventInfo.end() && (*it).second.m_nChannel == channel) {
            if ((*it).second.m_nClickEvent == event)
                OnClickEvent(iButton);
            else if ((*it).second.m_nRolloverEvent == event)
                OnRolloverEvent(iButton);
            else if ((*it).second.m_nLeaveEvent == event)
                OnLeaveEvent(iButton);
        }
    }
}

// 0x10033a10 ?GetSelected@CMBButtonGroup@@QAE_NAAPAVISBAnimButton@@@Z
bool CMBButtonGroup::GetSelected(ISBAnimButton *&iButton)
{
    if (m_iCurrentSelected) {
        iButton = m_iCurrentSelected;
        return true;
    }
    return false;
}

// 0x10033a40 ?Enable@CMBButtonGroup@@QAEX_N@Z
void CMBButtonGroup::Enable(bool bEnable)
{
    m_bEnabled = bEnable;
    for (eventinfomap::iterator it = m_mapEventInfo.begin(); it != m_mapEventInfo.end(); ++it) {
        if (bEnable)
            (*it).first->Enable();
        else
            (*it).first->Disable();
    }
}

// 0x10033ab0 ?ClearSelected@CMBButtonGroup@@QAEXXZ
void CMBButtonGroup::ClearSelected()
{
    if (m_iCurrentSelected) {
        m_iCurrentSelected->SetButtonState((MBButtonState)0);
        m_iCurrentSelected = 0;
    }
}

// 0x10033ae0 ?OnClickEvent@CMBButtonGroup@@MAEXPAVISBAnimButton@@@Z
void CMBButtonGroup::OnClickEvent(ISBAnimButton *iButton)
{
    if (m_iCurrentSelected && iButton != m_iCurrentSelected)
        m_iCurrentSelected->SetButtonState((MBButtonState)0);
    m_iCurrentSelected = iButton;
}

// 0x10033b30 ?OnRolloverEvent@CMBButtonGroup@@MAEXPAVISBAnimButton@@@Z
void CMBButtonGroup::OnRolloverEvent(ISBAnimButton *iButton)
{
}

// 0x10033b50 ?OnLeaveEvent@CMBButtonGroup@@MAEXPAVISBAnimButton@@@Z
void CMBButtonGroup::OnLeaveEvent(ISBAnimButton *iButton)
{
}

// The containers' template instances (emitted by the code above):
// 0x100334b0 ??1?$MBMap@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@@@QAE@XZ
// 0x10033550 ??1?$MBList@J@@QAE@XZ
// 0x10033b70 ?erase@?$list@JV?$MBAllocator@J@@@std@@QAE?AViterator@12@V312@@Z
// 0x10034c30 ?_Construct@std@@YAXPAU?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@1@ABU21@@Z
// 0x10033bc0 ?insert@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@_N@2@ABU?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@2@@Z
// 0x10033d40 ?erase@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10034500 ?find@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@QAE?AViterator@12@ABQAVISBAnimButton@@@Z
// 0x10034570 ?lower_bound@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@QAE?AViterator@12@ABQAVISBAnimButton@@@Z
// 0x10034660 ?_Insert@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@2@@Z
// 0x100349f0 ?_Inc@const_iterator@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@QAEXXZ
// 0x10034ad0 ?_Lbound@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@IBEPAU_Node@12@ABQAVISBAnimButton@@@Z
// 0x10034b40 ?_Dec@const_iterator@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@QAEXXZ
// 0x10034380 ?erase@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100345a0 ?_Erase@?$_Tree@PAVISBAnimButton@@U?$pair@QAVISBAnimButton@@USEventInfo@CMBButtonGroup@@@std@@U_Kfn@?$map@PAVISBAnimButton@@USEventInfo@CMBButtonGroup@@U?$less@PAVISBAnimButton@@@std@@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@3@U?$less@PAVISBAnimButton@@@3@V?$MBAllocator@USEventInfo@CMBButtonGroup@@@@@std@@IAEXPAU_Node@12@@Z
