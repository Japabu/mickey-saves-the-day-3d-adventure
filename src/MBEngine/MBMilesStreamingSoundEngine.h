// MBMilesStreamingSoundEngine.h - declarations for MBEngine.dll MBMilesStreamingSoundEngine.obj
#ifndef MBMILESSTREAMINGSOUNDENGINE_H
#define MBMILESSTREAMINGSOUNDENGINE_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBStreamLocator;

class IMBSoundDescription : public IMBUnknown {
public:
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

// Miles Sound System (mss32.dll)
struct _STREAM;
typedef _STREAM *HSTREAM;
extern "C" {
__declspec(dllimport) void __stdcall AIL_set_stream_loop_count(HSTREAM stream, long count);
__declspec(dllimport) void __stdcall AIL_start_stream(HSTREAM stream);
__declspec(dllimport) void __stdcall AIL_pause_stream(HSTREAM stream, long onoff);
__declspec(dllimport) long __stdcall AIL_stream_status(HSTREAM stream);
__declspec(dllimport) void __stdcall AIL_set_stream_volume(HSTREAM stream, long volume);
__declspec(dllimport) void __stdcall AIL_close_stream(HSTREAM stream);
}

class MBFileStream;

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
    IMBSoundDescription *mSoundDescription;
    float mRateMultiplier;
    short mVolume;
    bool mIsPaused;
    bool mIsFinished;
};

class MBMilesStreamingSoundEngine : public MBSoundEngine {
public:
    MBMilesStreamingSoundEngine(void);
    virtual ~MBMilesStreamingSoundEngine(void);
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
    _STREAM *mMilesStream;
    MBFileStream *mFileStream;
    long mResumeLoopCount;
    unsigned long mResumeTimePosition;
};

#endif
