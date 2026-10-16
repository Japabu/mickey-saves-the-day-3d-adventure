#include "MfScConsoleManager_3.h"
#include "MfScConsoleManager.h"

// 0x10008660 ??4MfScConsoleManager@@QAEAAV0@ABV0@@Z
MfScConsoleManager & MfScConsoleManager::operator=(MfScConsoleManager const &other)
{
    field_4 = other.field_4;
    return *this;
}
