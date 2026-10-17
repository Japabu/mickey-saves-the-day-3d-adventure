// MfScMethodRoutine: a script routine that calls a method of the object the script runs on.
#pragma once
#include "PiccoloRT_api.h"
#include "MfScDataMethod.h"

typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

// A method of the object the calling script runs on.
class PICCOLORT_API MfScMethodRoutine : public MfScDataMethod {
public:
    MfScMethodRoutine(MfScMethod method = 0, int numArgs = -1)
        : MfScDataMethod(kMethod, numArgs), m_ordinal(0), m_method(method) {}
    MfScMethodRoutine(int ordinal, MfScMethod method, int numArgs = -1)
        : MfScDataMethod(kMethod, numArgs), m_method(method), m_ordinal(ordinal) {}
    void SetMethod(MfScMethod method) { m_method = method; }
    MfScMethod GetMethod() const { return m_method; }
    int GetOrdinal() const { return m_ordinal; }
private:
    MfScMethod m_method;            // +0x1c
    int m_ordinal;                  // +0x20
};
