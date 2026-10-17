// MBMouseEvent.h - a mouse event.
#ifndef MBENGINE_MBMOUSEEVENT_H
#define MBENGINE_MBMOUSEEVENT_H

#include "../MBEngine/MBPoint.h"

class IMBScreen;

struct MBMouseEvent {
    MBPoint mMousePos;
    IMBScreen *mScreen;
    short mButton;
};

#endif
