#include "SBCursorItems.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x10045310 ?CursorItemStringToInt@@YAXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAH@Z
void CursorItemStringToInt(MBString cursorString, int &cursorInt)
{
    cursorInt = 0;
    for (int i = 0; i < kNumCursorItems; i++) {
        if (cursorString == kCursorItemString[i] || cursorString == kCursorItemString[i] + "0") {
            cursorInt = i;
            break;
        }
    }
}

// 0x10045520 ?CursorItemIntToString@@YAXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@H@Z
void CursorItemIntToString(MBString &cursorString, int cursorInt)
{
    cursorString = "invalid string";
    if (cursorInt >= 0 && cursorInt < kNumCursorItems)
        cursorString = kCursorItemString[cursorInt];
}

// 0x10044cd0 _$E18
// 0x10044cf0 _$E15
// 0x100452c0 _$E17
// 0x100452e0 _$E16
// 0x10045750 _$E30
// 0x10045790 _$E26
// 0x100457b0 _$E29
