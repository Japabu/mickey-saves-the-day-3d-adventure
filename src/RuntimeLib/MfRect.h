// MfRect (MfActorLib.dll): an axis-aligned rectangle. RuntimeLib's units
// declare no constructor (the road map's locals are not constructed, but
// have an unwind state) and read the bounds directly. As in src/MfActorLib/MfRect.h otherwise.
#ifndef RUNTIMELIB_MFRECT_H
#define RUNTIMELIB_MFRECT_H
class MfTriangleSet;

class __declspec(dllimport) MfRect {
public:
    ~MfRect() {}
    float &minx();
    float &maxx();
    float &miny();
    float &maxy();
    void projectTriSet(MfTriangleSet *);

    float mMinx;                                            // 0x00
    float mMaxx;                                            // 0x04
    float mMiny;                                            // 0x08
    float mMaxy;                                            // 0x0c
};

#endif
