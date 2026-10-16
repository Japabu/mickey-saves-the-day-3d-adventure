#pragma once
// SBMouseEvent: what a click on a hotspot or clickable does (declaration from SteamboatApp.pdb).
#include "MBContainers.h"

struct SBMouseEvent {
    long cursorID;
    MBString action;
    MBString data;
};
