#pragma once
#include "MfScDataMethod.h"

typedef MfScDataHandle *(__cdecl *MfScFunction)(MfScDataHandle *, int, MfScDataHandle **);
typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

// A C function callable from script code.
class __declspec(dllexport) MfScFunctionRoutine : public MfScDataMethod {
public:
    MfScFunctionRoutine(MfScFunction fn = 0, int numArgs = -1)
        : MfScDataMethod(kFunction, numArgs), m_function(fn) {}
    void SetFunction(MfScFunction fn) { m_function = fn; }
    MfScFunction GetFunction() const { return m_function; }
private:
    MfScFunction m_function;        // +0x1c
};

// A method of the object the calling script runs on.
class __declspec(dllexport) MfScMethodRoutine : public MfScDataMethod {
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

// A method bound to a given object.
class __declspec(dllexport) MfScExtRoutine : public MfScDataMethod {
public:
    MfScExtRoutine(MfScData *object, MfScMethod method, int numArgs = -1)
        : MfScDataMethod(kExtMethod, numArgs), m_method(method), m_object(object), m_ordinal(0) {}
    MfScExtRoutine(MfScData *object, int ordinal, MfScMethod method, int numArgs = -1)
        : MfScDataMethod(kMethod, numArgs), m_ordinal(ordinal), m_method(method), m_object(object) {}
    void Bind(MfScData *object, MfScMethod method);
    MfScMethod GetMethod() const { return m_method; }
    MfScData *GetObjectA() const { return m_object; }
    int GetOrdinal() const { return m_ordinal; }
private:
    MfScMethod m_method;            // +0x1c
    MfScData *m_object;             // +0x20
    int m_ordinal;                  // +0x24
};
