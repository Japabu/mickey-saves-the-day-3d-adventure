#include "MBMilesSoundChannel_unit.h"

// 0x100434c0 ??0MBMilesSoundChannel@@QAE@J@Z
MBMilesSoundChannel::MBMilesSoundChannel(long channelID)
: MBSoundChannel(channelID)
{
}

// 0x10043510 ??1MBMilesSoundChannel@@UAE@XZ
MBMilesSoundChannel::~MBMilesSoundChannel()
{
}

// 0x10043520 ?CreateSoundEngine@MBMilesSoundChannel@@MBEPAVMBSoundEngine@@PAVIMBSoundDescription@@@Z
MBSoundEngine *MBMilesSoundChannel::CreateSoundEngine(IMBSoundDescription *soundDescription) const
{
    MBSoundEngine *engine;
    if (soundDescription->IsStreaming())
        engine = new MBMilesStreamingSoundEngine;
    else
        engine = new MBMilesSoundEngine;
    if (engine)
        engine->SetRateMultiplier(mRateMultiplier);
    return engine;
}


// Matched by this code:
// 0x100434e0 ??_GMBMilesSoundChannel@@UAEPAXI@Z
// 0x10043580 _$E19
// 0x100435a0 _$E18
