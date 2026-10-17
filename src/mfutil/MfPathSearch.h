// MfPathSearch (mfutil.dll 0x1000acf0-0x1000b100): looks for a file name in the
// directories of a ';'-separated path list. Reconstructed declarations (no PDB):
// layouts from the code, signatures from the export names.
#ifndef MFUTIL_MFPATHSEARCH_H
#define MFUTIL_MFPATHSEARCH_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include "MfString.h"

// size 0xc, vtable 0x10017674 (only the destructor)
class MFUTIL_API MfPathSearch {
public:
    MfPathSearch(const MfString &file, const MfString &path);
    MfPathSearch(const MfPathSearch &o);
    virtual ~MfPathSearch();
    MfPathSearch &operator=(const MfPathSearch &o)
    {
        m_file = o.m_file;
        m_path = o.m_path;
        return *this;
    }
    static int TestAccess(const MfString &file);
    MfString Search();

    MfString m_file;    // +4
    MfString m_path;    // +8
};

#endif
