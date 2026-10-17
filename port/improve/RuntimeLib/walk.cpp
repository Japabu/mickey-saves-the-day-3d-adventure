// walk.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// a walking character's height follows the floor smoothly.
#include "MfSkeletonNode.h"
#include "MfTransform3.h"

// The floor checks of a walk (MfLocoActivity::_CheckBubble) lift or lower the character
// at once when a foot finds the floor elsewhere: when the walk starts (its legs reach
// lower than the standing pose's) and at footfalls on uneven ground, by up to a quarter
// of a leg in one frame. The height now goes there over about 70 ms instead.
void PortEaseWalkHeight(MfSkeletonNode *root, const MfTransform3 &before, unsigned int elapsed) // (the decompiled code's unsigned long is 32-bit)
{
    MfTransform3 xf = root->transform();
    float from = before.translation().mY, to = xf.translation().mY;
    if (to == from)
        return;
    float k = elapsed / 70.0f;
    if (k >= 1.0f)
        return;
    xf.translation().mY = from + (to - from) * k;
    root->setTransform(xf);
    root->syncChildTransforms(0);
}
