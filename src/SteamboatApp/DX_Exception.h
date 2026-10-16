#pragma once

namespace DX {
class Exception {
public:
    virtual ~Exception();
    Exception(char *);
    Exception(long, char *, int, char *);
    Exception(long, char *, ...);
    char * GetDetails();
    char * GetErrorString();
    int ReportError(unsigned int);
    long GetResult();
    static int __cdecl MessageBoxA(unsigned int, char *, ...);
protected:
    static char * __cdecl ErrorToString(long);
public:
    long mResult;
    char m_szDetails[256];
};
}
