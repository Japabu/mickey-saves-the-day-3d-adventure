// MfScDataMethod, MfScMethodRoutine (PiccoloRT.dll): callable script data; a
// method routine calls a member function of the object the script runs on.
// Declarations as in src/PiccoloRT; MfPoLibrary only reads the method back.
#pragma once
#include "MfScData.h"
#include "MfScDataHandle.h"

class __declspec(dllimport) MfScDataMethod : public MfScData {
public:
    enum MfScMethodType { kNone, kFunction, kMethod, kExtMethod };
    MfScDataMethod(MfScMethodType type, int numArgs);
    virtual ~MfScDataMethod();
protected:
    int m_type;                                             // 0x0c
    MfScDataHandle **m_constants;                           // 0x10
    int m_numConstants;                                     // 0x14
    int m_numArgs;                                          // 0x18
};

class __declspec(dllimport) MfScMethodRoutine : public MfScDataMethod {
public:
    MfScMethod GetMethod() const { return m_method; }
private:
    MfScMethod m_method;                                    // 0x1c
    int m_ordinal;                                          // 0x20
};
