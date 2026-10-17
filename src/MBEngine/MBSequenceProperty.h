// MBSequenceProperty.h - the properties of an animation sequence (IMBAnim, MBAnimSequence).
#ifndef MBENGINE_MBSEQUENCEPROPERTY_H
#define MBENGINE_MBSEQUENCEPROPERTY_H

enum MBSequenceProperty {
    kSequenceComment = 0,
    kSequenceZPlane = 1,
    kSequenceDelay = 2,
    kSequenceInSync = 3,
    kSequenceEndAction = 4,
    kSequenceJumpSequence = 5,
    kSequenceJumpRandomSequenceMin = 6,
    kSequenceJumpRandomSequenceMax = 7,
    kSequenceTileWidth = 8,
    kSequenceTileHeight = 9
};

#endif
