// items.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// the dog whistle can be played.
//
// A click on an item in the inventory bar does something only when the item's space
// codes (steamboat.ini) name the scene. The whistle's are "XX", no scene, so the click
// never reached SteamboatNode, which plays the whistle for it (hc_play_whistle; the
// player's scripts answer it in every scene, and in Daisy's yard it scares the cat away).
// The whistle is usable everywhere.
#include "SteamboatNode.h"
#include "SBItem.h"

bool PortItemUsable(SBItem *item)
{
    if (!item)
        return false;
    if (item->GetInstanceName().compare("PRwhistle0") == 0)
        return true;
    MBString space = SteamboatNode::GetSpaceCode();
    return item->IsItemActiveInSpace(space);
}
