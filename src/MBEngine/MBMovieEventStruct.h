// MBMovieEventStruct.h - the data of a movie event.
#ifndef MBENGINE_MBMOVIEEVENTSTRUCT_H
#define MBENGINE_MBMOVIEEVENTSTRUCT_H

class IMBMovie;

struct MBMovieEventStruct {
    IMBMovie *movie;
    unsigned long id;
};

#endif
