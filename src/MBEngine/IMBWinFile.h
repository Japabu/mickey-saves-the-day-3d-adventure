// IMBWinFile.h - a file with its Windows handle.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBWinFile.
#ifndef MBENGINE_IMBWINFILE_H
#define MBENGINE_IMBWINFILE_H

class IMBWinFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBWinFile; }
    virtual void *GetFileHandle(void) = 0;
};

#endif
