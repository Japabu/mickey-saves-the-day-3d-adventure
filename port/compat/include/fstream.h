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

// The file a game path names on the virtual file system (port_host_path in
// platform/win32/crtfs.cpp): read only, an existing file (0); written, the overlay's file
// (1), with the CD's contents copied there first when it is appended to or also read (2).
inline const char *port_fstream_path(const char *path, std::ios_base::openmode m, char *buf, size_t size)
{
    int write = !(m & std::ios_base::out) && !(m & std::ios_base::app) ? 0
                : (m & (std::ios_base::app | std::ios_base::in)) ? 2 : 1;
    return port_host_path(path, write, buf, size) == 0 ? buf : path;
}

#define PORT_OLD_FSTREAM(name, base, mode)                                                    \
    class name : public base {                                                                \
    public:                                                                                   \
        name() {}                                                                             \
        explicit name(const char *path, std::ios_base::openmode m = mode, int = filebuf::openprot) \
        {                                                                                     \
            open(path, m);                                                                    \
        }                                                                                     \
        void open(const char *path, std::ios_base::openmode m = mode, int = filebuf::openprot) \
        {                                                                                     \
            char host[1024];                                                                  \
            base::open(port_fstream_path(path, m | (mode), host, sizeof host), m);              \
        }                                                                                     \
    };
PORT_OLD_FSTREAM(ifstream, std::ifstream, std::ios_base::in)
PORT_OLD_FSTREAM(ofstream, std::ofstream, std::ios_base::out)
PORT_OLD_FSTREAM(fstream, std::fstream, std::ios_base::in | std::ios_base::out)
#undef PORT_OLD_FSTREAM
#endif
