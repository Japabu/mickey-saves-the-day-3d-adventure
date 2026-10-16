// MfPoJoystick: the Piccolo objects of the two joysticks ("Joystick1",
// "Joystick2"; the class "Joystick"). RuntimeLib.dll 1006cd30-1006f770, class
// name from the RTTI; member names follow MfPoLibrary's script classes.
#ifndef RUNTIMELIB_MFPOJOYSTICK_H
#define RUNTIMELIB_MFPOJOYSTICK_H
#include "MfJoystick.h"
#include "MfPoBase.h"

class MfPoJoystick : public MfPoBase {
public:
    MfPoJoystick(MfJoystick *joystick);
    virtual ~MfPoJoystick();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void Initialize();
    static void Shutdown();
    static MfString InitConstants();
private:
    MfPoJoystick(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMSetRoute(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetAxisModeToRel(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetAxisModeToAbs(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetAxisRange(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetAxisDeadZone(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetPOVValue(MfScDataHandle *, int, MfScDataHandle **);

    // the joystick; 0 until the first use, then chosen by the object's name
    MfJoystick *mJoystick;                                  // 0x14

    static MfPoJoystick *poExemplar;                        // 100f59f8
    static MfScPropTokenTable *pttPropTokens;               // 100f59fc

    friend void ResolveJoystick(MfJoystick *&joystick, MfPoJoystick *object);
};

#endif
