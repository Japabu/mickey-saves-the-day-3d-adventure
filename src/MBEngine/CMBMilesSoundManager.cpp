// CMBMilesSoundManager.cpp - MBEngine.dll CMBMilesSoundManager.obj
#include "CMBMilesSoundManager.h"

// 0x10042d30 ?MakeCMBMilesSoundManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBMilesSoundManager(void)
{
    static IMBUnknown *instance = 0;
    if (!instance) {
        instance = (IMBSoundManager *)new CMBMilesSoundManager;
        if (instance) {
            if (((CMBMilesSoundManager *)(IMBSoundManager *)instance)->GetDigitalDriver()) {
                MBComUtilAddSingleton(instance);
            } else {
                if (instance) {
                    delete (CMBMilesSoundManager *)(IMBSoundManager *)instance;
                    instance = 0;
                }
            }
        }
    }
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x10042da0 ??0CMBMilesSoundManager@@QAE@XZ
CMBMilesSoundManager::CMBMilesSoundManager(void)
    : mDigitalDriver(0), mDeactivatedVolume(mMasterVolume), mDeactivateVolumeControl(false)
{
    _MDI_DRIVER *midiDrvr;
    _DLSDEVICE *dlsDrvr;
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-9, this);
    long result = AIL_quick_startup(1, 0, 22050, 16, 2);
    if (!result) {
        AIL_set_preference(15, 1);
        result = AIL_quick_startup(2, 0, 22050, 16, 2);
    }
    if (result)
        AIL_quick_handles(&mDigitalDriver, &midiDrvr, &dlsDrvr);
}

// 0x10042f10 ?QueryInterface@CMBMilesSoundManager@@UAEJABUMBGUID@@PAPAX@Z
long CMBMilesSoundManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBSoundManager *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (iid == MBIID_IMBSoundManager)
        p = (IMBSoundManager *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBWinSoundManager)
        p = (IMBWinSoundManager *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100430b0 ?AddRef@CMBMilesSoundManager@@UAEKXZ
unsigned long CMBMilesSoundManager::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x100430c0 ?Release@CMBMilesSoundManager@@UAEKXZ
unsigned long CMBMilesSoundManager::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100430e0 ??_GCMBMilesSoundManager@@UAEPAXI@Z (compiler-generated)

// 0x10043110 ??1CMBMilesSoundManager@@UAE@XZ
CMBMilesSoundManager::~CMBMilesSoundManager(void)
{
    DestroyAllSoundChannels();
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-9, this);
    AIL_quick_shutdown();
}

// 0x10043220 ?GetDigitalDriver@CMBMilesSoundManager@@QAEPAU_DIG_DRIVER@@XZ
_DIG_DRIVER *CMBMilesSoundManager::GetDigitalDriver(void)
{
    return mDigitalDriver;
}

// 0x10043230 ?CreateSoundChannel@CMBMilesSoundManager@@MAEPAVMBSoundChannel@@J@Z
MBSoundChannel *CMBMilesSoundManager::CreateSoundChannel(long channelID)
{
    return new MBMilesSoundChannel(channelID);
}

// 0x10043260 ?DestroySoundChannel@CMBMilesSoundManager@@MAEXPAVMBSoundChannel@@@Z
void CMBMilesSoundManager::DestroySoundChannel(MBSoundChannel *soundChannel)
{
    delete soundChannel;
}

// 0x10043280 ?LoadSound@CMBMilesSoundManager@@MAEPAVMBSoundData@@J@Z
MBSoundData *CMBMilesSoundManager::LoadSound(long soundID)
{
    MBSoundData *soundData = 0;
    MBComPtr<IMBResourceFileList> resourceMgr(MBIID_CMBResourceMgr);
    if (resourceMgr) {
        IMBStream *stream;
        resourceMgr->GetStream(soundID, 'wav ', &stream);
        if (stream) {
            unsigned long size = stream->GetStreamSize();
            unsigned char *data = (unsigned char *)MBmalloc_release(size);
            stream->Read(data, size);
            stream->Release();
            soundData = new MBSoundData(data, size);
        }
    }
    return soundData;
}

// 0x10043350 ?GetHWND@CMBMilesSoundManager@@UAEPAUHWND__@@XZ
HWND__ *CMBMilesSoundManager::GetHWND(void)
{
    return AIL_HWND();
}

// 0x10043360 ?SetHWND@CMBMilesSoundManager@@UAEXPAUHWND__@@@Z
void CMBMilesSoundManager::SetHWND(HWND__ *hWnd)
{
    AIL_set_DirectSound_HWND(mDigitalDriver, hWnd);
}

// 0x10043380 ?SetMasterVolume@CMBMilesSoundManager@@UAEXF@Z
void CMBMilesSoundManager::SetMasterVolume(short volume)
{
    mDeactivatedVolume = volume;
    if (!mDeactivateVolumeControl)
        CMBBaseSoundManager::SetMasterVolume(volume);
}

// 0x100433a0 ?ListenToEvent@CMBMilesSoundManager@@UAEXJJPAX@Z
void CMBMilesSoundManager::ListenToEvent(long channel, long event, void *eventdata)
{
    if (channel == -9) {
        if (event == 0) {
            AIL_set_digital_master_volume(mDigitalDriver, mDeactivatedVolume);
            mDeactivateVolumeControl = false;
        } else if (event == 1) {
            mDeactivatedVolume = GetMasterVolume();
            mDeactivateVolumeControl = true;
            AIL_set_digital_master_volume(mDigitalDriver, 0);
        }
    }
}

// 0x10043400 _$E25 (original $E25,) 0x10043420 $E24: std::ctype<unsigned short>::id initializer (from <string>)
// 0x10043430.. adjustor thunks (compiler-generated)


// Matched by this code:
// 0x10043420 _$E24
// 0x10043430 ?QueryInterface@CMBMilesSoundManager@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10043440 ?AddRef@CMBMilesSoundManager@@W3AEKXZ
// 0x10043450 ?Release@CMBMilesSoundManager@@W3AEKXZ
// 0x10043460 ?QueryInterface@CMBMilesSoundManager@@WEI@AEJABUMBGUID@@PAPAX@Z
// 0x10043470 ?AddRef@CMBMilesSoundManager@@WEI@AEKXZ
// 0x10043480 ?Release@CMBMilesSoundManager@@WEI@AEKXZ
// 0x10043490 ?QueryInterface@CMBMilesSoundManager@@WEM@AEJABUMBGUID@@PAPAX@Z
// 0x100434a0 ?AddRef@CMBMilesSoundManager@@WEM@AEKXZ
// 0x100434b0 ?Release@CMBMilesSoundManager@@WEM@AEKXZ
// 0x10046f00 $E25
// 0x10046f20 $E24
// 0x10053070 $E25
// 0x10053090 $E24
