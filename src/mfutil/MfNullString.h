// MfNullString: the empty-string token (NULL_STRING) that the MfString operators
// compare with and assign (mfutil.dll). No data; the constructor and destructor are
// inline and empty (mfutil exports them).
#ifndef MFUTIL_MFNULLSTRING_H
#define MFUTIL_MFNULLSTRING_H

#include "mfutil_api.h"

class MFUTIL_API MfNullString {
public:
    MfNullString() {}
    ~MfNullString() {}
};

extern MFUTIL_API MfNullString NULL_STRING;

#endif
