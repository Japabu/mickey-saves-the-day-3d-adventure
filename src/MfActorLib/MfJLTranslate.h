#pragma once
// MfJLTranslate: a joint limit that keeps the translation in a box.
#include "MfActorLib_api.h"
#include "MfJLimit.h"

// Translation limited to a box. size 0x1c
class MFACTOR_API MfJLTranslate : public MfJLimit {
public:
    MfJLTranslate();
    MfJLTranslate(const MfJLTranslate &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfJLTranslate() {}
    virtual MfJLimit *Duplicate() { return new MfJLTranslate(*this); }
    virtual int GetType() { return MF_JL_TRANSLATE; }
    virtual int FixTransform(MfTransform3 &);
    virtual void PivotLimit(MfRotation3 &);
    virtual void SetValue(const MfVector3 &, const MfVector3 &);
    virtual void GetValue(MfVector3 &, MfVector3 &) const;
    int CenterTransform(MfTransform3 &);
    static const long STREAM_TYPE;
protected:
    MfVector3 m_min;        // 0x04
    MfVector3 m_max;        // 0x10
};
