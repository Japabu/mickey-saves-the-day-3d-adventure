// MfVersionResource: reads the strings of a module's VERSIONINFO resource
// (the English StringTable, code page 1200 or 1252).
#ifndef MFUTIL_MFVERSIONRESOURCE_H
#define MFUTIL_MFVERSIONRESOURCE_H

#include "mfutil_api.h"

struct HINSTANCE__;

class MFUTIL_API MfVersionResource {
public:
    MfVersionResource(HINSTANCE__ *hInstance);
    MfVersionResource(unsigned char *pData);
    ~MfVersionResource();
    // The value of the string named name, its length (without the
    // terminator) in *pLength; 0 if there is no such string.
    unsigned short *GetString(unsigned short *name, unsigned long *pLength);
private:
    void BindToAddress(void *pData);

    void *pVersionData;     // +0  the VS_VERSIONINFO block
    unsigned char *pFirst;  // +4  first String of the StringTable
    unsigned char *pEnd;    // +8  end of the StringTable
};

#endif
