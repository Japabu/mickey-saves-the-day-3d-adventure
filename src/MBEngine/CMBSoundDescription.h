// CMBSoundDescription.h - declarations for MBEngine.dll CMBSoundDescription.obj
#ifndef CMBSOUNDDESCRIPTION_H
#define CMBSOUNDDESCRIPTION_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    unsigned char operator==(const MBGUID &o) const {
        return data1 == o.data1 && data2 == o.data2 && data3 == o.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)o.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)o.data4)[1];
    }
};

static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBSoundDescription =
    { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBStream;

class IMBStreamLocator : public IMBUnknown {
public:
    virtual long GetStream(IMBStream **) = 0;
    virtual unsigned long GetID(void) const = 0;
};

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

class CMBSoundDescription : public IMBSoundDescription {
public:
    CMBSoundDescription(void);
    ~CMBSoundDescription(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void SetStreamLocator(IMBStreamLocator *streamLocator);
    virtual void GetStreamLocator(IMBStreamLocator **streamLocator);
    virtual unsigned long GetSoundID(void) const;
    virtual void SetChannel(long channel);
    virtual long GetChannel(void) const;
    virtual void SetLoopCount(long loopCount);
    virtual long GetLoopCount(void) const;
    virtual void SetStreaming(bool isStreaming);
    virtual bool IsStreaming(void) const;
    virtual void SetRateMultiplier(float mult);
    virtual float GetRateMultiplier(void) const;
protected:
    IMBStreamLocator *mStreamLocator;
    long mChannel;
    long mLoopCount;
    bool mStreaming;
    float mRateMultiplier;
};

IMBUnknown *MakeCMBSoundDescription(void);

#endif
