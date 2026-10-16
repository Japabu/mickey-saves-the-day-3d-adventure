// MfPoKeyboard (MfPoLibrary.dll 0x1002cf20-0x1002ff60)
#include "MfPoKeyboard.h"
#include "MfPoActor.h"
#include "MfPoLibrary.h"
#include "MfAiActor.h"
#include "MfAiKeyboard.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x1002cf20 ?GetPropTokenTable@MfPoKeyboard@@EAEPAVMfScPropTokenTable@@XZ
// 0x1002cf30 ??0MfPoKeyboard@@QAE@ABV0@@Z (implicit)
// 0x1002cf60 ??_EMfPoKeyboard@@UAEPAXI@Z

// 0x1002cfb0 _$E27
// 0x1002cfc0 _$E24
// 0x1002cfe0 _$E26
// 0x1002cff0 _$E25
static MfString strClassName("Keyboard");
MfPoKeyboard *MfPoKeyboard::poExemplar;
MfScPropTokenTable *MfPoKeyboard::pttPropTokens;

// 0x1002d000 ?GetClassNameA@MfPoKeyboard@@EAE?AVMfString@@XZ
MfString MfPoKeyboard::GetClassName()
{
    return strClassName;
}

// 0x1002d030 ??0MfPoKeyboard@@QAE@XZ
MfPoKeyboard::MfPoKeyboard() : MfPoBase(1)
{
}

// 0x1002d050 ??0MfPoKeyboard@@AAE@VExemplar@MfScData@@@Z
MfPoKeyboard::MfPoKeyboard(MfScData::Exemplar) : MfPoBase(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Keyboard");
    MfScCiMethod *m;
    SetProperty(0, "DisableAuthorToolKeys", MfScDataHandle::CreateData((MfScMethod)&MfPoKeyboard::PMDisableAuthorToolKeys, "Keyboard::DisableAuthorToolKeys", 0), 1);
    m = new MfScCiMethod("DisableAuthorToolKeys");
    *ci + m;
    SetProperty(0, "EnableAuthorToolKeys", MfScDataHandle::CreateData((MfScMethod)&MfPoKeyboard::PMEnableAuthorToolKeys, "Keyboard::EnableAuthorToolKeys", 0), 1);
    m = new MfScCiMethod("EnableAuthorToolKeys");
    *ci + m;
    SetProperty(0, "SetKeyRoute", MfScDataHandle::CreateData((MfScMethod)&MfPoKeyboard::PMSetKeyRoute, "Keyboard::SetKeyRoute", 2), 1);
    m = new MfScCiMethod("SetKeyRoute");
    *ci + m;
    *m + new MfScMethodParameterInfo("int keyCode", 0);
    *m + new MfScMethodParameterInfo("Actor actorHandle", 0);
    SetProperty(0, "SetKeyChangeRoute", MfScDataHandle::CreateData((MfScMethod)&MfPoKeyboard::PMSetKeyChangeRoute, "Keyboard::SetKeyChangeRoute", 2), 1);
    m = new MfScCiMethod("SetKeyChangeRoute");
    *ci + m;
    *m + new MfScMethodParameterInfo("int keyCode", 0);
    *m + new MfScMethodParameterInfo("Actor actorHandle", 0);
    SetProperty(0, "SetKeyPressPollRate", MfScDataHandle::CreateData((MfScMethod)&MfPoKeyboard::PMSetKeyPressPollRate, "Keyboard::SetKeyPressPollRate", 1), 1);
    m = new MfScCiMethod("SetKeyPressPollRate");
    *ci + m;
    *m + new MfScMethodParameterInfo("int pollRate", 0);
    *ci + new MfScCiProperty("lastKeycode - int (read only)");
    *ci + new MfScCiProperty("repeatCount - int (read only)");
    *ci + new MfScCiProperty("shiftKeyDown - boolean (read only)");
    *ci + new MfScCiProperty("lShiftKeyDown - boolean (read only)");
    *ci + new MfScCiProperty("rShiftKeyDown - boolean (read only)");
    *ci + new MfScCiProperty("ctrlKeyDown - boolean (read only)");
    *ci + new MfScCiProperty("lCtrlKeyDown - boolean (read only)");
    *ci + new MfScCiProperty("rCtrlKeyDown - boolean (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("repeatCount", 0);
    pttPropTokens->Add("lastKeycode", 1);
    pttPropTokens->Add("shiftKeyDown", 2);
    pttPropTokens->Add("lShiftKeyDown", 3);
    pttPropTokens->Add("rShiftKeyDown", 4);
    pttPropTokens->Add("ctrlKeyDown", 5);
    pttPropTokens->Add("lCtrlKeyDown", 6);
    pttPropTokens->Add("rCtrlKeyDown", 7);
}

// 0x1002e0e0 ??1MfPoKeyboard@@UAE@XZ
MfPoKeyboard::~MfPoKeyboard()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x1002e170 ?Initialize@MfPoKeyboard@@SAXXZ
void MfPoKeyboard::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoKeyboard(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1002e200 ?Shutdown@MfPoKeyboard@@SAXXZ
void MfPoKeyboard::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x1002e260 ?GetProperty@MfPoKeyboard@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoKeyboard::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    switch (LookupPropertyToken(name, pttPropTokens)) {
    case 0:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->GetRepeatCount());
        break;
    case 1:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->GetLastKeycode());
        break;
    case 2:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->ShiftKeyDown());
        break;
    case 3:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->LShiftKeyDown());
        break;
    case 4:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->RShiftKeyDown());
        break;
    case 5:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->CtrlKeyDown());
        break;
    case 6:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->LCtrlKeyDown());
        break;
    case 7:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->RCtrlKeyDown());
        break;
    default:
        if (this == poExemplar)
            err = MfScDataObject::GetProperty(name, value);
        else
            err = poExemplar->GetProperty(name, value);
        break;
    }
    return err;
}

// 0x1002e3e0 ?SetProperty@MfPoKeyboard@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoKeyboard::SetProperty(MfScDataHandle *self, const MfString &name,
                                                              MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return PEC_NO_ERROR;
}

// 0x1002e430 ?PMSetKeyRoute@MfPoKeyboard@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKeyboard::PMSetKeyRoute(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("KeyBoard.SetKeyRoute()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && actor) {
        int keyCode = args[0]->GetInt();
        MfAiActor *aiActor = actor->GetActorPtr();
        if (!aiActor) {
            BadParameters(MfString("Keyboard.SetKeyRoute()   The actor handle is not a valid actor."));
            return 0;
        }
        if (keyCode == -1) {
            for (int i = 0; i < 256; i++)
                g_paiKeyboard->SetKeyRoute(i, aiActor);
            return MfScDataHandle::CreateData(1);
        }
        g_paiKeyboard->SetKeyRoute(keyCode, aiActor);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Keyboard"), MfString("SetKeyRoute"), 0, nArgs, args, &typeid(int), &typeid(MfPoActor));
    return 0;
}

// 0x1002e580 ?PMSetKeyChangeRoute@MfPoKeyboard@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKeyboard::PMSetKeyChangeRoute(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("KeyBoard.SetKeyChangeRoute()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && actor) {
        int keyCode = args[0]->GetInt();
        MfAiActor *aiActor = actor->GetActorPtr();
        if (!aiActor) {
            BadParameters(MfString("Keyboard.SetKeyChangeRoutee()   The actor handle is not a valid actor."));
            return 0;
        }
        if (keyCode == -1) {
            for (int i = 0; i < 256; i++)
                g_paiKeyboard->SetKeyChangeRoute(i, aiActor);
            return MfScDataHandle::CreateData(1);
        }
        g_paiKeyboard->SetKeyChangeRoute(keyCode, aiActor);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Keyboard"), MfString("SetKeyChangeRoute"), 0, nArgs, args, &typeid(int), &typeid(MfPoActor));
    return 0;
}

// 0x1002e6d0 ?PMSetKeyPressPollRate@MfPoKeyboard@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKeyboard::PMSetKeyPressPollRate(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Keyboard.SetKeyPressPollRate()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
        g_paiKeyboard->SetKeyPressPollRate(args[0]->GetInt());
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("Keyboard"), MfString("SetKeyPressPollRate"), 0, nArgs, args, &typeid(int));
    return 0;
}

// 0x1002e790 ?PMEnableAuthorToolKeys@MfPoKeyboard@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKeyboard::PMEnableAuthorToolKeys(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    g_paiKeyboard->EnableAuthorToolKeys(1);
    return 0;
}

// 0x1002e7b0 ?PMDisableAuthorToolKeys@MfPoKeyboard@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKeyboard::PMDisableAuthorToolKeys(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    g_paiKeyboard->EnableAuthorToolKeys(0);
    return 0;
}

// 0x1002e7d0 ?InitConstants@MfPoKeyboard@@SA?AVMfString@@XZ
MfString MfPoKeyboard::InitConstants()
{
    MfString s;
    s << "global " << "VK_ALL" << " = " << -1 << ";";
    s << "global " << "VK_BACK" << " = " << VK_BACK << ";";
    s << "global " << "VK_TAB" << " = " << VK_TAB << ";";
    s << "global " << "VK_CLEAR" << " = " << VK_CLEAR << ";";
    s << "global " << "VK_RETURN" << " = " << VK_RETURN << ";";
    s << "global " << "VK_SHIFT" << " = " << VK_SHIFT << ";";
    s << "global " << "VK_CONTROL" << " = " << VK_CONTROL << ";";
    s << "global " << "VK_MENU" << " = " << VK_MENU << ";";
    s << "global " << "VK_PAUSE" << " = " << VK_PAUSE << ";";
    s << "global " << "VK_CAPITAL" << " = " << VK_CAPITAL << ";";
    s << "global " << "VK_ESCAPE" << " = " << VK_ESCAPE << ";";
    s << "global " << "VK_SPACE" << " = " << VK_SPACE << ";";
    s << "global " << "VK_PRIOR" << " = " << VK_PRIOR << ";";
    s << "global " << "VK_NEXT" << " = " << VK_NEXT << ";";
    s << "global " << "VK_END" << " = " << VK_END << ";";
    s << "global " << "VK_HOME" << " = " << VK_HOME << ";";
    s << "global " << "VK_LEFT" << " = " << VK_LEFT << ";";
    s << "global " << "VK_UP" << " = " << VK_UP << ";";
    s << "global " << "VK_RIGHT" << " = " << VK_RIGHT << ";";
    s << "global " << "VK_DOWN" << " = " << VK_DOWN << ";";
    s << "global " << "VK_SELECT" << " = " << VK_SELECT << ";";
    s << "global " << "VK_PRINT" << " = " << VK_PRINT << ";";
    s << "global " << "VK_EXECUTE" << " = " << VK_EXECUTE << ";";
    s << "global " << "VK_SNAPSHOT" << " = " << VK_SNAPSHOT << ";";
    s << "global " << "VK_INSERT" << " = " << VK_INSERT << ";";
    s << "global " << "VK_DELETE" << " = " << VK_DELETE << ";";
    s << "global " << "VK_HELP" << " = " << VK_HELP << ";";
    s << "global " << "VK_0" << " = " << 48 << ";";
    s << "global " << "VK_1" << " = " << 49 << ";";
    s << "global " << "VK_2" << " = " << 50 << ";";
    s << "global " << "VK_3" << " = " << 51 << ";";
    s << "global " << "VK_4" << " = " << 52 << ";";
    s << "global " << "VK_5" << " = " << 53 << ";";
    s << "global " << "VK_6" << " = " << 54 << ";";
    s << "global " << "VK_7" << " = " << 55 << ";";
    s << "global " << "VK_8" << " = " << 56 << ";";
    s << "global " << "VK_9" << " = " << 57 << ";";
    s << "global " << "VK_A" << " = " << 65 << ";";
    s << "global " << "VK_B" << " = " << 66 << ";";
    s << "global " << "VK_C" << " = " << 67 << ";";
    s << "global " << "VK_D" << " = " << 68 << ";";
    s << "global " << "VK_E" << " = " << 69 << ";";
    s << "global " << "VK_F" << " = " << 70 << ";";
    s << "global " << "VK_G" << " = " << 71 << ";";
    s << "global " << "VK_H" << " = " << 72 << ";";
    s << "global " << "VK_I" << " = " << 73 << ";";
    s << "global " << "VK_J" << " = " << 74 << ";";
    s << "global " << "VK_K" << " = " << 75 << ";";
    s << "global " << "VK_L" << " = " << 76 << ";";
    s << "global " << "VK_M" << " = " << 77 << ";";
    s << "global " << "VK_N" << " = " << 78 << ";";
    s << "global " << "VK_O" << " = " << 79 << ";";
    s << "global " << "VK_P" << " = " << 80 << ";";
    s << "global " << "VK_Q" << " = " << 81 << ";";
    s << "global " << "VK_R" << " = " << 82 << ";";
    s << "global " << "VK_S" << " = " << 83 << ";";
    s << "global " << "VK_T" << " = " << 84 << ";";
    s << "global " << "VK_U" << " = " << 85 << ";";
    s << "global " << "VK_V" << " = " << 86 << ";";
    s << "global " << "VK_W" << " = " << 87 << ";";
    s << "global " << "VK_X" << " = " << 88 << ";";
    s << "global " << "VK_Y" << " = " << 89 << ";";
    s << "global " << "VK_Z" << " = " << 90 << ";";
    s << "global " << "VK_a" << " = " << 353 << ";";
    s << "global " << "VK_b" << " = " << 354 << ";";
    s << "global " << "VK_c" << " = " << 355 << ";";
    s << "global " << "VK_d" << " = " << 356 << ";";
    s << "global " << "VK_e" << " = " << 357 << ";";
    s << "global " << "VK_f" << " = " << 358 << ";";
    s << "global " << "VK_g" << " = " << 359 << ";";
    s << "global " << "VK_h" << " = " << 360 << ";";
    s << "global " << "VK_i" << " = " << 361 << ";";
    s << "global " << "VK_j" << " = " << 362 << ";";
    s << "global " << "VK_k" << " = " << 363 << ";";
    s << "global " << "VK_l" << " = " << 364 << ";";
    s << "global " << "VK_m" << " = " << 365 << ";";
    s << "global " << "VK_n" << " = " << 366 << ";";
    s << "global " << "VK_o" << " = " << 367 << ";";
    s << "global " << "VK_p" << " = " << 368 << ";";
    s << "global " << "VK_q" << " = " << 369 << ";";
    s << "global " << "VK_r" << " = " << 370 << ";";
    s << "global " << "VK_s" << " = " << 371 << ";";
    s << "global " << "VK_t" << " = " << 372 << ";";
    s << "global " << "VK_u" << " = " << 373 << ";";
    s << "global " << "VK_v" << " = " << 374 << ";";
    s << "global " << "VK_w" << " = " << 375 << ";";
    s << "global " << "VK_x" << " = " << 376 << ";";
    s << "global " << "VK_y" << " = " << 377 << ";";
    s << "global " << "VK_z" << " = " << 378 << ";";
    s << "global " << "VK_NUMPAD0" << " = " << VK_NUMPAD0 << ";";
    s << "global " << "VK_NUMPAD1" << " = " << VK_NUMPAD1 << ";";
    s << "global " << "VK_NUMPAD2" << " = " << VK_NUMPAD2 << ";";
    s << "global " << "VK_NUMPAD3" << " = " << VK_NUMPAD3 << ";";
    s << "global " << "VK_NUMPAD4" << " = " << VK_NUMPAD4 << ";";
    s << "global " << "VK_NUMPAD5" << " = " << VK_NUMPAD5 << ";";
    s << "global " << "VK_NUMPAD6" << " = " << VK_NUMPAD6 << ";";
    s << "global " << "VK_NUMPAD7" << " = " << VK_NUMPAD7 << ";";
    s << "global " << "VK_NUMPAD8" << " = " << VK_NUMPAD8 << ";";
    s << "global " << "VK_NUMPAD9" << " = " << VK_NUMPAD9 << ";";
    s << "global " << "VK_MULTIPLY" << " = " << VK_MULTIPLY << ";";
    s << "global " << "VK_ADD" << " = " << VK_ADD << ";";
    s << "global " << "VK_SEPARATOR" << " = " << VK_SEPARATOR << ";";
    s << "global " << "VK_SUBTRACT" << " = " << VK_SUBTRACT << ";";
    s << "global " << "VK_DECIMAL" << " = " << VK_DECIMAL << ";";
    s << "global " << "VK_DIVIDE" << " = " << VK_DIVIDE << ";";
    s << "global " << "VK_F1" << " = " << VK_F1 << ";";
    s << "global " << "VK_F2" << " = " << VK_F2 << ";";
    s << "global " << "VK_F3" << " = " << VK_F3 << ";";
    s << "global " << "VK_F4" << " = " << VK_F4 << ";";
    s << "global " << "VK_F5" << " = " << VK_F5 << ";";
    s << "global " << "VK_F6" << " = " << VK_F6 << ";";
    s << "global " << "VK_F7" << " = " << VK_F7 << ";";
    s << "global " << "VK_F8" << " = " << VK_F8 << ";";
    s << "global " << "VK_F9" << " = " << VK_F9 << ";";
    s << "global " << "VK_F10" << " = " << VK_F10 << ";";
    s << "global " << "VK_F11" << " = " << VK_F11 << ";";
    s << "global " << "VK_F12" << " = " << VK_F12 << ";";
    s << "global " << "VK_F13" << " = " << VK_F13 << ";";
    s << "global " << "VK_F14" << " = " << VK_F14 << ";";
    s << "global " << "VK_F15" << " = " << VK_F15 << ";";
    s << "global " << "VK_F16" << " = " << VK_F16 << ";";
    s << "global " << "VK_F17" << " = " << VK_F17 << ";";
    s << "global " << "VK_F18" << " = " << VK_F18 << ";";
    s << "global " << "VK_F19" << " = " << VK_F19 << ";";
    s << "global " << "VK_F20" << " = " << VK_F20 << ";";
    s << "global " << "VK_F21" << " = " << VK_F21 << ";";
    s << "global " << "VK_F22" << " = " << VK_F22 << ";";
    s << "global " << "VK_F23" << " = " << VK_F23 << ";";
    s << "global " << "VK_F24" << " = " << VK_F24 << ";";
    s << "global " << "VK_NUMLOCK" << " = " << VK_NUMLOCK << ";";
    s << "global " << "VK_SCROLL" << " = " << VK_SCROLL << ";";
    return s;
}

// 0x1002ff40 ?FilloutPropertyNameList@MfPoKeyboard@@EAEXPAVMfStringList@@@Z
void MfPoKeyboard::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
