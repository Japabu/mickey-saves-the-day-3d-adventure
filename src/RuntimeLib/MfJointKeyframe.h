// MfJointKeyframe (MfActorLib.dll): one key of a joint track. Declaration as
// in src/MfActorLib/MfJointKeyframe.h (only what RuntimeLib uses).
#ifndef RUNTIMELIB_MFJOINTKEYFRAME_H
#define RUNTIMELIB_MFJOINTKEYFRAME_H
#include "MfKeyframe.h"
#include "MfTransform3.h"
#include "MfVector3.h"
class MfStream;

class __declspec(dllimport) MfJointKeyframe : public MfKeyframe {
public:
    MfJointKeyframe();
    MfJointKeyframe(const MfJointKeyframe &);
    virtual ~MfJointKeyframe();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual MfKeyframe *CreateCopy();

    int mFlags;                                             // 0x0c 1: the key moves an IK chain
    MfTransform3 mXform;                                    // 0x10 relative to the parent joint
    MfTransform3 mXformCtrl[2];                             // 0x2c
    MfTransform3 mWorld;                                    // 0x64 in the actor
    MfTransform3 mWorldCtrl[2];                             // 0x80
    MfVector3 mOffset;                                      // 0xb8
    int mUserData;                                          // 0xc4 the chain length
    float mBlend[2];                                        // 0xc8
    int mInterpolation;                                     // 0xd0
};

#endif
