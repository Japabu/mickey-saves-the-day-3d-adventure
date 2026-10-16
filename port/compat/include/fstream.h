// fstream.h - the file streams of the pre-standard iostreams (see iostream.h). The old
// constructors and open() take a third argument, the file protection (filebuf::openprot),
// which the standard streams do not have; it is accepted and ignored.
#ifndef PORT_FSTREAM_H
#define PORT_FSTREAM_H
#include <iostream.h>
#include <fstream>

class filebuf : public std::filebuf {
public:
    static const int openprot = 0644;
    static const int sh_none = 0, sh_read = 0, sh_write = 0;
};

#define PORT_OLD_FSTREAM(name, base, mode)                                                    \
    class name : public base {                                                                \
    public:                                                                                   \
        name() {}                                                                             \
        explicit name(const char *path, std::ios_base::openmode m = mode, int = filebuf::openprot) \
            : base(path, m) {}                                                                \
        void open(const char *path, std::ios_base::openmode m = mode, int = filebuf::openprot) \
        {                                                                                     \
            base::open(path, m);                                                              \
        }                                                                                     \
    };
PORT_OLD_FSTREAM(ifstream, std::ifstream, std::ios_base::in)
PORT_OLD_FSTREAM(ofstream, std::ofstream, std::ios_base::out)
PORT_OLD_FSTREAM(fstream, std::fstream, std::ios_base::in | std::ios_base::out)
#undef PORT_OLD_FSTREAM
#endif
