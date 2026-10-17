// IMBResourceFileList.h - the list of resource files searched in order.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBResourceFileList.
#ifndef MBENGINE_IMBRESOURCEFILELIST_H
#define MBENGINE_IMBRESOURCEFILELIST_H

#include "IMBResourceFile.h"

#include "MBResFileListAddLoc.h"

class IMBResourceFileList : public IMBResourceFile {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceFileList; }
    virtual void AddFile(IMBResourceFile *, MBResFileListAddLoc, unsigned long) = 0;
    virtual void RemoveFile(IMBResourceFile *) = 0;
};

#endif
