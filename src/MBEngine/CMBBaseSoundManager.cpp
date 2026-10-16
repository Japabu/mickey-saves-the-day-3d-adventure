// MBEngine.dll .\MBEngineRelease\CMBBaseSoundManager.obj
#include "CMBMilesSoundManager.h"

// 0x10043b40 ??0CMBBaseSoundManager@@QAE@XZ
CMBBaseSoundManager::CMBBaseSoundManager(void)
    : mMasterVolume(0x7f), mIsPaused(false)
{
}

// 0x10043c90 ??_GCMBBaseSoundManager@@UAEPAXI@Z

// 0x10043cc0 ??1CMBBaseSoundManager@@UAE@XZ
CMBBaseSoundManager::~CMBBaseSoundManager(void)
{
}

// 0x10043e40 ?Idle@CMBBaseSoundManager@@UAEXXZ
void CMBBaseSoundManager::Idle(void)
{
    if (mIsPaused)
        return;
    MBList<MBSoundChannel *>::iterator it = mSoundChannels.begin();
    while (it != mSoundChannels.end()) {
        (*it)->Idle();
        if ((*it)->IsFinished()) {
            DestroySoundChannel(*it);
            it = mSoundChannels.erase(it);
        } else {
            ++it;
        }
    }
}

// 0x10043eb0 ?StartSound@CMBBaseSoundManager@@UAEXPAVIMBSoundDescription@@@Z
void CMBBaseSoundManager::StartSound(IMBSoundDescription *soundDescription)
{
    StopChannel(soundDescription->GetChannel());
    QueueSound(soundDescription);
}

// 0x10043ee0 ?QueueSound@CMBBaseSoundManager@@UAEXPAVIMBSoundDescription@@@Z
void CMBBaseSoundManager::QueueSound(IMBSoundDescription *soundDescription)
{
    long channelID = soundDescription->GetChannel();
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel == 0)
    {
        channel = AddSoundChannel(channelID);
    }
    if (channel != 0)
    {
        channel->AddSound(soundDescription);
    }
}

// 0x10043f20 ?StopSound@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::StopSound(long soundID)
{
    for (MBList<MBSoundChannel *>::iterator it = mSoundChannels.begin();
         it != mSoundChannels.end();
         ++it)
    {
        (*it)->StopSound(soundID);
    }
}

// 0x10043f50 ?StopChannel@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::StopChannel(long channelID)
{
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel != 0)
    {
        channel->StopSound();
        channel->ClearQueue();
    }
}

// 0x10043f80 ?StopAllSound@CMBBaseSoundManager@@UAEXXZ
void CMBBaseSoundManager::StopAllSound(void)
{
    for (MBList<MBSoundChannel *>::iterator it = mSoundChannels.begin();
         it != mSoundChannels.end();
         ++it)
    {
        (*it)->StopSound();
        (*it)->ClearQueue();
    }
}

// 0x10043fb0 ?IsPaused@CMBBaseSoundManager@@UAE_NXZ
bool CMBBaseSoundManager::IsPaused()
{
    return mIsPaused;
}

// 0x10043fc0 ?IsChannelPaused@CMBBaseSoundManager@@UAE_NJ@Z
bool CMBBaseSoundManager::IsChannelPaused(long channelID)
{
    bool result = false;
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel)
        result = channel->IsPaused();
    return result;
}

// 0x10043ff0 ?IsChannelPlaying@CMBBaseSoundManager@@UAE_NJ@Z
bool CMBBaseSoundManager::IsChannelPlaying(long channelID)
{
    bool result = false;
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel)
    {
        if (!channel->IsPaused() && !channel->IsFinished())
            result = true;
        else
            result = false;
    }
    return result;
}

// 0x10044030 ?IsSoundPlaying@CMBBaseSoundManager@@UAE_NJ@Z
bool CMBBaseSoundManager::IsSoundPlaying(long soundID)
{
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        if (channel->IsSoundPlaying(soundID))
            return true;
    }
    return false;
}

// 0x10044070 ?IsSoundPlaying@CMBBaseSoundManager@@UAE_NJJ@Z
bool CMBBaseSoundManager::IsSoundPlaying(long channelID, long soundID)
{
    bool result = false;
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel)
        result = channel->IsSoundPlaying(soundID);
    return result;
}

// 0x100440a0 ?IsSoundPlayingOrQueued@CMBBaseSoundManager@@UAE_NJ@Z
bool CMBBaseSoundManager::IsSoundPlayingOrQueued(long soundID)
{
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        if (channel->IsSoundPlayingOrQueued(soundID))
            return true;
    }
    return false;
}

// 0x100440e0 ?IsAnySoundPlaying@CMBBaseSoundManager@@UAE_NXZ
bool CMBBaseSoundManager::IsAnySoundPlaying()
{
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        if (IsChannelPlaying(channel->GetChannelID()))
            return true;
    }
    return false;
}

// 0x10044120 ?Pause@CMBBaseSoundManager@@UAEXXZ
void CMBBaseSoundManager::Pause()
{
    mIsPaused = true;
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        channel->Pause();
    }
}

// 0x10044150 ?PauseChannel@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::PauseChannel(long channelID)
{
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel)
        channel->Pause();
}

// 0x10044170 ?PauseSound@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::PauseSound(long soundID)
{
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        channel->PauseSound(soundID);
    }
}

// 0x100441a0 ?Resume@CMBBaseSoundManager@@UAEXXZ
void CMBBaseSoundManager::Resume()
{
    mIsPaused = false;
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        channel->Resume();
    }
}

// 0x100441d0 ?ResumeChannel@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::ResumeChannel(long channelID)
{
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel)
        channel->Resume();
}

// 0x100441f0 ?ResumeSound@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::ResumeSound(long soundID)
{
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        channel->ResumeSound(soundID);
    }
}

// 0x10044220 ?GetMasterVolume@CMBBaseSoundManager@@UAEFXZ
short CMBBaseSoundManager::GetMasterVolume()
{
    return mMasterVolume;
}

// 0x10044230 ?SetMasterVolume@CMBBaseSoundManager@@UAEXF@Z
void CMBBaseSoundManager::SetMasterVolume(short volume)
{
    mMasterVolume = volume;
    for (MBList<MBSoundChannel *>::iterator i = mSoundChannels.begin(); i != mSoundChannels.end(); i++)
    {
        MBSoundChannel *channel = *i;
        channel->MasterVolumeChanged(volume);
    }
}

// 0x10044260 ?GetChannelVolume@CMBBaseSoundManager@@UAEFJ@Z
short CMBBaseSoundManager::GetChannelVolume(long channelID)
{
    short volume = 0x7f;
    MBMap<long, short, std::less<long> >::iterator it = mSoundChannelVolumes.find(channelID);
    if (it != mSoundChannelVolumes.end())
        volume = (*it).second;
    return volume;
}

// 0x100442d0 ?SetChannelVolume@CMBBaseSoundManager@@UAEXJF@Z
void CMBBaseSoundManager::SetChannelVolume(long channelID, short volume)
{
    mSoundChannelVolumes[channelID] = volume;
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel)
        channel->SetVolume(volume);
}

// 0x10044460 ?GetChannelRateMultiplier@CMBBaseSoundManager@@UAEMJ@Z
float CMBBaseSoundManager::GetChannelRateMultiplier(long channelID)
{
    float rateMultiplier = 1.0f;
    MBMap<long, float, std::less<long> >::iterator it = mSoundChannelRateMultipliers.find(channelID);
    if (it != mSoundChannelRateMultipliers.end())
        rateMultiplier = (*it).second;
    return rateMultiplier;
}

// 0x100444c0 ?SetChannelRateMultiplier@CMBBaseSoundManager@@UAEXJM@Z
void CMBBaseSoundManager::SetChannelRateMultiplier(long channelID, float rateMultiplier)
{
    mSoundChannelRateMultipliers[channelID] = rateMultiplier;
    MBSoundChannel *channel = FindSoundChannel(channelID);
    if (channel)
        channel->SetRateMultiplier(rateMultiplier);
}

// 0x10044650 ?GetLockedSound@CMBBaseSoundManager@@UAEPAVMBSoundData@@J@Z
MBSoundData *CMBBaseSoundManager::GetLockedSound(long soundID)
{
    MBSoundData *soundData = 0;
    MBMap<long, MBSoundData *, std::less<long> >::iterator it = mLockedSounds.find(soundID);
    if (it != mLockedSounds.end())
        soundData = new MBSoundData(*(*it).second);
    return soundData;
}

// 0x100446d0 ?LockSound@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::LockSound(long soundID)
{
    MBMap<long, MBSoundData *, std::less<long> >::iterator it = mLockedSounds.find(soundID);
    if (it == mLockedSounds.end()) {
        MBSoundData *soundData = LoadSound(soundID);
        if (soundData)
            mLockedSounds[soundID] = soundData;
    }
}

// 0x10044760 ?UnlockSound@CMBBaseSoundManager@@UAEXJ@Z
void CMBBaseSoundManager::UnlockSound(long soundID)
{
    MBMap<long, MBSoundData *, std::less<long> >::iterator it = mLockedSounds.find(soundID);
    if (it != mLockedSounds.end()) {
        delete (*it).second;
        mLockedSounds.erase(soundID);
    }
}

// 0x10044880 ?FindSoundChannel@CMBBaseSoundManager@@IAEPAVMBSoundChannel@@J@Z
MBSoundChannel *CMBBaseSoundManager::FindSoundChannel(long channelID)
{
    MBSoundChannel *channel = 0;
    if (channelID != -1) {
        for (MBList<MBSoundChannel *>::iterator it = mSoundChannels.begin(); it != mSoundChannels.end(); it++) {
            if ((*it)->GetChannelID() == channelID) {
                channel = *it;
                break;
            }
        }
    }
    return channel;
}

// 0x100448d0 ?AddSoundChannel@CMBBaseSoundManager@@IAEPAVMBSoundChannel@@J@Z
MBSoundChannel *CMBBaseSoundManager::AddSoundChannel(long channelID)
{
    MBSoundChannel *channel = CreateSoundChannel(channelID);
    if (channel) {
        channel->SetVolume(GetChannelVolume(channelID));
        channel->SetRateMultiplier(GetChannelRateMultiplier(channelID));
    }
    mSoundChannels.push_back(channel);
    return channel;
}

// 0x10044950 ?DestroyAllSoundChannels@CMBBaseSoundManager@@IAEXXZ
void CMBBaseSoundManager::DestroyAllSoundChannels(void)
{
    MBList<MBSoundChannel *>::iterator it = mSoundChannels.begin();
    while (it != mSoundChannels.end()) {
        DestroySoundChannel(*it);
        it = mSoundChannels.erase(it);
    }
}

// 0x100449b0 ?erase@?$list@PAVMBSoundChannel@@V?$MBAllocator@PAVMBSoundChannel@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x100449f0 ?begin@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@QAE?AViterator@12@XZ
// 0x10044a00 ?erase@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10044af0 ?begin@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@QAE?AViterator@12@XZ
// 0x10044b00 ?erase@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10044bf0 ?erase@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10044ce0 ?erase@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@QAE?AViterator@12@V312@@Z
// 0x100451d0 ?_Erase@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@IAEXPAU_Node@12@@Z
// 0x10045230 ?_Init@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@IAEXXZ
// 0x100452b0 ?erase@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@QAE?AViterator@12@V312@@Z
// 0x100457a0 ?_Erase@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@IAEXPAU_Node@12@@Z
// 0x10045800 ?insert@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@stbdbbc3da
// 0x10045aa0 ?erase@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10045f90 ?_Erase@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10045ff0 ??0?$pair@Viterator@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@_N@std@@QAE@ABViterator@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J0ed03fcf
// 0x10046010 ??0?$pair@Viterator@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@_N@std@@QAE@ABViterator@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@Jb23e1376
// 0x10046030 ?_Insert@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJF@2@@Z
// 0x10046310 ?_Lbound@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10046360 ?_Insert@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJM@2@@Z
// 0x10046640 ?_Lbound@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10046690 ?_Buynode@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x100466b0 ?_Insert@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJPAVMBSoundData@@@2@@Z
// 0x10046990 ?_Lbound@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x100469e0 ?_Lrotate@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10046a40 ?_Rrotate@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10046aa0 ?_Buynode@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10046ac0 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@QAEXXZ
// 0x10046b60 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJF@std@@U_Kfn@?$map@JFU?$less@J@std@@V?$MBAllocator@F@@@2@U?$less@J@2@V?$MBAllocator@F@@@std@@QAEXXZ
// 0x10046bf0 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@QAEXXZ
// 0x10046c90 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJM@std@@U_Kfn@?$map@JMU?$less@J@std@@V?$MBAllocator@M@@@2@U?$less@J@2@V?$MBAllocator@M@@@std@@QAEXXZ
// 0x10046d20 ?_Ubound@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10046d70 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@QAEXXZ
// 0x10046e10 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJPAVMBSoundData@@@std@@U_Kfn@?$map@JPAVMBSoundData@@U?$less@J@std@@V?$MBAllocator@PAVMBSoundData@@@@@2@U?$less@J@2@V?$MBAllocator@PAVMBSoundData@@@@@std@@QAEXXZ
// 0x10046ea0 ?_Construct@std@@YAXPAU?$pair@$$CBJF@1@ABU21@@Z
// 0x10046ec0 ?_Construct@std@@YAXPAU?$pair@$$CBJM@1@ABU21@@Z
// 0x10046ee0 ?_Construct@std@@YAXPAU?$pair@$$CBJPAVMBSoundData@@@1@ABU21@@Z
