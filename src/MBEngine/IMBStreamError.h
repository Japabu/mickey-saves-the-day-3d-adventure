// IMBStreamError.h - IMBStream's last error.
#ifndef MBENGINE_IMBSTREAMERROR_H
#define MBENGINE_IMBSTREAMERROR_H

enum IMBStreamError {
    kIMBStreamNoError = 0,
    kIMBStreamReadError = 1,
    kIMBStreamWriteError = 2,
    kIMBStreamOpenError = 3,
    kIMBStreamCloseError = 4,
    kIMBStreamSeekError = 5,
    kIMBStreamEOF = 6
};

#endif
