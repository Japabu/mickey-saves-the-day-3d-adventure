#include "SBDialogue.h"
#include "MBRuntime.h"
#include "MfAi.h"
#include "MfProperty.h"
#include "SBdBlockFirstSubsequent.h"
#include "SBdBlockFirstSubsequentRandom.h"
#include "SBdBlockFirstTimeOnly.h"
#include "SBdBlockOrdered.h"
#include "SBdBlockRandom.h"
#include "SBdBlockRandomSequential.h"
#include "SBdLineGroup.h"
#include <stdio.h>

// 0x1004cf20 ??0SBDialogue@@QAE@XZ
SBDialogue::SBDialogue() : mActiveLineGroup(0)
{
}

// 0x1004cf50 ??_GSBDialogue@@UAEPAXI@Z

// 0x1004cf90 ??1SBDialogue@@UAE@XZ
SBDialogue::~SBDialogue()
{
    for (int i = 0; i < mDBlockVector.size(); i++)
        delete mDBlockVector[i];
    mDBlockVector.clear();
}

// 0x1004d0c0 ?LoadFromPref@SBDialogue@@QAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBDialogue::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    MBString blockName;
    int blockNum = 0;
    char block[80];
    sprintf(block, "dblock%d", blockNum);
    while (pref->GetString(stream, "Dialogue", block, blockName, "")) {
        SBdBlock *tmpBlock = 0;
        MBString blockType;
        if (pref->GetString(stream, blockName, "block type", blockType, "")) {
            if (blockType == "in order then randomly")
                tmpBlock = new SBdBlockOrdered(blockName);
            else if (blockType == "in order")
                tmpBlock = new SBdBlockOrdered(blockName);
            else if (blockType == "first time, subsequent times")
                tmpBlock = new SBdBlockFirstSubsequent(blockName);
            else if (blockType == "first time only")
                tmpBlock = new SBdBlockFirstTimeOnly(blockName);
            else if (blockType == "first time, subsequent times, randomly")
                tmpBlock = new SBdBlockFirstSubsequentRandom(blockName);
            else if (blockType == "random")
                tmpBlock = new SBdBlockRandom(blockName);
            else if (blockType == "random sequential")
                tmpBlock = new SBdBlockRandomSequential(blockName);
        }
        if (!tmpBlock) {
            char s[80];
            sprintf(s, "there is no block of the specified type : %s", blockType);
        }
        tmpBlock->LoadFromPref(stream, pref);
        mDBlockVector.push_back(tmpBlock);
        blockNum++;
        sprintf(block, "dblock%d", blockNum);
    }
}

// EQUIV 0x1004d8d0 ?Update@SBDialogue@@QAE_NPAVMfAiActor@@@Z: reviewed: the remaining differences are register and slot choices (the original keeps i in a stack slot and MBDebugStr in ebx, VC6 keeps i in ebx); calls, arguments, conditions and their order agree (the GetProperty result is now stored before the test, so the MfString temporary dies first as in the original). Was: register allocation differs (this in ebp, i in a stack slot in the original)
bool SBDialogue::Update(MfAiActor *mfPrimaryActor)
{
    if (mActiveLineGroup) {
        if (mActiveLineGroup->IsActive()) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(700, 207, 0);
            mActiveLineGroup->PlayDialogue(mfPrimaryActor);
            return true;
        }
        mActiveLineGroup = 0;
        return true;
    }
    bool sendMessage = false;
    int i;
    for (i = 0; i < mDBlockVector.size(); i++) {
        SBdBlock *block = mDBlockVector[i];
        if (block->EvaluateBlock()) {
            MBDebugStr("\n*** active dialog block is %s ***\n", block->GetBlockName().c_str());
            mActiveLineGroup = block->GetLineGroup(mfPrimaryActor);
            if (mActiveLineGroup) {
                MBDebugStr("*** active line group is %s ***\n", mActiveLineGroup->GetLineGroupName().c_str());
                sendMessage = true;
                mActiveLineGroup->MakeActive();
                mfPrimaryActor->SendMessageA(0, "HCStartTalking");
                MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
                broadcaster->BroadcastEvent(700, 207, 0);
                break;
            }
            MBDebugStr("no active line group found for block %s\n", block->GetBlockName().c_str());
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(700, 206, 0);
        }
    }
    for (i++; i < mDBlockVector.size(); i++)
        mDBlockVector[i]->ClearClick();
    if (!sendMessage && mfPrimaryActor) {
        MfProperty property;
        int found = mfPrimaryActor->GetProperty("HCTalking", property);
        if (found && (int)property)
            mfPrimaryActor->SendMessageA(0, "HCDoneTalking");
    }
    return false;
}

// Matched by this code (template and inline instances):
// 0x1004d070 ??1?$MBVector@PAVSBdBlock@@@@QAE@XZ
// 0x1004dd20 ?insert@?$vector@PAVSBdBlock@@V?$MBAllocator@PAVSBdBlock@@@@@std@@QAEXPAPAVSBdBlock@@IABQAV3@@Z

// Not claimed (static initializers and header instances of this object):
// TODO 0x1004dfc0 $E302: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E295); its $E number depends on the original header set
// 0x1004e000 _$E21
// TODO 0x1004e020 $E298: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
