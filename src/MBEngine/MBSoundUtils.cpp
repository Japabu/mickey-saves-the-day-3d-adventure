// MBEngine.dll MBSoundUtils.obj
#include "MBSoundUtils.h"

// 0x10047ae0 _MBPlaySound
void MBPlaySound(const char *soundName, long loopCount)
{
    MBPlaySoundID(MBHashString(soundName), loopCount);
}

// 0x10047b00 _MBForceSound
void MBForceSound(const char *soundName, long channelID, long loopCount)
{
    MBForceSoundID(MBHashString(soundName), channelID, loopCount);
}

// 0x10047b30 _MBQueueSound
void MBQueueSound(const char *soundName, long channelID, long loopCount)
{
    MBQueueSoundID(MBHashString(soundName), channelID, loopCount);
}

// 0x10047b60 _MBPlaySoundID
void MBPlaySoundID(unsigned long id, long loopCount)
{
    MBComPtr<IMBResourceStreamLocator> streamLocator(MBIID_CMBResourceStreamLocator);
    if (streamLocator) {
        streamLocator->SetResource(id, 'wav ');
        MBComPtr<IMBSoundDescription> soundDescription(MBIID_CMBSoundDescription);
        soundDescription->SetStreamLocator(streamLocator);
        soundDescription->SetLoopCount(loopCount);
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            soundManager->StartSound(soundDescription);
        soundManager.Release();
    }
}

// 0x10047c70 _MBForceSoundID
void MBForceSoundID(unsigned long id, long channelID, long loopCount)
{
    MBComPtr<IMBResourceStreamLocator> streamLocator(MBIID_CMBResourceStreamLocator);
    if (streamLocator) {
        streamLocator->SetResource(id, 'wav ');
        MBComPtr<IMBSoundDescription> soundDescription(MBIID_CMBSoundDescription);
        soundDescription->SetStreamLocator(streamLocator);
        soundDescription->SetChannel(channelID);
        soundDescription->SetLoopCount(loopCount);
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            soundManager->StartSound(soundDescription);
        soundManager.Release();
    }
}

// 0x10047d60 _MBQueueSoundID
void MBQueueSoundID(unsigned long id, long channelID, long loopCount)
{
    MBComPtr<IMBResourceStreamLocator> streamLocator(MBIID_CMBResourceStreamLocator);
    if (streamLocator) {
        streamLocator->SetResource(id, 'wav ');
        MBComPtr<IMBSoundDescription> soundDescription(MBIID_CMBSoundDescription);
        soundDescription->SetStreamLocator(streamLocator);
        soundDescription->SetChannel(channelID);
        soundDescription->SetLoopCount(loopCount);
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            soundManager->QueueSound(soundDescription);
        soundManager.Release();
    }
}

// 0x10047e50 _MBPlaySoundFile
void MBPlaySoundFile(IMBFileSpec *fileSpec, long loopCount)
{
    MBComPtr<IMBFileStreamLocator> streamLocator(MBIID_CMBFileStreamLocator);
    if (streamLocator) {
        streamLocator->SetFile(fileSpec);
        MBComPtr<IMBSoundDescription> soundDescription(MBIID_CMBSoundDescription);
        soundDescription->SetStreamLocator(streamLocator);
        soundDescription->SetLoopCount(loopCount);
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            soundManager->StartSound(soundDescription);
        soundManager.Release();
    }
}

// 0x10047f60 _MBForceSoundFile
void MBForceSoundFile(IMBFileSpec *fileSpec, long channelID, long loopCount)
{
    MBComPtr<IMBFileStreamLocator> streamLocator(MBIID_CMBFileStreamLocator);
    if (streamLocator) {
        streamLocator->SetFile(fileSpec);
        MBComPtr<IMBSoundDescription> soundDescription(MBIID_CMBSoundDescription);
        soundDescription->SetStreamLocator(streamLocator);
        soundDescription->SetChannel(channelID);
        soundDescription->SetLoopCount(loopCount);
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            soundManager->StartSound(soundDescription);
        soundManager.Release();
    }
}

// 0x10048050 _MBQueueSoundFile
void MBQueueSoundFile(IMBFileSpec *fileSpec, long channelID, long loopCount)
{
    MBComPtr<IMBFileStreamLocator> streamLocator(MBIID_CMBFileStreamLocator);
    if (streamLocator) {
        streamLocator->SetFile(fileSpec);
        MBComPtr<IMBSoundDescription> soundDescription(MBIID_CMBSoundDescription);
        soundDescription->SetStreamLocator(streamLocator);
        soundDescription->SetChannel(channelID);
        soundDescription->SetLoopCount(loopCount);
        MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
        if (soundManager)
            soundManager->QueueSound(soundDescription);
        soundManager.Release();
    }
}

// 0x100483c0 _$E20 (original $E20,) 0x100483e0 $E19: std::ctype<unsigned short>::id initializer from <string>
// (compiled as $E19/$E18: the original header set bumps the counter once more; manifest uses --sym)


// Matched by this code:
// 0x10048140 ?CreateObject@?$MBComPtr@VIMBResourceStreamLocator@@@@QAEJABUMBGUID@@@Z
// 0x100481e0 ?Release@?$MBComPtr@VIMBSoundDescription@@@@QAEXXZ
// 0x10048200 ?CreateObject@?$MBComPtr@VIMBSoundDescription@@@@QAEJABUMBGUID@@@Z
// 0x100482a0 ?CreateObject@?$MBComPtr@VIMBFileStreamLocator@@@@QAEJABUMBGUID@@@Z
// 0x10048340 ?_QueryInterface@?$MBComPtr@VIMBSoundDescription@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100483e0 _$E19
