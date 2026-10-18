// MfJointPath: the sampled transforms of one node of an MfActorPath
// (RuntimeLib.dll 1002ff40-10030b90).
#include "MfActorPath.h"
#include "MfStream.h"
#include <string.h>

// 0x1002ff40 ??0MfJointPath@@QAE@XZ
MfJointPath::MfJointPath()
    : mInterval(33), mNumTranslations(0), mNumRotations(0), mTranslations(0), mRotations(0)
{
}

// 0x1002ff80 ??0MfJointPath@@QAE@ABV0@@Z
MfJointPath::MfJointPath(const MfJointPath &path)
    : MfStreamObject(path), mInterval(path.mInterval), mNumTranslations(path.mNumTranslations),
      mNumRotations(path.mNumRotations)
{
    mTranslations = new MfVector3[mNumTranslations];
    memcpy(mTranslations, path.mTranslations, mNumTranslations * sizeof(MfVector3));
    mRotations = new MfRotation3[mNumRotations];
    memcpy(mRotations, path.mRotations, mNumRotations * sizeof(MfRotation3));
}

// 0x100300a0 ??1MfJointPath@@UAE@XZ
MfJointPath::~MfJointPath()
{
    if (mTranslations)
        delete [] mTranslations;
    if (mRotations)
        delete [] mRotations;
}

// 0x10030110 ?Allocate@MfJointPath@@QAEXH@Z
void MfJointPath::Allocate(int numKeys)
{
    if (mTranslations)
        delete [] mTranslations;
    if (mRotations)
        delete [] mRotations;
    mNumTranslations = mNumRotations = mNumKeys = numKeys;
    if (numKeys) {
        mTranslations = new MfVector3[numKeys];
        mRotations = new MfRotation3[numKeys];
    } else {
        mTranslations = 0;
        mRotations = 0;
    }
}

// 0x10030270 ?SetKey@MfJointPath@@QAEXHABVMfTransform3@@@Z
void MfJointPath::SetKey(int i, const MfTransform3 &xform)
{
    mTranslations[i] = xform.translation();
    mRotations[i] = xform.rotation();
}

// TODO 0x100302c0 ?Sample@MfJointPath@@QAE?AVMfTransform3@@KH@Z: (77% similar) the original calls the imported
// MfRotation3() and copy constructors where these headers expand them inline, and divides
// by mInterval with fidiv (the registers differ from there; 34% similar)
// The transform at time: the sample before it, or (with interpolate) the
// blend towards the next sample.
MfTransform3 MfJointPath::Sample(unsigned long time, int interpolate)
{
    if (time > 0 && mInterval > 0) {
        int i = time / mInterval;
        int r = i < mNumRotations ? i : mNumRotations - 1;
        int t = i < mNumTranslations ? i : mNumTranslations - 1;
        if (interpolate) {
            MfVector3 translation;
            MfRotation3 rotation;
            float fraction = (float)(time % mInterval) / mInterval;
            if (fraction != 0.0f && r != mNumRotations - 1)
                rotation.interpolate(mRotations[r], mRotations[r + 1], fraction, 0);
            else if (r < 0)
                rotation = MfRotation3::identity();
            else
                rotation = mRotations[r];
            if (fraction != 0.0f && t != mNumTranslations - 1)
                translation.interpolate(mTranslations[t], mTranslations[t + 1], fraction);
            else if (t < 0)
                translation = MfVector3(0.0f, 0.0f, 0.0f);
            else
                translation = mTranslations[t];
            return MfTransform3(rotation, translation);
        }
        return MfTransform3(mRotations[r], mTranslations[t]);
    }
    return MfTransform3(mRotations[0], mTranslations[0]);
}

// 0x10030510 ?Compact@MfJointPath@@QAEXXZ
void MfJointPath::Compact()
{
    int i;
    if (mNumTranslations) {
        MfVector3 last = mTranslations[mNumTranslations - 1];
        for (i = mNumTranslations - 2; i >= 0; i--) {
            MfVector3 d = mTranslations[i] - last;
            if (d.lengthSquared() >= 1e-8)
                break;
            mNumTranslations--;
        }
    }
    if (mNumRotations) {
        MfRotation3 last = mRotations[mNumRotations - 1];
        for (i = mNumRotations - 2; i >= 0; i--) {
            if (!(last == mRotations[i]))
                break;
            mNumRotations--;
        }
    }
}

// 0x10030640 ?Scale@MfJointPath@@QAEXABVMfVector3@@@Z
void MfJointPath::Scale(const MfVector3 &scale)
{
    for (int i = 0; i < mNumTranslations; i++)
        mTranslations[i] *= scale;
}

// 0x10030680 ?CreateJointPath@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateJointPath()
{
    return new MfJointPath;
}

// 0x100306e0 _$E7
// 0x100306f0 _$E6
static MfStreamConstructorEntry constructorEntry(MfJointPath::STREAM_TYPE, CreateJointPath);

// 0x10030710 ?StreamType@MfJointPath@@UBEJXZ
long MfJointPath::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10030720 ?ReadFromStream@MfJointPath@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJointPath::ReadFromStream(MfStream &stream)
{
    stream >> (long &)mInterval;
    if (stream.userFormat < 32) {
        stream >> mNumKeys;
        mNumTranslations = mNumKeys;
        mNumRotations = mNumKeys;
    } else
        stream >> mNumTranslations >> mNumRotations >> mNumKeys;
    mTranslations = new MfVector3[mNumTranslations];
    mRotations = new MfRotation3[mNumRotations];
    int i;
    short count;
    MfVector3 translation;
    for (i = 0; i < mNumTranslations;) {
        stream >> count;
        stream.ReadBytes(sizeof(MfVector3), &translation);
        for (int k = 0; k < count; k++)
            mTranslations[i++] = translation;
    }
    MfRotation3 rotation;
    for (i = 0; i < mNumRotations;) {
        stream >> count;
        stream.ReadBytes(sizeof(MfRotation3), &rotation);
        for (int k = 0; k < count; k++)
            mRotations[i++] = rotation;
    }
    return stream.Status();
}

// 0x10030960 ?WriteToStream@MfJointPath@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJointPath::WriteToStream(MfStream &stream) const
{
    stream << (long)mInterval << mNumTranslations << mNumRotations << mNumKeys;
    int i;
    short count;
    if (mNumTranslations > 0) {
        MfVector3 last = mTranslations[0];
        count = 1;
        for (i = 1; i < mNumTranslations; i++) {
            MfVector3 d = mTranslations[i] - last;
            if (d.lengthSquared() < 1e-8)
                count++;
            else {
                stream << count;
                stream.WriteBytes(sizeof(MfVector3), &last);
                last = mTranslations[i];
                count = 1;
            }
        }
        stream << count;
        stream.WriteBytes(sizeof(MfVector3), &last);
    }
    if (mNumRotations > 0) {
        MfRotation3 last = mRotations[0];
        count = 1;
        for (i = 1; i < mNumRotations; i++) {
            if (mRotations[i] == last)
                count++;
            else {
                stream << count;
                stream.WriteBytes(sizeof(MfRotation3), &last);
                last = mRotations[i];
                count = 1;
            }
        }
        stream << count;
        stream.WriteBytes(sizeof(MfRotation3), &last);
    }
    return stream.Status();
}
