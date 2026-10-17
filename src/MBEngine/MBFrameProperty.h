// MBFrameProperty.h - the properties of an animation frame (IMBAnim, MBAnimSequence, MBAnimFrame).
#ifndef MBENGINE_MBFRAMEPROPERTY_H
#define MBENGINE_MBFRAMEPROPERTY_H

enum MBFrameProperty {
    kFrameNotify = 0,
    kFrameDelay = 1,
    kFrameStartSound = 2,
    kFrameStartAnim = 3,
    kFrameStartAnimName = 4,
    kFrameStartAnimSequence = 5,
    kFrameStopAnim = 6,
    kFrameStartScript = 7,
    kFrameStopScript = 8,
    kFrameCastMemberOffset = 9
};

#endif
