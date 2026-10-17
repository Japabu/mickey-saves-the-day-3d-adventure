// SquishFileOpenFlags.h - how a SquishFile is opened.
#ifndef MBENGINE_SQUISHFILEOPENFLAGS_H
#define MBENGINE_SQUISHFILEOPENFLAGS_H

enum SquishFileOpenFlags {
    kSquishFileOpenReadBit = 1,
    kSquishFileOpenWriteBit = 2,
    kSquishFileOpenMustExist = 4
};

#endif
