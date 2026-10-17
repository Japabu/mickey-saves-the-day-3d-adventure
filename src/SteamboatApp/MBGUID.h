#pragma once
// muffin/include/Com: interface and class ids (MBGUID is MBCom's).
#include "../MBCom/MBGUID.h"

class IMBUnknown;
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);
