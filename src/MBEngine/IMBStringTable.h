// IMBStringTable.h - localized strings by ID or name.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBStringTable, MBString, MBString16.
#ifndef MBENGINE_IMBSTRINGTABLE_H
#define MBENGINE_IMBSTRINGTABLE_H

class IMBStream;
class MBLocale;

class IMBStringTable : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStringTable; }
    // overloads in reverse declaration order (see IMBFileSpec)
    virtual bool Load(const char *) = 0;                 // vftable+0x10
    virtual bool Load(unsigned long, IMBStream *) = 0;   // vftable+0x0c
    virtual bool SetLocale(const MBLocale &) = 0;
    virtual bool GetString(unsigned long, MBString16 &) = 0;
    virtual bool GetNamedString(const MBString &, MBString16 &) = 0;
};

#endif
