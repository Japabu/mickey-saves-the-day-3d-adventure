// IMBFileSpec.h - a file path and its parts.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBFileSpec, MBString.
#ifndef MBENGINE_IMBFILESPEC_H
#define MBENGINE_IMBFILESPEC_H

class IMBFileSpec : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFileSpec; }
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual void SetPath(const char *) = 0;                       // vftable+0x10
    virtual void SetPath(const IMBFileSpec *, const char *) = 0;  // vftable+0x0c
    virtual MBString GetName(void) const = 0;
    virtual MBString GetExtension(void) const = 0;
    virtual MBString GetPath(void) const = 0;
    virtual MBString GetTitle(void) const = 0;
    virtual unsigned long GetType(void) const = 0;
    virtual void GetParent(IMBFileSpec **) const = 0;
    virtual unsigned long Hash(void) const = 0;
};

#endif
