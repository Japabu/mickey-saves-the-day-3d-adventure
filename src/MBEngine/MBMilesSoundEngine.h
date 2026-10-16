// MBMilesSoundEngine.h - declarations for MBEngine.dll MBMilesSoundEngine.obj
#ifndef MBMILESSOUNDENGINE_H
#define MBMILESSOUNDENGINE_H

#include <string> // evidence: original TU has $E19/$E20 for std::ctype<unsigned short>::id

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

// header-defined interface IDs (PDB ldata 0x1008ed38..0x1008ed88)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBSoundDescription =
    { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager =
    { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStreamLocator =
    { 0x89cc0fe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

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

class IMBStreamLocator : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStreamLocator; }
    virtual long GetStream(IMBStream **) = 0;
    virtual unsigned long GetID(void) const = 0;
    // stand-in: the original's ctype id $E20/$E19 are one past the <string> baseline ($E19/$E18);
    // the obj's PDB data has no guard/static, and every STL unit including the IMBStreamLocator
    // header has the +1, so one static data member declaration there takes it (name unknown;
    // never defined or used)
    static const unsigned long sUnknownStreamLocatorStatic;
};

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

template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) {
        if (mInterface) mInterface->Release();
        mInterface = 0;
    }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    void Release(void); // 0x10018a60, emitted in another object
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
    long _QueryInterface(IMBUnknown *const &p); // 0x10018ac0, emitted in another object
    T *mInterface;
};

// Miles Sound System (mss32.dll)
struct _SAMPLE;
typedef _SAMPLE *HSAMPLE;
struct _DLSFILEID;
struct AUDIO_TYPE {
    const unsigned long *data;
    long size;
    long type;
    void *handle;
    long status;
    void *next;
    long speed;
    long volume;
    long extravol;
    float rlevel;
    float rrtime;
    float rdtime;
    _DLSFILEID *dlsid;
    void *dlsmem;
    void *dlsmemunc;
    long milliseconds;
    long length;
    long userdata;
};
typedef AUDIO_TYPE *HAUDIO;
extern "C" {
__declspec(dllimport) HAUDIO __stdcall AIL_quick_load_mem(const void *mem, unsigned long size);
__declspec(dllimport) long __stdcall AIL_quick_play(HAUDIO audio, unsigned long loop_count);
__declspec(dllimport) void __stdcall AIL_quick_halt(HAUDIO audio);
__declspec(dllimport) long __stdcall AIL_quick_status(HAUDIO audio);
__declspec(dllimport) void __stdcall AIL_quick_unload(HAUDIO audio);
__declspec(dllimport) void __stdcall AIL_quick_set_volume(HAUDIO audio, long volume, long extravol);
__declspec(dllimport) void __stdcall AIL_quick_set_speed(HAUDIO audio, long speed);
__declspec(dllimport) long __stdcall AIL_quick_ms_position(HAUDIO audio);
__declspec(dllimport) void __stdcall AIL_quick_set_ms_position(HAUDIO audio, long milliseconds);
__declspec(dllimport) unsigned long __stdcall AIL_sample_playback_rate(HSAMPLE S);
__declspec(dllimport) long __stdcall AIL_sample_loop_count(HSAMPLE S);
}

class MBSoundEngine {
public:
    MBSoundEngine(void);
    virtual ~MBSoundEngine(void);
    virtual bool LoadSound(IMBSoundDescription *) = 0;
    virtual void Play(void) = 0;
    virtual void Stop(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
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

class MBMilesSoundEngine : public MBSoundEngine {
public:
    MBMilesSoundEngine(void);
    virtual ~MBMilesSoundEngine(void);
    virtual bool LoadSound(IMBSoundDescription *soundDescription);
    virtual void Play(void);
    virtual void Stop(void);
    virtual void Pause(void);
    virtual void Resume(void);
    virtual bool IsSoundPlaying(long soundID) const;
protected:
    virtual bool IsSoundFinished(void) const;
    virtual void IdleSound(void);
    virtual void VolumeChanged(void);
    virtual void RateMultiplierChanged(void);
    virtual void DisposeSound(void);
    MBSoundData *mSoundData;
    AUDIO_TYPE *mMilesAudio;
    long mResumeLoopCount;
    long mResumeTimePosition;
};

#endif
