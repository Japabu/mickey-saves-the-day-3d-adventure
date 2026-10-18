#include "SBdLineGroup.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MBRuntime.h"
#include "MfAi.h"
#include "MfHookData.h"
#include "SBDialoguePropertyManager.h"
#include <stdio.h>

// 0x10051e60 ??0SBdLineGroup@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBdLineGroup::SBdLineGroup(MBString &lineGroupName) : mCurrentLine(0), mActive(false), mSoundPlaying(false)
{
    _mRefCnt = 1;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(700, this);
    mLineGroupName = lineGroupName;
}

// 0x10052040 ?QueryInterface@SBdLineGroup@@UAEJABUMBGUID@@PAPAX@Z
long SBdLineGroup::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100520e0 ?AddRef@SBdLineGroup@@UAEKXZ
unsigned long SBdLineGroup::AddRef()
{
    return ++_mRefCnt;
}

// 0x10052100 ?Release@SBdLineGroup@@UAEKXZ
unsigned long SBdLineGroup::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10052130 ??_GSBdLineGroup@@UAEPAXI@Z

// 0x10052170 ??1SBdLineGroup@@UAE@XZ
SBdLineGroup::~SBdLineGroup()
{
    mDialogueLines.clear();
    mTrueVector.clear();
    mFalseVector.clear();
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(700, this);
}

// EQUIV 0x10052420 ?LoadFromPref@SBdLineGroup@@QAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z: reviewed: VC6 inlines compare(const basic_string &) (its compare(0, size, ptr, len) call), the original calls it; the rest are EH state stores and pushes moved past independent instructions. Was: VC6 inlines the compare(const basic_string &) of the conditional test, the original calls it
void SBdLineGroup::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    MBString dialogueLine;
    int lineNum = 0;
    char line[80];
    sprintf(line, "line%d", lineNum);
    if (pref->GetString(stream, mLineGroupName, "conditional", mConditionalString, "")) {
        if (mConditionalString.compare(MBString("null")) != 0) {
            mCheckConditional = true;
            ParseConditional();
        }
    }
    while (pref->GetString(stream, mLineGroupName, line, dialogueLine, "")) {
        mDialogueLines.push_back(dialogueLine);
        lineNum++;
        sprintf(line, "line%d", lineNum);
    }
    pref->GetString(stream, mLineGroupName, "exit event", mExitEvent, "null");
    pref->GetString(stream, mLineGroupName, "exit event data", mExitEventData, "null");
    pref->GetString(stream, mLineGroupName, "true", mTrueString, "");
    pref->GetString(stream, mLineGroupName, "false", mFalseString, "");
    if (mTrueString.size() > 0)
        ParseString(mTrueString, mTrueVector);
    if (mFalseString.size() > 0)
        ParseString(mFalseString, mFalseVector);
}

// 0x10052c40 ?ParseString@SBdLineGroup@@IAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAV?$MBVector@K@@@Z
void SBdLineGroup::ParseString(MBString &propString, MBVector<unsigned long> &propVec)
{
    int nProperties = SBDialoguePropertyManager::GetInstance()->GetNumberOfProperties();
    for (int i = 0; i < nProperties; i++) {
        MBString tmpString = SBDialoguePropertyManager::GetInstance()->GetPropertyStringByOrder(i);
        const char *propertyString = tmpString.c_str();
        int pos = propString.find(propertyString);
        while (pos != -1) {
            bool space = true;
            if (pos > 0 && propString[pos - 1] != ' ')
                space = false;
            if (pos + tmpString.size() < propString.size() && propString[pos + tmpString.size()] != ' ')
                space = false;
            if (space)
                propVec.push_back(MBHashString(SBDialoguePropertyManager::GetInstance()->GetPropertyStringByOrder(i).c_str()));
            pos = propString.find(propertyString, pos + 1);
        }
    }
}

// 0x10052f60 ?SetProperties@SBdLineGroup@@IAEXAAV?$MBVector@K@@HPAVMfAiActor@@@Z
void SBdLineGroup::SetProperties(MBVector<unsigned long> &propVec, int val, MfAiActor *mfPrimaryActor)
{
    for (int i = 0; i < propVec.size(); i++)
        SBDialoguePropertyManager::GetInstance()->SetProperty(propVec[i], val, mfPrimaryActor);
}

// TODO 0x10052fd0 ?PlayDialogue@SBdLineGroup@@QAE_NPAVMfAiActor@@@Z: (79% similar) not matching yet
bool SBdLineGroup::PlayDialogue(MfAiActor *mfPrimaryActor)
{
    if (mActive && !mSoundPlaying) {
        if (mCurrentLine < mDialogueLines.size()) {
            MBString ndtoken("event");
            if (mDialogueLines[mCurrentLine].compare(0, 5, ndtoken) == 0) {
                MBString message = mDialogueLines[mCurrentLine].substr(6);
                MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
                broadcaster->BroadcastEvent(300, 309, (void *)message.c_str());
            } else {
                if (mDialogueLines[mCurrentLine][5] == 'x' && mDialogueLines[mCurrentLine][6] == 'x') {
                    const MBString &code = SBDialoguePropertyManager::GetInstance()->GetCharacterCode(mfPrimaryActor);
                    mDialogueLines[mCurrentLine][5] = code.c_str()[0];
                    mDialogueLines[mCurrentLine][6] = code.c_str()[1];
                }
                mSoundPlaying = true;
                MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
                broadcaster->BroadcastEvent(700, 701, (void *)mDialogueLines[mCurrentLine].c_str());
            }
            MBDebugStr("\tnow playing %s\n", mDialogueLines[mCurrentLine].c_str());
            mCurrentLine++;
            return true;
        }
        MBDebugStr("*** line group finished ***\n\n");
        mCurrentLine = 0;
        mSoundPlaying = false;
        mActive = false;
        mfPrimaryActor->SendMessageA(0, "HCDoneTalking");
        SetProperties(mTrueVector, 1, mfPrimaryActor);
        SetProperties(mFalseVector, 0, mfPrimaryActor);
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (mExitEvent != "null") {
            MfUserHookData *data = new MfUserHookData;
            data->event = mExitEvent.c_str();
            data->data = mExitEventData.c_str();
            broadcaster->BroadcastEvent(300, 303, data);
            delete data;
        }
        broadcaster->BroadcastEvent(700, 208, &mLineGroupName);
    }
    return false;
}

// 0x10053510 ?ListenToEvent@SBdLineGroup@@UAEXJJPAX@Z
void SBdLineGroup::ListenToEvent(long channel, long event, void *data)
{
    if (channel == 700 && event == 702 && mActive && mSoundPlaying)
        mSoundPlaying = false;
}

// 0x10053550 ?ParseConditional@SBdLineGroup@@IAEXXZ
void SBdLineGroup::ParseConditional()
{
    int nProperties = SBDialoguePropertyManager::GetInstance()->GetNumberOfProperties();
    for (int i = 0; i < nProperties; i++) {
        MBString tmpString = SBDialoguePropertyManager::GetInstance()->GetPropertyStringByOrder(i);
        const char *propertyString = tmpString.c_str();
        int pos = mConditionalString.find(propertyString);
        while (pos != -1) {
            bool space = true;
            if (pos > 0 && mConditionalString[pos - 1] != ' ')
                space = false;
            if (pos + tmpString.size() < mConditionalString.size() && mConditionalString[pos + tmpString.size()] != ' ')
                space = false;
            if (space)
                mConditionalVector.push_back(
                    MBHashString(SBDialoguePropertyManager::GetInstance()->GetPropertyStringByOrder(i).c_str()));
            pos = mConditionalString.find(propertyString, pos + 1);
        }
    }
}

// 0x100538f0 ?EvaluateConditional@SBdLineGroup@@QAE_NXZ
bool SBdLineGroup::EvaluateConditional()
{
    if (mCheckConditional) {
        for (int i = 0; i < mConditionalVector.size(); i++) {
            MBString debugString = SBDialoguePropertyManager::GetInstance()->GetPropertyString(mConditionalVector[i]);
            if (!SBDialoguePropertyManager::GetInstance()->GetProperty(mConditionalVector[i]))
                return false;
        }
    }
    return true;
}

// Matched by this code (template and inline instances):
// 0x10053aa0 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEHIIABV12@II@Z
// 0x10053b80 ?insert@?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@PAV32@ABV32@@Z
// 0x10053d80 ?size@?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QBEIXZ
// 0x10053f90 ?_Ucopy@?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAEPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@PBV32@0PAV32@@Z
// 0x10053fe0 ?_Ufill@?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@IAEXPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@IABV32@@Z
// 0x10054020 ?fill@std@@YAXPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@0ABV21@@Z
// 0x100541d0 ?copy_backward@std@@YAPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@PAV21@00@Z

// Not claimed (static initializers and header instances of this object):
// 0x10053db0 ?erase@?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@PAV32@0@Z
// 0x10054390 _$E31
// 0x100543d0 _$E27
// 0x100543f0 _$E30
