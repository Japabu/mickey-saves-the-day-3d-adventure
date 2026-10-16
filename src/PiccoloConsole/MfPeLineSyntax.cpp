// PiccoloConsole.dll 0x1001a310-0x1001a40f: MfPeLineSyntax, the tokens the
// syntax colouring found on a line.
#include "PiccoloConsole.h"

// 0x1001a310 ?Add@MfPeLineSyntax@@QAEXABVMfPeMarker@@@Z
void MfPeLineSyntax::Add(const MfPeMarker &marker)
{
    mMarkers.Set(mCount++, marker);
}

// 0x1001a340 ?Grow@?$MfArray@VMfPeMarker@@@@QAEXI@Z
