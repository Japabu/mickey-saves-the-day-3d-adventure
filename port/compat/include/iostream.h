// iostream.h - the pre-standard iostreams of Visual C++ (msvcirt), on the standard
// library. The classes are the std ones under their old global names; `ios` is a
// scope that adds the old names (seek_dir, nocreate, noreplace) to std::ios.
#ifndef PORT_IOSTREAM_H
#define PORT_IOSTREAM_H
#include <ios>
#include <istream>
#include <ostream>
#include <iostream>
#include <streambuf>

using std::istream;
using std::ostream;
using std::iostream;
using std::streambuf;
using std::cin;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::ends;
using std::flush;
using std::ws;
using std::dec;
using std::hex;
using std::oct;
typedef std::istream istream_withassign;
typedef std::ostream ostream_withassign;
typedef std::iostream iostream_withassign;

class ios : public std::ios {
public:
    typedef std::ios_base::seekdir seek_dir;
    typedef std::ios_base::openmode open_mode;
    // the standard streams create and replace by their mode alone
    static const std::ios_base::openmode nocreate = 0;
    static const std::ios_base::openmode noreplace = 0;
private:
    ios();
};
#endif
