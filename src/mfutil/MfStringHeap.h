// Motion Factory utility library (mfutil.dll): MfStringHeap, a list of heap
// MfStrings handed out by AllocString. Reconstructed declarations (no PDB):
// layouts from the code, signatures from the export names.
#ifndef MFUTIL_MFSTRINGHEAP_H
#define MFUTIL_MFSTRINGHEAP_H

#include "MfStringList.h"

class MFUTIL_API MfStringHeap : public MfStringList {
public:
    MfStringHeap();
    virtual ~MfStringHeap();
    MfString *AllocString();
    void ReleaseString(MfString *s);
    void ReleaseAll();
};

#endif
