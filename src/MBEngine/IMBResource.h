// IMBResource.h - a loaded resource with its type and ID.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBResource.
#ifndef MBENGINE_IMBRESOURCE_H
#define MBENGINE_IMBRESOURCE_H

class IMBResource : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResource; }
    virtual unsigned long GetID(void) = 0;
    virtual unsigned long GetType(void) = 0;
};

#endif
