// IMBMovie.h - a movie player (Bink, Smacker).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBMovie.
#ifndef MBENGINE_IMBMOVIE_H
#define MBENGINE_IMBMOVIE_H

class IMBStream;
class MBColor;
class MBPalette;

class IMBMovie : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMovie; }
    virtual bool Load(unsigned long, IMBStream *, unsigned long) = 0;
    virtual void Play(void) = 0;
    virtual void Stop(void) = 0;
    virtual bool IsPlaying(void) = 0;
    virtual void Rewind(void) = 0;
    virtual long Pause(void) = 0;
    virtual long Unpause(void) = 0;
    virtual bool IsPaused(void) = 0;
    virtual void SetLoop(bool) = 0;
    virtual bool IsLooping(void) = 0;
    virtual long CurrentFrameNum(void) const = 0;
    virtual long NumTotalFrames(void) const = 0;
    virtual long FrameDelay(void) const = 0;
    virtual long Width(void) const = 0;
    virtual long Height(void) const = 0;
    virtual long Depth(void) const = 0;
    virtual void SetTransparentColor(MBColor) = 0;
    virtual MBColor GetTransparentColor(void) = 0;
    virtual void SetPalette(MBPalette *) = 0;
    virtual MBPalette *GetPalette(void) = 0;
};

#endif
