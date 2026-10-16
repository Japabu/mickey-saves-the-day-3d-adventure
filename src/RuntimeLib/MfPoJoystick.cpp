// MfPoJoystick (RuntimeLib.dll 0x1006cd30-0x1006f770): the Piccolo objects of
// the joysticks. Two instances exist, the globals Joystick1 and Joystick2; the
// class object "Joystick" is not registered. The properties report the stick
// axes and the device capabilities, the methods route joystick events to
// actors and configure the axes.
#include "MfPoJoystick.h"
#include "MfPoActor.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"
#include <typeinfo.h>

// the event codes of MfJoystick::SetRoute (see InitConstants)
enum {
    JOYSTICK1_BUTTON_FIRST = 0,  JOYSTICK1_BUTTON_LAST = 9,
    JOYSTICK2_BUTTON_FIRST = 10, JOYSTICK2_BUTTON_LAST = 19,
    JOYSTICK1_AXIS_FIRST = 20,   JOYSTICK1_AXIS_LAST = 25,
    JOYSTICK2_AXIS_FIRST = 26,   JOYSTICK2_AXIS_LAST = 31,
    JOYSTICK1_POV0 = 32, JOYSTICK1_POV1, JOYSTICK1_POV2, JOYSTICK1_POV3,
    JOYSTICK2_POV0 = 36, JOYSTICK2_POV1, JOYSTICK2_POV2, JOYSTICK2_POV3,
    JOYSTICK1_ALL_EVENTS = 9998,
    JOYSTICK2_ALL_EVENTS = 9999
};

// 0x1006cd30 _$E21
// 0x1006cd40 _$E18
// 0x1006cd60 _$E20
// 0x1006cd70 _$E19
static MfString strClassName("Joystick");
MfPoJoystick *MfPoJoystick::poExemplar;
MfScPropTokenTable *MfPoJoystick::pttPropTokens;

// 0x1006cd80 ?GetClassNameA@MfPoJoystick@@EAE?AVMfString@@XZ
MfString MfPoJoystick::GetClassName()
{
    return strClassName;
}

// 0x1006cdb0 ??0MfPoJoystick@@QAE@PAVMfJoystick@@@Z
MfPoJoystick::MfPoJoystick(MfJoystick *joystick) : MfPoBase(1)
{
    mJoystick = joystick;
}

// 0x1006cde0 ?GetPropTokenTable@MfPoJoystick@@EAEPAVMfScPropTokenTable@@XZ
// 0x1006cdf0 ??_GMfPoJoystick@@UAEPAXI@Z

// 0x1006ce10 ??0MfPoJoystick@@AAE@VExemplar@MfScData@@@Z
MfPoJoystick::MfPoJoystick(MfScData::Exemplar) : MfPoBase(0), mJoystick(0)
{
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Joystick");
    MfScCiMethod *m;
    SetProperty(0, "SetRoute", MfScDataHandle::CreateData((MfScMethod)&MfPoJoystick::PMSetRoute, "Joystick::SetRoute", 2), 1);
    m = new MfScCiMethod("SetRoute");
    *ci + m;
    *m + new MfScMethodParameterInfo("int JoystickCode", 0);
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    SetProperty(0, "SetAxisModeToRel", MfScDataHandle::CreateData((MfScMethod)&MfPoJoystick::PMSetAxisModeToRel, "Joystick::SetAxisModeToRel", 1), 1);
    m = new MfScCiMethod("SetAxisModeToRel");
    *ci + m;
    *m + new MfScMethodParameterInfo("int axis", 0);
    SetProperty(0, "SetAxisModeToAbs", MfScDataHandle::CreateData((MfScMethod)&MfPoJoystick::PMSetAxisModeToAbs, "Joystick::SetAxisModeToAbs", 1), 1);
    m = new MfScCiMethod("SetAxisModeToAbs");
    *ci + m;
    *m + new MfScMethodParameterInfo("int axis", 0);
    SetProperty(0, "SetAxisRange", MfScDataHandle::CreateData((MfScMethod)&MfPoJoystick::PMSetAxisRange, "Joystick::SetAxisRange", 3), 1);
    m = new MfScCiMethod("SetAxisRange");
    *ci + m;
    *m + new MfScMethodParameterInfo("int axis", 0);
    *m + new MfScMethodParameterInfo("int min", 0);
    *m + new MfScMethodParameterInfo("int max", 0);
    SetProperty(0, "GetPOVValue", MfScDataHandle::CreateData((MfScMethod)&MfPoJoystick::PMGetPOVValue, "Joystick::GetPOVValue", 1), 1);
    m = new MfScCiMethod("GetPOVValue");
    *ci + m;
    *m + new MfScMethodParameterInfo("int indexPOV", 0);
    SetProperty(0, "SetAxisDeadZone", MfScDataHandle::CreateData((MfScMethod)&MfPoJoystick::PMSetAxisDeadZone, "Joystick::SetAxisDeadZone", 2), 1);
    m = new MfScCiMethod("SetAxisDeadZone");
    *ci + m;
    *m + new MfScMethodParameterInfo("int axis", 0);
    *m + new MfScMethodParameterInfo("int value", 0);
    *ci + new MfScCiProperty("iXAxisValue - int (read only)");
    *ci + new MfScCiProperty("iYAxisValue - int (read only)");
    *ci + new MfScCiProperty("iZAxisValue - int (read only)");
    *ci + new MfScCiProperty("iRxAxisValue - int (read only)");
    *ci + new MfScCiProperty("iRyAxisValue - int (read only)");
    *ci + new MfScCiProperty("iRzAxisValue - int (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("iXAxisValue", 0);
    pttPropTokens->Add("iYAxisValue", 1);
    pttPropTokens->Add("iZAxisValue", 2);
    pttPropTokens->Add("iRxAxisValue", 3);
    pttPropTokens->Add("iRyAxisValue", 4);
    pttPropTokens->Add("iRzAxisValue", 5);
    pttPropTokens->Add("iNumAxis", 7);
    pttPropTokens->Add("iNumButtons", 6);
}

// 0x1006e0e0 ??1MfPoJoystick@@UAE@XZ
MfPoJoystick::~MfPoJoystick()
{
    if (this == poExemplar)
        poExemplar = 0;
}

// 0x1006e100 ?Initialize@MfPoJoystick@@SAXXZ
// Creates the class object and the two joystick objects (only when both
// joysticks exist) and registers the joysticks as Joystick1 and Joystick2.
void MfPoJoystick::Initialize()
{
    if (poExemplar == 0 && gJoysticks[0] && gJoysticks[1]) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoJoystick(exemplar);
        MfPoJoystick *first = new MfPoJoystick(gJoysticks[0]);
        first->Name() = "Joystick1";
        MfPoJoystick *second = new MfPoJoystick(gJoysticks[1]);
        second->Name() = "Joystick2";
        MfScDataHandle *handle = MfScDataHandle::CreateData(first);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName + MfString(1), handle, 1, 0);
        handle = MfScDataHandle::CreateData(second);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName + MfString(2), handle, 1, 0);
    }
}

// 0x1006e2c0 ?Shutdown@MfPoJoystick@@SAXXZ
void MfPoJoystick::Shutdown()
{
    MfScDataHandle *first;
    MfScVmEnv::GetGlobalVariable(strClassName + MfString(1), &first);
    if (first) {
        first->mFlags &= ~8;
        MfScVmEnv::SetGlobalVariable(strClassName + MfString(1), 0, 1, 0);
        first->Release();
    }
    MfScDataHandle *second;
    MfScVmEnv::GetGlobalVariable(strClassName + MfString(2), &second);
    if (second) {
        second->mFlags &= ~8;
        MfScVmEnv::SetGlobalVariable(strClassName + MfString(2), 0, 1, 0);
        second->Release();
    }
    if (poExemplar) {
        delete poExemplar;
        poExemplar = 0;
    }
}

// 0x1006ccf0 ?ResolveJoystick@@YAXAAPAVMfJoystick@@PAVMfPoJoystick@@@Z
// An object without a joystick takes the one its name says.
void ResolveJoystick(MfJoystick *&joystick, MfPoJoystick *object)
{
    if (joystick == 0) {
        if (object->Name() == "Joystick1")
            joystick = gJoysticks[0];
        else
            joystick = gJoysticks[1];
    }
}

// 0x1006e480 ?GetProperty@MfPoJoystick@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoJoystick::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    *value = 0;
    ResolveJoystick(mJoystick, this);
    int inherited = 0;
    if (mJoystick) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(mJoystick->XAxis());
            break;
        case 1:
            *value = MfScDataHandle::CreateData(mJoystick->YAxis());
            break;
        case 2:
            *value = MfScDataHandle::CreateData(mJoystick->ZAxis());
            break;
        case 3:
            *value = MfScDataHandle::CreateData(mJoystick->RxAxis());
            break;
        case 4:
            *value = MfScDataHandle::CreateData(mJoystick->RyAxis());
            break;
        case 5:
            *value = MfScDataHandle::CreateData(mJoystick->RzAxis());
            break;
        case 7:     // iNumAxis: always the first joystick's
            *value = MfScDataHandle::CreateData((int)gJoysticks[0]->NumAxes());
            break;
        case 6:     // iNumButtons
            *value = MfScDataHandle::CreateData((int)gJoysticks[0]->NumButtons());
            break;
        default:
            inherited = 1;
        }
    } else
        inherited = 1;
    if (inherited) {
        if (this == poExemplar)
            err = MfScDataObject::GetProperty(name, value);
        else
            err = poExemplar->GetProperty(name, value);
    }
    return err;
}

// 0x1006e600 ?SetProperty@MfPoJoystick@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
// The joystick properties are read only.
MfScDataObject::MfPropertyErrorCode MfPoJoystick::SetProperty(MfScDataHandle *self, const MfString &name,
                                                              MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return (MfPropertyErrorCode)0;
}

// 0x1006e650 ?PMSetRoute@MfPoJoystick@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// SetRoute(code, actor): sends the joystick event `code` (or all events of
// this joystick) to the actor.
MfScDataHandle *MfPoJoystick::PMSetRoute(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    ResolveJoystick(mJoystick, this);
    if (CheckNull(Name() + MfString(".SetRoute()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
    if (args[0]->IsType(0x10) && actor) {
        int code = args[0]->mLong;
        MfAiActor *aiActor = actor->mActor;
        if (aiActor) {
        if (mJoystick == gJoysticks[0]) {
            if (code == JOYSTICK1_ALL_EVENTS) {
                int i;
                for (i = JOYSTICK1_BUTTON_FIRST; i <= JOYSTICK1_BUTTON_LAST; i++)
                    mJoystick->SetRoute(i, aiActor);
                for (i = JOYSTICK1_AXIS_FIRST; i <= JOYSTICK1_AXIS_LAST; i++)
                    mJoystick->SetRoute(i, aiActor);
                mJoystick->SetRoute(JOYSTICK1_POV0, aiActor);
                mJoystick->SetRoute(JOYSTICK1_POV1, aiActor);
                mJoystick->SetRoute(JOYSTICK1_POV2, aiActor);
                mJoystick->SetRoute(JOYSTICK1_POV3, aiActor);
            } else
                mJoystick->SetRoute(code, aiActor);
        } else if (mJoystick == gJoysticks[1]) {
            if (code == JOYSTICK2_ALL_EVENTS) {
                int i;
                for (i = JOYSTICK2_BUTTON_FIRST; i <= JOYSTICK2_BUTTON_LAST; i++)
                    mJoystick->SetRoute(i, aiActor);
                for (i = JOYSTICK2_AXIS_FIRST; i <= JOYSTICK2_AXIS_LAST; i++)
                    mJoystick->SetRoute(i, aiActor);
                mJoystick->SetRoute(JOYSTICK2_POV0, aiActor);
                mJoystick->SetRoute(JOYSTICK2_POV1, aiActor);
                mJoystick->SetRoute(JOYSTICK2_POV2, aiActor);
                mJoystick->SetRoute(JOYSTICK2_POV3, aiActor);
            } else
                mJoystick->SetRoute(code, aiActor);
        }
        return MfScDataHandle::CreateData(1);
        }
        BadParameters(Name() + MfString(".SetRoute()   The actor handle is not a valid actor."));
        return 0;
    }
    InvalidType(Name(), MfString("SetRoute"), 0, nArgs, args, &typeid(int), &typeid(MfPoActor));
    return 0;
}

// 0x1006e8c0 ?PMSetAxisModeToRel@MfPoJoystick@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoJoystick::PMSetAxisModeToRel(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    ResolveJoystick(mJoystick, this);
    if (CheckNull(Name() + MfString(".SetAxisModeToRel()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x10)) {
        mJoystick->SetAxisModeToRel(args[0]->mLong);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(Name(), MfString("SetAxisModeToRel"), 0, nArgs, args, &typeid(int));
    return 0;
}

// 0x1006e9d0 ?PMSetAxisModeToAbs@MfPoJoystick@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoJoystick::PMSetAxisModeToAbs(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    ResolveJoystick(mJoystick, this);
    if (CheckNull(Name() + MfString(".SetAxisModeToAbs()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x10)) {
        mJoystick->SetAxisModeToAbs(args[0]->mLong);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(Name(), MfString("SetAxisModeToAbs"), 0, nArgs, args, &typeid(int));
    return 0;
}

// 0x1006eae0 ?PMSetAxisRange@MfPoJoystick@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoJoystick::PMSetAxisRange(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    ResolveJoystick(mJoystick, this);
    if (CheckNull(Name() + MfString(".SetAxisRange()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x10) && args[1]->IsType(0x10) && args[2]->IsType(0x10)) {
        mJoystick->SetAxisRange(args[0]->mLong, args[1]->mLong, args[2]->mLong);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(Name(), MfString("SetAxisRange"), 0, nArgs, args, &typeid(int), &typeid(int), &typeid(int));
    return 0;
}

// 0x1006ec20 ?PMSetAxisDeadZone@MfPoJoystick@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoJoystick::PMSetAxisDeadZone(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    ResolveJoystick(mJoystick, this);
    if (CheckNull(Name() + MfString(".SetAxisDeadZone()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x10) && args[1]->IsType(0x10)) {
        mJoystick->SetDeadZone(args[0]->mLong, args[1]->mLong);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(Name(), MfString("SetAxisDeadZone"), 0, nArgs, args, &typeid(int), &typeid(int));
    return 0;
}

// 0x1006ed50 ?PMGetPOVValue@MfPoJoystick@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// GetPOVValue(index): the hat position of the first joystick (0 for a hat it
// does not have).
MfScDataHandle *MfPoJoystick::PMGetPOVValue(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    ResolveJoystick(mJoystick, this);
    if (CheckNull(Name() + MfString(".GetPOVValue()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x10)) {
        return MfScDataHandle::CreateData((int)gJoysticks[0]->POVValue(args[0]->mLong));
    }
    InvalidType(Name(), MfString("GetPOVValue"), 0, nArgs, args, &typeid(int), &typeid(int), &typeid(int));
    return 0;
}

// 0x1006ee80 ?InitConstants@MfPoJoystick@@SA?AVMfString@@XZ
// The script constants of the joystick events.
MfString MfPoJoystick::InitConstants()
{
    MfString s;
    s << "global " << "MF_JOYSTK_RXAXIS" << " = " << 12 << ";";
    s << "global " << "MF_JOYSTK_RYAXIS" << " = " << 16 << ";";
    s << "global " << "MF_JOYSTK_RZAXIS" << " = " << 20 << ";";
    s << "global " << "MF_JOYSTK_XAXIS" << " = " << 0 << ";";
    s << "global " << "MF_JOYSTK_YAXIS" << " = " << 4 << ";";
    s << "global " << "MF_JOYSTK_ZAXIS" << " = " << 8 << ";";
    s << "global " << "MF_JOYSTK1_BTN1_DOWN" << " = " << 0 << ";";
    s << "global " << "MF_JOYSTK1_BTN2_DOWN" << " = " << 1 << ";";
    s << "global " << "MF_JOYSTK1_BTN3_DOWN" << " = " << 2 << ";";
    s << "global " << "MF_JOYSTK1_BTN4_DOWN" << " = " << 3 << ";";
    s << "global " << "MF_JOYSTK1_BTN5_DOWN" << " = " << 4 << ";";
    s << "global " << "MF_JOYSTK1_BTN6_DOWN" << " = " << 5 << ";";
    s << "global " << "MF_JOYSTK1_BTN7_DOWN" << " = " << 6 << ";";
    s << "global " << "MF_JOYSTK1_BTN8_DOWN" << " = " << 7 << ";";
    s << "global " << "MF_JOYSTK1_BTN9_DOWN" << " = " << 8 << ";";
    s << "global " << "MF_JOYSTK1_BTN10_DOWN" << " = " << 9 << ";";
    s << "global " << "MF_JOYSTK1_X_AXISMOVE" << " = " << 20 << ";";
    s << "global " << "MF_JOYSTK1_Y_AXISMOVE" << " = " << 21 << ";";
    s << "global " << "MF_JOYSTK1_Z_AXISMOVE" << " = " << 22 << ";";
    s << "global " << "MF_JOYSTK1_RX_AXISMOVE" << " = " << 23 << ";";
    s << "global " << "MF_JOYSTK1_RY_AXISMOVE" << " = " << 24 << ";";
    s << "global " << "MF_JOYSTK1_RZ_AXISMOVE" << " = " << 23 << ";"; // sic: the original repeats RX
    s << "global " << "MF_JOYSTK1_POV0" << " = " << 32 << ";";
    s << "global " << "MF_JOYSTK1_POV1" << " = " << 33 << ";";
    s << "global " << "MF_JOYSTK1_POV2" << " = " << 34 << ";";
    s << "global " << "MF_JOYSTK1_POV3" << " = " << 35 << ";";
    s << "global " << "MF_JOYSTK1_ALL_EVENTS" << " = " << 9998 << ";";
    s << "global " << "MF_JOYSTK2_BTN1_DOWN" << " = " << 10 << ";";
    s << "global " << "MF_JOYSTK2_BTN2_DOWN" << " = " << 11 << ";";
    s << "global " << "MF_JOYSTK2_BTN3_DOWN" << " = " << 12 << ";";
    s << "global " << "MF_JOYSTK2_BTN4_DOWN" << " = " << 13 << ";";
    s << "global " << "MF_JOYSTK2_BTN5_DOWN" << " = " << 14 << ";";
    s << "global " << "MF_JOYSTK2_BTN6_DOWN" << " = " << 15 << ";";
    s << "global " << "MF_JOYSTK2_BTN7_DOWN" << " = " << 16 << ";";
    s << "global " << "MF_JOYSTK2_BTN8_DOWN" << " = " << 17 << ";";
    s << "global " << "MF_JOYSTK2_BTN9_DOWN" << " = " << 18 << ";";
    s << "global " << "MF_JOYSTK2_BTN10_DOWN" << " = " << 19 << ";";
    s << "global " << "MF_JOYSTK2_X_AXISMOVE" << " = " << 26 << ";";
    s << "global " << "MF_JOYSTK2_Y_AXISMOVE" << " = " << 27 << ";";
    s << "global " << "MF_JOYSTK2_Z_AXISMOVE" << " = " << 28 << ";";
    s << "global " << "MF_JOYSTK2_RX_AXISMOVE" << " = " << 29 << ";";
    s << "global " << "MF_JOYSTK2_RY_AXISMOVE" << " = " << 30 << ";";
    s << "global " << "MF_JOYSTK2_RZ_AXISMOVE" << " = " << 29 << ";"; // sic: the original repeats RX
    s << "global " << "MF_JOYSTK2_POV0" << " = " << 36 << ";";
    s << "global " << "MF_JOYSTK2_POV1" << " = " << 37 << ";";
    s << "global " << "MF_JOYSTK2_POV2" << " = " << 38 << ";";
    s << "global " << "MF_JOYSTK2_POV3" << " = " << 39 << ";";
    s << "global " << "MF_JOYSTK2_ALL_EVENTS" << " = " << 9999 << ";";
    return s;
}

// 0x1006f750 ?FilloutPropertyNameList@MfPoJoystick@@EAEXPAVMfStringList@@@Z
// (folded with MfPoActivity::FilloutPropertyNameList)
void MfPoJoystick::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
