// Utilities/StdString.h: Joe O'Leary's CStdString, an MFC CString work-alike
// built on std::basic_string.  The template is renamed _SS to keep the mangled
// names of its instances short (warning 4786); member list from the PDB.
#pragma once
#include <windows.h>
#include <string>
#include <stdarg.h>
#include <stdio.h>

#define CStdStr _SS

// Without MFC, TRACE(...) is a discarded comma expression (its arguments are
// still evaluated, hence the GetLastError calls in Load).
#ifndef TRACE
#define TRACE
#endif

typedef const char *PCSTR;
typedef char *PSTR;

// Length of a string that may be null.
inline int sslen(PCSTR pA) { return pA == 0 ? 0 : strlen(pA); }

// Assignment that copes with the source pointing into the destination.
inline void ssasn(std::string &sDst, const std::string &sSrc)
{
    if (sDst.c_str() != sSrc.c_str()) {
        sDst.erase();
        sDst.assign(sSrc.c_str());
    }
}

inline void ssasn(std::string &sDst, PCSTR pA)
{
    if (pA == 0)
        sDst.erase();
    else if (pA >= sDst.c_str() && pA <= sDst.c_str() + sDst.size())
        sDst = sDst.substr(static_cast<std::string::size_type>(pA - sDst.c_str()));
    else
        sDst.assign(pA);
}

// Append that ignores a null pointer.
inline void ssadd(std::string &sDst, PCSTR pA)
{
    if (pA != 0)
        sDst.append(pA);
}

// vsprintf with a buffer size for the character type.
inline int ssvsprintf(PSTR pA, size_t nCount, PCSTR pFmtA, va_list vl)
{
    return _vsnprintf(pA, nCount, pFmtA, vl);
}

// Format's buffers: two on the stack, then up to MAX_FMT_TRIES heap sizes
// growing by FMT_BLOCK_SIZE.
#define BUFSIZE_1ST 256
#define BUFSIZE_2ND 512
#define MAX_FMT_TRIES 4
#define FMT_BLOCK_SIZE 2048

// LoadString for the character type.
inline int ssload(HMODULE hInst, UINT uId, PSTR pBuf, int nMax)
{
    return ::LoadStringA(hInst, uId, pBuf, nMax);
}

// The module string resources load from.
inline HMODULE &SSResourceHandle()
{
    static HMODULE hModuleSS = GetModuleHandle(0);
    return hModuleSS;
}

template <typename CT>
class CStdStr : public std::basic_string<CT> {
    typedef std::basic_string<CT> MYBASE;
    typedef CStdStr<CT> MYTYPE;
    typedef const CT *PCMYSTR;
    typedef CT *PMYSTR;
    typedef MYBASE::iterator MYITER;
    typedef MYBASE::const_iterator MYCITER;
    typedef MYBASE::size_type MYSIZE;
    typedef MYBASE::value_type MYVAL;
    typedef MYBASE::allocator_type MYALLOC;

public:
    CStdStr()
    {
    }

    CStdStr(const MYTYPE &str) : MYBASE(str.c_str())
    {
    }

    CStdStr(const std::string &str)
    {
        ssasn(*this, str.c_str());
    }

    CStdStr(PCSTR pA)
    {
        if (0 != HIWORD(pA))
            *this = pA;
        else if (0 != pA && !Load(LOWORD(pA)))
            ;   // TRACE("Can't load string %u\n", LOWORD(pA))
    }

    MYTYPE &operator=(const MYTYPE &str)
    {
        ssasn(*this, str);
        return *this;
    }

    MYTYPE &operator=(const std::string &str)
    {
        ssasn(*this, str);
        return *this;
    }

    MYTYPE &operator=(PCSTR pA)
    {
        ssasn(*this, pA);
        return *this;
    }

    MYTYPE &operator+=(const MYTYPE &str)
    {
        append(str.c_str());
        return *this;
    }

    MYTYPE &operator+=(PCSTR pA)
    {
        ssadd(*this, pA);
        return *this;
    }

    // Loads string resource nId of GetResourceHandle().
    bool Load(UINT nId)
    {
        bool bLoaded = false;   // set to true if we succeed
        PCMYSTR szName = MAKEINTRESOURCE((nId >> 4) + 1);
        HRSRC hrsrc;
        DWORD dwSize;
        HMODULE hModule = GetResourceHandle();
        if (0 == (hrsrc = ::FindResource(hModule, szName, RT_STRING)))
            TRACE("Cannot find resource %d: 0x%X", nId, ::GetLastError());
        else if (0 == (dwSize = ::SizeofResource(hModule, hrsrc) / sizeof(CT)))
            TRACE("Can't get size of resource %d 0x%X\n", nId, ::GetLastError());
        else {
            bLoaded = 0 != ssload(hModule, nId, GetBuffer(dwSize), dwSize);
            ReleaseBuffer();
        }
        if (!bLoaded)
            TRACE("String not loaded 0x%X\n", ::GetLastError());
        return bLoaded;
    }

    CT *Buffer(int nMinLen = -1)
    {
        if (nMinLen > -1)
            resize(static_cast<MYSIZE>(nMinLen));
        return empty() ? const_cast<CT *>(data()) : &(at(0));
    }
    void BufferRel(int nNewLen = -1)
    {
        resize(static_cast<MYSIZE>(nNewLen > -1 ? nNewLen : sslen(c_str())));
    }

    CT *GetBuffer(int nMinLen = -1) { return Buffer(nMinLen); }
    void ReleaseBuffer(int nNewLen = -1) { BufferRel(nNewLen); }

    void Format(PCMYSTR szFormat, ...)
    {
        va_list argList;
        va_start(argList, szFormat);
        FormatV(szFormat, argList);
        va_end(argList);
    }

    void FormatV(PCMYSTR szFormat, va_list argList)
    {
        va_list argListSave = argList;
        int nChars = 0;
        int nTry = MAX_FMT_TRIES;
        CT szBuf1[BUFSIZE_1ST];
        CT szBuf2[BUFSIZE_2ND];

        if ((nChars = ssvsprintf(szBuf1, BUFSIZE_1ST - 1, szFormat, argList)) >= 0) {
            szBuf1[nChars] = '\0';
            ssasn(*this, szBuf1);
        } else if ((nChars = ssvsprintf(szBuf2, BUFSIZE_2ND - 1, szFormat, argList)) >= 0) {
            szBuf2[nChars] = '\0';
            ssasn(*this, szBuf2);
        } else {
            int nLen = BUFSIZE_2ND + 1;
            do {
                nLen += FMT_BLOCK_SIZE;
                nChars = ssvsprintf(GetBuffer(nLen), nLen - 1, szFormat, argListSave);
                if (nChars >= 0) {
                    ReleaseBuffer(nChars);
                    break;
                }
            } while (--nTry > 0);
        }
    }

    int Find(PCMYSTR szSub) const
    {
        return static_cast<int>(find(szSub));
    }

    int ReverseFind(CT ch) const
    {
        MYSIZE nIdx = rfind(ch);
        return static_cast<int>(MYBASE::npos == nIdx ? -1 : nIdx);
    }

    MYTYPE Left(int nCount) const
    {
        return substr(0, static_cast<MYSIZE>(nCount));
    }
    MYTYPE Right(int nCount) const
    {
        nCount = std::_cpp_min(nCount, static_cast<int>(size()));
        return substr(size() - static_cast<MYSIZE>(nCount));
    }

    int GetLength() const { return static_cast<int>(length()); }
    bool IsEmpty() const { return empty(); }
    void Empty() { erase(); }

    operator PCMYSTR() const { return c_str(); }

    static void SetResourceHandle(HMODULE hNew) { SSResourceHandle() = hNew; }
    static HMODULE GetResourceHandle() { return SSResourceHandle(); }
};

// Concatenation (non-template, as in the original CStdStringA operators).
inline _SS<char> operator+(const _SS<char> &s1, const _SS<char> &s2)
{
    _SS<char> sRet(s1.c_str());
    sRet.append(s2);
    return sRet;
}

inline _SS<char> operator+(const _SS<char> &s1, PCSTR pA)
{
    return s1 + _SS<char>(pA);
}
