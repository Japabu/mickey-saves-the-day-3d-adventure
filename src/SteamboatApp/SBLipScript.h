#pragma once
// SBLipScript.h: the mouth positions of one spoken line, read from its lip-sync resource
// (SBLipScript.obj, declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class SBLipScript {
public:
    SBLipScript();
    ~SBLipScript();
    int GetLength() { return mLength; }
    int GetVal(int i) { return mLipPositions[i]; }
    bool Valid() { return mValid; }
    void Invalidate() { mValid = false; }
    bool LoadLipScript(IMBStream &stream);
private:
    int Convert4BytesToInt(int *bytes);
    int *mLipPositions;                                 // +0x0000
    int mLength;                                        // +0x0004
    bool mValid;                                        // +0x0008
};
