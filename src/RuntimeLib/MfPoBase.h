// MfPoBase (MfPoLibrary.dll): the base of the Piccolo objects (the
// script-visible wrappers of engine objects), with the helpers that report
// script errors. RuntimeLib's own script classes (MfPoJoystick, MfPoSound ...)
// derive from it. Declaration as in src/MfPoLibrary/MfPoBase.h (SteamboatApp
// PDB); constructor, copy, assignment and destructor are inline.
#ifndef RUNTIMELIB_MFPOBASE_H
#define RUNTIMELIB_MFPOBASE_H
// the script class members of MfScData/MfScDataObject (see MfScData.h); a
// unit that includes MfScData.h before this header does not get them
#ifndef RUNTIMELIB_MFSCDATA_H
#ifndef MF_SCRIPT_CLASS_DECLS
#define MF_SCRIPT_CLASS_DECLS
#endif
#endif
#include "MfScData.h"
class MfProperty;
class MfTypedValue;
class type_info;

// the member functions a script calls (MfScDataHandle::CreateData)
typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

class __declspec(dllimport) MfPoBase : public MfScObject {
public:
    MfPoBase(unsigned int flags = 0) : MfScObject(flags) {}
    int CheckNull(MfString, int, MfScDataHandle **, int *);
    void InvalidObject(MfString, MfString);
    int InvalidObject(void *, MfString, MfString);
    int DeletedObject(void *, MfString, MfString);
    void BadParameters(MfString);
    const type_info &getType(MfScDataHandle *, const type_info *);
    void __cdecl InvalidType(MfString, MfString, int, int, MfScDataHandle **, ...);
    void __cdecl WrongNumber(MfString, MfString, int, int, ...);
    static MfScDataHandle *__cdecl PropertyToPiccolo(MfProperty *);
    static MfScDataHandle *__cdecl PropertyValueToPiccolo(MfTypedValue *);
    static MfProperty *__cdecl PiccoloToProperty(const MfString &, MfScDataHandle *);
    static MfTypedValue *__cdecl PiccoloToPropertyValue(MfScDataHandle *, int);
};

#endif
