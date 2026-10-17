// PxtMath.dll 0x10001000-0x10002040: the "Math" script extension (MfPxtMath).
#ifndef PXTMATH_10001000_H
#define PXTMATH_10001000_H

// PiccoloRT.dll; its headers bring mfutil's MfString, MfThread, MfListItemInterface and
// MfSortedListItem (PiccoloRT's declarations of them until PiccoloRT includes mfutil's).
#include "../../PiccoloRT/MfScDataHandle.h"
#include "../../mfutil/MfNullString.h"

class MfScData;
typedef MfScDataHandle *(MfScData::*MfScMethod)(MfScDataHandle *, int, MfScDataHandle **);

#include "../../PiccoloRT/MfScVmEnv.h"
#include "../../PiccoloRT/MfScThread.h"
#include "../../PiccoloRT/MfScObject.h"
#include "../../PiccoloRT/MfScClassInfo.h"

// by-value constructor argument with an empty destructor (its out-of-line copy
// is at 0x100017c0, used by the EH unwind funclet of the constructor)
class MfPxtMathArg {
public:
    MfPxtMathArg() { }
    ~MfPxtMathArg() { }
};

// the plugin's script object, the global "Math"
class MfPxtMath : public MfScObject {
public:
    MfPxtMath(MfPxtMathArg);
    ~MfPxtMath();
    MfScDataHandle *Sinh(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Cosh(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Tanh(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Hypot(MfScDataHandle *, int, MfScDataHandle **);
};

typedef int (*PscCompileFn)(const MfString &, const MfString &, MfString *);

MfString GetMathConstantsSource();
void CreateMathObject();
void DestroyMathObject();

#endif
