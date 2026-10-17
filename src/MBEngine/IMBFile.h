// IMBFile.h - a file opened from an IMBFileSpec.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBFile.
#ifndef MBENGINE_IMBFILE_H
#define MBENGINE_IMBFILE_H

class IMBFileSpec;

class IMBFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFile; }
    virtual bool Open(const IMBFileSpec *, unsigned long) = 0;
    virtual void GetFileSpec(IMBFileSpec **) = 0;
    virtual bool Close(void) = 0;
    virtual bool SetSize(unsigned long) = 0;
};

#endif
