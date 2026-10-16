// fstream.h - the file streams of the pre-standard iostreams (see iostream.h).
#ifndef PORT_FSTREAM_H
#define PORT_FSTREAM_H
#include <iostream.h>
#include <fstream>

using std::ifstream;
using std::ofstream;
using std::fstream;

// filebuf::openprot, the default protection argument of the old open(), is accepted
// and ignored by the open() overloads below.
class filebuf : public std::filebuf {
public:
    static const int openprot = 0644;
    static const int sh_none = 0, sh_read = 0, sh_write = 0;
};
#endif
