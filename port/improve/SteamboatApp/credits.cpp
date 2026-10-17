// credits.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// a click ends the credits (so do Space, Enter and Escape, in OnKeyDown).
#include "SBCreditsNode.h"

void SBCreditsNode::OnLMouseUp(MBMouseEvent &evt)
{
    SBActivityNode::OnLMouseUp(evt);
    Done();
}
