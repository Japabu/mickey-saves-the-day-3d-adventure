// SquishMovie.h - a Squish movie: a list of frames.
// Declared first by the includer (still one copy per unit in wave 1): MBVector.
#ifndef MBENGINE_SQUISHMOVIE_H
#define MBENGINE_SQUISHMOVIE_H

class SquishFile;
class SquishImageFrame;
class SquishPalette;

class SquishMovie {
public:
    SquishMovie(void);
    ~SquishMovie(void);
    unsigned long GetWidth(void) const { return mWidth; }
    unsigned long GetHeight(void) const { return mHeight; }
    unsigned long GetDepth(void) const { return mDepth; }
    unsigned long GetNFrames(void) const { return mFrames.size(); }
    bool AddFrame(SquishImageFrame *frame);
protected:
    bool DeleteFrame(SquishImageFrame *frame, bool deleteData);
public:
    bool DeleteFrame(unsigned int index);
    SquishImageFrame *ExtractFrame(unsigned int index);
    SquishImageFrame *GetFrame(unsigned int index) const;
    void Reset(void);
    bool StreamIn(SquishFile *file);
    bool StreamOut(SquishFile *file) const;
    bool Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette);
protected:
    MBVector<SquishImageFrame *> mFrames;
    unsigned short mWidth;
    unsigned short mHeight;
    unsigned short mDepth;
};

#endif
