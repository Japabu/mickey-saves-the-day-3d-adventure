// MfPoBase: the base of MfPoLibrary's Piccolo objects (the script-visible
// wrappers of engine objects), with the helpers that report script errors and
// convert between Piccolo values and property values. Declaration from the
// SteamboatApp PDB; constructor, copy, assignment and destructor are inline.
#ifndef MFPOLIBRARY_MFPOBASE_H
#define MFPOLIBRARY_MFPOBASE_H
#include "MfPoLibrary_api.h"
#include "MfScObject.h"
class MfProperty;
class MfTypedValue;
class type_info;

class MFPO_API MfPoBase : public MfScObject {
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
