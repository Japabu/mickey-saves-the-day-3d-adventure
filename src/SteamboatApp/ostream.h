#pragma once
#include "ios.h"
class ios;
class streambuf;

class ostream {
public:
    virtual ~ostream();
    int opfx();
    long tellp();
    ostream & flush();
    ostream & operator<<(char const *);
    ostream & operator<<(double);
    ostream & operator<<(int);
    ostream & operator<<(long);
    ostream & operator<<(ostream & (__cdecl*)(ostream &));
    ostream & operator<<(streambuf *);
    ostream & operator<<(unsigned char);
    ostream & seekp(long);
    ostream & seekp(long, ios::seek_dir);
    ostream(streambuf *);
    void osfx();
protected:
    ostream & operator=(streambuf *);
    ostream();
    ostream(ostream const &);
private:
    ostream & writepad(char const *, char const *);
public:
    int field_4;
};
