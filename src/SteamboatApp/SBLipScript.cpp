#include "SBLipScript.h"

// 0x1007b5f0 ??0SBLipScript@@QAE@XZ
SBLipScript::SBLipScript()
{
    mLipPositions = 0;
    mLength = 0;
    mValid = false;
}

// 0x1007b610 ??1SBLipScript@@QAE@XZ
SBLipScript::~SBLipScript()
{
    if (mLipPositions)
    {
        MBfree_release(mLipPositions);
    }
}

// 0x1007b630 ?LoadLipScript@SBLipScript@@QAE_NAAVIMBStream@@@Z
bool SBLipScript::LoadLipScript(IMBStream &stream)
{
    unsigned long size = 4;
    int val;
    stream.Read(&val, size);
    mLength = Convert4BytesToInt(&val);
    if (mLipPositions) {
        MBfree_release(mLipPositions);
        mLipPositions = 0;
    }
    mLipPositions = new int[mLength];
    for (int i = 0; i < mLength; i++) {
        stream.Read(&val, size);
        mLipPositions[i] = Convert4BytesToInt(&val) - 1;
    }
    mValid = true;
    return true;
}

// 0x1007b700 ?Convert4BytesToInt@SBLipScript@@AAEHPAH@Z
int SBLipScript::Convert4BytesToInt(int *bytes)
{
    char *digits = (char *)bytes + 3;
    int result = 0;
    int factor = 1;
    for (int i = 0; i < 4; i++, digits--) {
        if (*digits >= '0' && *digits <= '9') {
            result += (*digits - '0') * factor;
            factor *= 10;
        }
    }
    return result;
}

// Not claimed (static initializers and header instances of this object):
// TODO 0x1007b750 $E297: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E290); its $E number depends on the original header set
// 0x1007b790 _$E18
// TODO 0x1007b7b0 $E293: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
