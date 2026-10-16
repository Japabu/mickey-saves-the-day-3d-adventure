// PORT: InvalidType's variable arguments are all pointers; an overload passes literal 0 arguments on as null pointers (an int in `...` is not a pointer on arm64).
// MfPoBase: the base of MfPoLibrary's Piccolo objects (the script-visible
// wrappers of engine objects), with the helpers that report script errors and
// convert between Piccolo values and property values. Declaration from the
// SteamboatApp PDB; constructor, copy, assignment and destructor are inline.
#pragma once
#include "MfScObject.h"
class MfProperty;
class MfTypedValue;
#include <typeinfo.h> // port: was a declaration of class type_info

class __declspec(dllexport) MfPoBase : public MfScObject {
public:
    MfPoBase(unsigned int flags = 0) : MfScObject(flags) {}
    int CheckNull(MfString, int, MfScDataHandle **, int *);
    void InvalidObject(MfString, MfString);
    int InvalidObject(void *, MfString, MfString);
    int DeletedObject(void *, MfString, MfString);
    void BadParameters(MfString);
    const type_info &getType(MfScDataHandle *, const type_info *);
    void __cdecl InvalidType(MfString, MfString, int, int, MfScDataHandle **, ...);
    // PORT: callers pass a literal 0 for "no expected type" among the const type_info *
    // arguments; through `...` that is a 32-bit int, and on arm64 InvalidType's 64-bit
    // va_arg would read garbage in the upper half. This overload takes the calls first
    // and hands every argument on as a pointer (an int becomes a null pointer).
    template <class... A>
    void InvalidType(MfString cls, MfString func, int nPrefixes, int nParams, MfScDataHandle **params, A... a)
    {
        void (__cdecl MfPoBase::*variadic)(MfString, MfString, int, int, MfScDataHandle **, ...) = &MfPoBase::InvalidType;
        (this->*variadic)(cls, func, nPrefixes, nParams, params, PortVarargPointer(a)...);
    }
    static const void *PortVarargPointer(int v) { return (const void *)(intptr_t)v; }
    template <class T> static T *PortVarargPointer(T *p) { return p; }
    void __cdecl WrongNumber(MfString, MfString, int, int, ...);
    static MfScDataHandle *__cdecl PropertyToPiccolo(MfProperty *);
    static MfScDataHandle *__cdecl PropertyValueToPiccolo(MfTypedValue *);
    static MfProperty *__cdecl PiccoloToProperty(const MfString &, MfScDataHandle *);
    static MfTypedValue *__cdecl PiccoloToPropertyValue(MfScDataHandle *, int);
};
