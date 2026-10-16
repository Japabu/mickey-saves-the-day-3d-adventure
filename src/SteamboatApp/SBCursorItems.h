#pragma once
// SBCursorItems.h: conversion between the cursor item names and the numbers the game state keeps
// (SBCursorItems.obj, declarations from SteamboatApp.pdb).
#include "MBContainers.h"

void CursorItemStringToInt(MBString cursorString, int &cursorInt);
void CursorItemIntToString(MBString &cursorString, int cursorInt);

// The cursor items in the order of their numbers (every unit that includes this header has its
// own copy, as in the original: SteamboatNode.obj, dllmain.obj and SBCursorItems.obj).
static MBString kCursorItemString[] = {
    "none",          "PRbadsoup",   "PRbow",           "PRbucket",    "PRbutton",
    "PRcandybowl",   "PRchewyWorm", "PRcrowbar",       "PRflour",     "PRfork",
    "PRgoodsoup",    "PRhankie",    "PRmglass",        "PRmglassAndHankie", "PRpropeller",
    "PRsaucer",      "PRsheetmusic", "PRSRkeys",       "PRwand",      "PRwhistle",
    "PRwormpack",    "PRwrench",    "PRcat",           "PRsandwichW", "PRbucketPiranha",
    "PRbasket",      "PRspoon",     "PRknife",         "PRbird",      "PRcabinet",
    "PRbaitBucket"
};
const int kNumCursorItems = sizeof(kCursorItemString) / sizeof(kCursorItemString[0]);
