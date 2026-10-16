// PORT: the VS_FIXEDFILEINFO signature scan steps in 32-bit DWORDs (unsigned long is 8 bytes here).
// MfVersionResource (mfutil.dll 0x10011470-0x100116e0).
#include <stdint.h>
#include <windows.h>
#include <string.h>
#include "MfVersionResource.h"

// A version resource block: WORD wLength, WORD wValueLength, WORD wType, WCHAR
// szKey[], padding to a DWORD, the value, then the child blocks.
#define BLOCK_LENGTH(p)     (*(unsigned short *)(p))
#define BLOCK_VALUELENGTH(p) (((unsigned short *)(p))[1])
#define BLOCK_KEY(p)        ((unsigned short *)((unsigned char *)(p) + 6))
#define BLOCK_END(p)        ((unsigned char *)(p) + BLOCK_LENGTH(p) + 2)

// 0x10011470 ??0MfVersionResource@@QAE@PAUHINSTANCE__@@@Z
MfVersionResource::MfVersionResource(HINSTANCE__ *hInstance)
{
    pVersionData = 0;
    pFirst = 0;
    pEnd = 0;
    HRSRC hRes = FindResourceA(hInstance, MAKEINTRESOURCE(1), RT_VERSION);
    if (hRes) {
        HGLOBAL hData = LoadResource(hInstance, hRes);
        if (hData)
            BindToAddress(LockResource(hData));
    }
}

// 0x100114c0 ??0MfVersionResource@@QAE@PAE@Z
MfVersionResource::MfVersionResource(unsigned char *pData)
{
    pVersionData = 0;
    pFirst = 0;
    pEnd = 0;
    BindToAddress(pData);
}

// 0x100114e0 ??1MfVersionResource@@QAE@XZ
MfVersionResource::~MfVersionResource()
{
}

// EQUIV 0x100114f0 ?BindToAddress@MfVersionResource@@AAEXPAX@Z: reviewed: register allocation: wcslen cached in a register, pChild spilled; same calls, arguments and loop structure
void MfVersionResource::BindToAddress(void *pData)
{
    pVersionData = pData;
    if (!pData)
        return;

    // the children follow the VS_FIXEDFILEINFO value
    DWORD *pFixed = (DWORD *)pData;   // PORT: 32-bit words, as in the resource
    while (*pFixed != VS_FFI_SIGNATURE)
        pFixed++;
    unsigned char *pChild = (unsigned char *)pFixed + BLOCK_VALUELENGTH(pData);
    while ((uintptr_t)pChild & 3)
        pChild++;

    for (; pChild < BLOCK_END(pData); pChild = BLOCK_END(pChild)) {
        if (_wcsicmp(L"StringFileInfo", BLOCK_KEY(pChild)) != 0)
            continue;
        unsigned char *pTable = (unsigned char *)BLOCK_KEY(pChild) + (wcslen(BLOCK_KEY(pChild)) + 1) * 2;
        while ((uintptr_t)pTable & 3)
            pTable++;
        for (; pTable < BLOCK_END(pChild); pTable = BLOCK_END(pTable)) {
            if (_wcsicmp(L"040904b0", BLOCK_KEY(pTable)) == 0 ||
                _wcsicmp(L"040904e4", BLOCK_KEY(pTable)) == 0) {
                unsigned char *pString = (unsigned char *)BLOCK_KEY(pTable) + (wcslen(BLOCK_KEY(pTable)) + 1) * 2;
                while ((uintptr_t)pString & 3)
                    pString++;
                pFirst = pString;
                pEnd = BLOCK_END(pTable);
                return;
            }
        }
    }
}

// EQUIV 0x10011620 ?GetString@MfVersionResource@@QAEPAGPAGPAK@Z: reviewed: the value length is read into si instead of cx
unsigned short *MfVersionResource::GetString(unsigned short *name, unsigned int *pLength)
{
    *pLength = 0;
    unsigned char *pString = pFirst;
    if (pString) {
        while (pString < pEnd) {
            if (_wcsicmp(name, BLOCK_KEY(pString)) == 0) {
                *pLength = BLOCK_VALUELENGTH(pString) - 1;
                if (BLOCK_VALUELENGTH(pString) <= 0)
                    return 0;
                unsigned char *pValue = (unsigned char *)BLOCK_KEY(pString) + (wcslen(BLOCK_KEY(pString)) + 1) * 2;
                while ((uintptr_t)pValue & 3)
                    pValue++;
                return (unsigned short *)pValue;
            }
            unsigned char *pNext = (unsigned char *)BLOCK_KEY(pString) + (wcslen(BLOCK_KEY(pString)) + 1) * 2;
            while ((uintptr_t)pNext & 3)
                pNext++;
            if (BLOCK_VALUELENGTH(pString) > 0) {
                pNext += BLOCK_VALUELENGTH(pString) * 2;
                while ((uintptr_t)pNext & 3)
                    pNext++;
            }
            pString = pNext;
        }
    }
    return 0;
}
