#include "ScriptManager.h"

// 0x10118960 ?AddRef@ScriptManager@@UAEKXZ
unsigned long ScriptManager::AddRef()
{
    return ++_mRefCnt;
}
