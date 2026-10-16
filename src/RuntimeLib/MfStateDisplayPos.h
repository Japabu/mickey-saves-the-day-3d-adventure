// MfStateDisplayPos, MfFSMCameraSettings: where the editor shows a part of a
// behaviour ('display_pos{x y w h}', 'camera_setting{...}'). RTTI names;
// vtables 100b609c and 100b5a6c; units 10085320-10085560 and (with
// MfFiniteStateMachine) 1007f970-1007fc60.
#ifndef RUNTIMELIB_MFSTATEDISPLAYPOS_H
#define RUNTIMELIB_MFSTATEDISPLAYPOS_H
#include "MfStateProperty.h"

class MfStateDisplayPos : public MfStateProperty {
public:
    enum { TYPE = 0x6a };

    MfStateDisplayPos(double x, double y, double width, double height);
    virtual ~MfStateDisplayPos();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);

    double mX, mY, mWidth, mHeight;                                             // 0x28
};

class MfFSMCameraSettings : public MfStateProperty {
public:
    enum { TYPE = 0x74 };

    MfFSMCameraSettings(double a, double b, double c, double d);
    virtual ~MfFSMCameraSettings();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);

    double mA, mB, mC, mD;                                                      // 0x28
};

// the playback setting of a behaviour ('playback{n}'; vtable 100b6d34)
class MfFSMPlayBack : public MfStateProperty {
public:
    enum { TYPE = 0x75 };

    MfFSMPlayBack(int playback);
    virtual ~MfFSMPlayBack();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);

    int mPlayback;                                                              // 0x28
};

#endif
