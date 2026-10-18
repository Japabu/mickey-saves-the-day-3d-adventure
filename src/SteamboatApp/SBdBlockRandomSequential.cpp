#include "SBdBlockRandomSequential.h"
#include "SBDialoguePropertyManager.h"
#include "SBdLineGroup.h"
#include <stdlib.h>

// 0x1004b1c0 ??0SBdBlockRandomSequential@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBdBlockRandomSequential::SBdBlockRandomSequential(MBString &blockName) : SBdBlock(blockName)
{
}

// 0x1004b210 ??_GSBdBlockRandomSequential@@UAEPAXI@Z

// 0x1004b250 ??1SBdBlockRandomSequential@@UAE@XZ
SBdBlockRandomSequential::~SBdBlockRandomSequential()
{
    for (int i = 0; i < mGeneratedLineGroups.size(); i++)
        delete mGeneratedLineGroups[i];
    mGeneratedLineGroups.clear();
}

// 0x1004b370 ?LoadFromPref@SBdBlockRandomSequential@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z
void SBdBlockRandomSequential::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    SBdBlock::LoadFromPref(stream, pref);
    if (pref->GetString(stream, mBlockName, "block type", mBlockType, "")) {
        if (mBlockType.compare(MBString("random sequential")) == 0) {
        }
    }
}

// TODO 0x1004b640 ?GetLineGroup@SBdBlockRandomSequential@@UAEPAVSBdLineGroup@@PAVMfAiActor@@@Z: (81% similar) not matching yet (frame layout and zero register)
SBdLineGroup *SBdBlockRandomSequential::GetLineGroup(MfAiActor *mfPrimaryActor)
{
    MBString lineGroupName("");
    int val = SBDialoguePropertyManager::GetInstance()->GetProperty(mSectionID);
    SBdLineGroup *newLineGroup = new SBdLineGroup(lineGroupName);
    for (MBVector<SBdLineGroup *>::iterator it = mLineGroups.begin(); it != mLineGroups.end(); it++) {
        MBVector<MBString> lines;
        (*it)->GetLineVector(lines);
        if (lines.size() != 0) {
            int line = lines.size() > 1 ? rand() % lines.size() : 0;
            newLineGroup->AddLine(lines[line]);
        }
    }
    if (mLineGroups.size() != 0) {
        MBString exitEvent;
        MBString exitEventData;
        mLineGroups[mLineGroups.size() - 1]->GetExitEvent(exitEvent);
        newLineGroup->SetExitEvent(exitEvent);
        mLineGroups[mLineGroups.size() - 1]->GetExitEventData(exitEventData);
        newLineGroup->SetExitEventData(exitEventData);
        MBVector<unsigned long> trueVector;
        MBVector<unsigned long> falseVector;
        mLineGroups[mLineGroups.size() - 1]->GetPropertyVectors(trueVector, falseVector);
        newLineGroup->SetPropertyVectors(trueVector, falseVector);
    }
    mGeneratedLineGroups.push_back(newLineGroup);
    SBDialoguePropertyManager::GetInstance()->SetProperty(mSectionID, val + 1, mfPrimaryActor);
    return newLineGroup;
}

// Matched by this code (template and inline instances):
// 0x1004bae0 ?AddLine@SBdLineGroup@@QAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
// 0x1004bb80 ?SetExitEvent@SBdLineGroup@@QAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
// 0x1004bd40 ?SetExitEventData@SBdLineGroup@@QAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
// 0x1004bf00 ?SetPropertyVectors@SBdLineGroup@@QAEXV?$MBVector@K@@0@Z
// 0x1004bfc0 ??4?$MBVector@K@@QAEAAV0@ABV0@@Z
// 0x1004c1a0 ??0?$MBVector@K@@QAE@ABV0@@Z
// 0x1004c220 ??0?$vector@KV?$MBAllocator@K@@@std@@QAE@ABV01@@Z
// 0x1004c2a0 ??1?$vector@KV?$MBAllocator@K@@@std@@QAE@XZ
// 0x1004c2f0 ??4?$vector@KV?$MBAllocator@K@@@std@@QAEAAV01@ABV01@@Z
// 0x1004c4f0 ??0?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE@ABV?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@Z
// 0x1004c520 ??1?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE@XZ
// 0x1004c5b0 ??4?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEAAV01@ABV01@@Z
// 0x1004c850 ??A?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@1@I@Z
// 0x1004c870 ?_Ucopy@?$vector@KV?$MBAllocator@K@@@std@@IAEPAKPBK0PAK@Z
// 0x1004c8b0 ?insert@?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAEXPAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@2@IABV32@@Z
// 0x1004cc20 ?_Construct@std@@YAXPAKABK@Z
// 0x1004ce20 ??_G?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEPAXI@Z

// Not claimed (static initializers and header instances of this object):
// TODO 0x1004cea0 $E302: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E295); its $E number depends on the original header set
// 0x1004cee0 _$E25
// TODO 0x1004cf00 $E298: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
