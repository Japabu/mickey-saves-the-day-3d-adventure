// CMBMilesSoundManager.h - declarations for MBEngine.dll CMBMilesSoundManager.obj
#ifndef CMBMILESSOUNDMANAGER_H
#define CMBMILESSOUNDMANAGER_H

#include <string> // evidence: original TU has $E24/$E25 for std::ctype<unsigned short>::id
#include <list>
#include <map> // three MBMap instances (_Nil/_Nilrefs statics) account for $E18 -> $E24

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &g) const {
        return data1 == g.data1 && data2 == g.data2 && data3 == g.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)g.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)g.data4)[1];
    }
};

// header-defined interface IDs (PDB ldata 0x1008eb48..0x1008ec18)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBPeriodical =
    { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager =
    { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener =
    { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager =
    { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundDescription =
    { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinSoundManager =
    { 0x8455df50, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile =
    { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList =
    { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceMgr =
    { 0x8991fec0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);
extern "C" IMBUnknown *MBComUtilAddSingleton(IMBUnknown *unk);

enum IMBStreamSeekValues { kSeekDummy };
enum IMBStreamError { kStreamErrorDummy };

class IMBStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStream; }
    virtual bool Read(void *, unsigned long &) = 0;
    virtual bool Write(const void *, unsigned long) = 0;
    virtual bool Seek(long, IMBStreamSeekValues) = 0;
    virtual unsigned long GetPosition(void) = 0;
    virtual bool Flush(void) = 0;
    virtual bool SetLastError(IMBStreamError) = 0;
    virtual IMBStreamError GetLastError(void) = 0;
    virtual unsigned long GetStreamSize(void) = 0;
    virtual void Clone(IMBStream **) = 0;
};

class IMBStreamLocator;

class IMBSoundDescription : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSoundDescription; }
    virtual void SetStreamLocator(IMBStreamLocator *) = 0;
    virtual void GetStreamLocator(IMBStreamLocator **) = 0;
    virtual unsigned long GetSoundID(void) const = 0;
    virtual void SetChannel(long) = 0;
    virtual long GetChannel(void) const = 0;
    virtual void SetLoopCount(long) = 0;
    virtual long GetLoopCount(void) const = 0;
    virtual void SetStreaming(bool) = 0;
    virtual bool IsStreaming(void) const = 0;
    virtual void SetRateMultiplier(float) = 0;
    virtual float GetRateMultiplier(void) const = 0;
};

class MBSoundData {
public:
    MBSoundData(const MBSoundData &soundData);
    MBSoundData(unsigned char *soundData, unsigned long dataSize);
    ~MBSoundData(void);
    unsigned char *GetSoundData(void) const;
    unsigned long GetDataSize(void) const;
protected:
    unsigned char *mSoundData;
    unsigned long mDataSize;
};

class IMBSoundManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSoundManager; }
    virtual void StartSound(IMBSoundDescription *) = 0;
    virtual void QueueSound(IMBSoundDescription *) = 0;
    virtual void StopSound(long) = 0;
    virtual void StopChannel(long) = 0;
    virtual void StopAllSound(void) = 0;
    virtual bool IsPaused(void) = 0;
    virtual bool IsChannelPaused(long) = 0;
    virtual bool IsChannelPlaying(long) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual bool IsSoundPlaying(long) = 0;       // vftable+0x30
    virtual bool IsSoundPlaying(long, long) = 0; // vftable+0x2c
    virtual bool IsSoundPlayingOrQueued(long) = 0;
    virtual bool IsAnySoundPlaying(void) = 0;
    virtual void Pause(void) = 0;
    virtual void PauseChannel(long) = 0;
    virtual void PauseSound(long) = 0;
    virtual void Resume(void) = 0;
    virtual void ResumeChannel(long) = 0;
    virtual void ResumeSound(long) = 0;
    virtual short GetMasterVolume(void) = 0;
    virtual void SetMasterVolume(short) = 0;
    virtual short GetChannelVolume(long) = 0;
    virtual void SetChannelVolume(long, short) = 0;
    virtual float GetChannelRateMultiplier(long) = 0;
    virtual void SetChannelRateMultiplier(long, float) = 0;
    virtual MBSoundData *GetLockedSound(long) = 0;
    virtual void LockSound(long) = 0;
    virtual void UnlockSound(long) = 0;
};

class IMBPeriodical : public IMBUnknown {
public:
    virtual void Idle(void) = 0;
};

class IMBPeriodicalManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPeriodicalManager; }
    virtual void AddPeriodical(IMBPeriodical *) = 0;
    virtual void RemovePeriodical(IMBPeriodical *) = 0;
    virtual void DevoteTimeToPeriodicals(void) = 0;
};

class IMBEventListener : public IMBUnknown {
public:
    virtual void ListenToEvent(long, long, void *) = 0;
};

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual void RemoveListener(long, IMBEventListener *) = 0; // vftable+0x18
    virtual void RemoveListener(IMBEventListener *) = 0;       // vftable+0x14
};

class IMBMemoryBlock;

class IMBResourceFile : public IMBUnknown {
public:
    virtual void LoadResource(unsigned long, unsigned long, IMBMemoryBlock **) = 0;
    virtual void GetStream(unsigned long, unsigned long, IMBStream **) = 0;
};

enum MBResFileListAddLoc {
    kResFileListFront = 1,
    kResFileListRear = 2
};

class IMBResourceFileList : public IMBResourceFile {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceFileList; }
    virtual void AddFile(IMBResourceFile *, MBResFileListAddLoc, unsigned long) = 0;
    virtual void RemoveFile(IMBResourceFile *) = 0;
};

struct HWND__;

class IMBWinSoundManager : public IMBUnknown {
public:
    virtual HWND__ *GetHWND(void) = 0;
    virtual void SetHWND(HWND__ *) = 0;
};

template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p); // emitted in another object
    T *mInterface;
};

// Miles Sound System (mss32.dll)
struct _DIG_DRIVER;
struct _MDI_DRIVER;
struct _DLSDEVICE;
extern "C" {
__declspec(dllimport) long __stdcall AIL_quick_startup(long use_digital, long use_MIDI,
    unsigned long output_rate, long output_bits, long output_channels);
__declspec(dllimport) void __stdcall AIL_quick_shutdown(void);
__declspec(dllimport) void __stdcall AIL_quick_handles(_DIG_DRIVER **pdig, _MDI_DRIVER **pmdi,
    _DLSDEVICE **pdls);
__declspec(dllimport) long __stdcall AIL_set_preference(unsigned long number, long value);
__declspec(dllimport) HWND__ *__stdcall AIL_HWND(void);
__declspec(dllimport) long __stdcall AIL_set_DirectSound_HWND(_DIG_DRIVER *dig, HWND__ *wnd);
__declspec(dllimport) void __stdcall AIL_set_digital_master_volume(_DIG_DRIVER *dig, long master_volume);
}

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

class MBSoundEngine {
public:
    MBSoundEngine(void);
    virtual ~MBSoundEngine(void);
    virtual bool LoadSound(IMBSoundDescription *) = 0;
    bool HasSound(void) const;
    void Idle(void);
    virtual void Play(void) = 0;
    virtual void Stop(void) = 0;
    bool IsPaused(void) const;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
    short GetVolume(void) const;
    void SetVolume(short volume);
    float GetRateMultiplier(void) const;
    void SetRateMultiplier(float rateMultiplier);
    bool IsFinished(void) const;
    long GetSoundID(void) const;
    virtual bool IsSoundPlaying(long) const = 0;
protected:
    virtual bool IsSoundFinished(void) const = 0;
    virtual void IdleSound(void) = 0;
    virtual void VolumeChanged(void) = 0;
    virtual void RateMultiplierChanged(void) = 0;
    virtual void DisposeSound(void);
    void SendSoundStartedMessage(bool completed);
    void SendSoundEndedMessage(bool completed);
    IMBSoundDescription *mSoundDescription;
    float mRateMultiplier;
    short mVolume;
    bool mIsPaused;
    bool mIsFinished;
};

class MBSoundChannel {
public:
    MBSoundChannel(long channelID);
    virtual ~MBSoundChannel(void);
    void StopSound(long soundID);
    void StopSound(void);
    void ClearQueue(void);
    void AddSound(IMBSoundDescription *soundDescription);
    bool IsPaused(void) const;
    void Pause(void);
    void Resume(void);
    void PauseSound(long soundID);
    void ResumeSound(long soundID);
    virtual void Idle(void);
    long GetChannelID(void) const;
    bool IsFinished(void) const;
    bool IsSoundPlayingOrQueued(long soundID);
    bool IsSoundPlaying(long soundID);
    short GetVolume(void) const;
    void SetVolume(short volume);
    void MasterVolumeChanged(short masterVolume);
    float GetRateMultiplier(void) const;
    void SetRateMultiplier(float rateMultiplier);
protected:
    void VolumeChanged(void);
    void RateMultiplierChanged(void);
    short ScaleVolume(short masterVolume) const;
    virtual MBSoundEngine *CreateSoundEngine(IMBSoundDescription *) const = 0;
    MBList<IMBSoundDescription *> mSoundQueue;
    MBSoundEngine *mSoundEngine;
    long mChannelID;
    float mRateMultiplier;
    short mVolume;
    bool mIsPaused;
    bool mIsFinished;
};

class MBMilesSoundChannel : public MBSoundChannel {
public:
    MBMilesSoundChannel(long channelID);
    virtual ~MBMilesSoundChannel(void);
protected:
    virtual MBSoundEngine *CreateSoundEngine(IMBSoundDescription *soundDescription) const;
};

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

class CMBBaseSoundManager : public IMBSoundManager, public IMBPeriodical {
public:
    CMBBaseSoundManager(void);
    virtual ~CMBBaseSoundManager(void);
    virtual void Idle(void);
    virtual void StartSound(IMBSoundDescription *soundDescription);
    virtual void QueueSound(IMBSoundDescription *soundDescription);
    virtual void StopSound(long soundID);
    virtual void StopChannel(long channelID);
    virtual void StopAllSound(void);
    virtual bool IsPaused(void);
    virtual bool IsChannelPaused(long channelID);
    virtual bool IsChannelPlaying(long channelID);
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual bool IsSoundPlaying(long soundID);
    virtual bool IsSoundPlaying(long channelID, long soundID);
    virtual bool IsSoundPlayingOrQueued(long soundID);
    virtual bool IsAnySoundPlaying(void);
    virtual void Pause(void);
    virtual void PauseChannel(long channelID);
    virtual void PauseSound(long soundID);
    virtual void Resume(void);
    virtual void ResumeChannel(long channelID);
    virtual void ResumeSound(long soundID);
    virtual short GetMasterVolume(void);
    virtual void SetMasterVolume(short volume);
    virtual short GetChannelVolume(long channelID);
    virtual void SetChannelVolume(long channelID, short volume);
    virtual float GetChannelRateMultiplier(long channelID);
    virtual void SetChannelRateMultiplier(long channelID, float rateMultiplier);
    virtual MBSoundData *GetLockedSound(long soundID);
    virtual void LockSound(long soundID);
    virtual void UnlockSound(long soundID);
protected:
    MBSoundChannel *FindSoundChannel(long channelID);
    MBSoundChannel *AddSoundChannel(long channelID);
    void DestroyAllSoundChannels(void);
    virtual MBSoundChannel *CreateSoundChannel(long) = 0;
    virtual void DestroySoundChannel(MBSoundChannel *) = 0;
    virtual MBSoundData *LoadSound(long) = 0;
    MBList<MBSoundChannel *> mSoundChannels;
    MBMap<long, short, std::less<long> > mSoundChannelVolumes;
    MBMap<long, float, std::less<long> > mSoundChannelRateMultipliers;
    MBMap<long, MBSoundData *, std::less<long> > mLockedSounds;
    short mMasterVolume;
    bool mIsPaused;
};

class CMBMilesSoundManager : public CMBBaseSoundManager, public IMBWinSoundManager, public IMBEventListener {
public:
    CMBMilesSoundManager(void);
    virtual ~CMBMilesSoundManager(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void SetMasterVolume(short volume);
    virtual HWND__ *GetHWND(void);
    virtual void SetHWND(HWND__ *hWnd);
    virtual void ListenToEvent(long channel, long event, void *eventdata);
    _DIG_DRIVER *GetDigitalDriver(void);
protected:
    virtual MBSoundChannel *CreateSoundChannel(long channelID);
    virtual void DestroySoundChannel(MBSoundChannel *soundChannel);
    virtual MBSoundData *LoadSound(long soundID);
    _DIG_DRIVER *mDigitalDriver;
    short mDeactivatedVolume;
    bool mDeactivateVolumeControl;
};

IMBUnknown *MakeCMBMilesSoundManager(void);

#endif
