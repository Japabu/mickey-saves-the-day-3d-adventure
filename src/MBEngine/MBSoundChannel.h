// MBSoundChannel.h - declarations for MBEngine.dll MBSoundChannel.obj
#ifndef MBSOUNDCHANNEL_H
#define MBSOUNDCHANNEL_H

#include <string> // evidence: original TU has $E18/$E19 for std::ctype<unsigned short>::id
#include <list>

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

// header-defined interface IDs (PDB ldata 0x1008f028..0x1008f058)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBSoundDescription =
    { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager =
    { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

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
    virtual long GetStream(IMBStream **) = 0;
    virtual unsigned long GetID(void) const = 0;
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
    long _QueryInterface(IMBUnknown *const &p); // 0x10018ac0, emitted in another object
    T *mInterface;
};

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
    void construct(T *p, const T &v) { new ((void *)p) T(v); }
    void destroy(T *p) { p->~T(); }
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

#endif
