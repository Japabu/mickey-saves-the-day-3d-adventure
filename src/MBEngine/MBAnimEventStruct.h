// MBAnimEventStruct.h - the data of an animation event.
#ifndef MBENGINE_MBANIMEVENTSTRUCT_H
#define MBENGINE_MBANIMEVENTSTRUCT_H

class IMBAnim;

struct MBAnimEventStruct {
    IMBAnim *anim;
    unsigned long id;
    long sequence;
    long frame;
};

#endif
