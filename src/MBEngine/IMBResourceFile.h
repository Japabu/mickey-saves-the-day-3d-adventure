// IMBResourceFile.h - a resource container (zip file or directory tree).
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBResourceFile.
#ifndef MBENGINE_IMBRESOURCEFILE_H
#define MBENGINE_IMBRESOURCEFILE_H

class IMBMemoryBlock;
class IMBStream;

class IMBResourceFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceFile; }
    virtual void LoadResource(unsigned long, unsigned long, IMBMemoryBlock **) = 0;
    virtual void GetStream(unsigned long, unsigned long, IMBStream **) = 0;
};

#endif
