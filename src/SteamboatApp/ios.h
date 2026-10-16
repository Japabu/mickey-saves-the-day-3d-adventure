#pragma once
struct _CRT_CRITICAL_SECTION;
class streambuf;

class ios {
public:
    enum seek_dir { seek_dir_unknown };
    virtual ~ios();
    virtual void _slot1();
    ios(streambuf *);
    long setf(long);
    static int __cdecl xalloc();
    static long __cdecl bitalloc();
    void clear(int);
    static long const floatfield;
    static long const adjustfield;
    static long const basefield;
protected:
    ios & operator=(ios const &);
    ios();
    ios(ios const &);
    void init(streambuf *);
private:
    static int fLockcInit;
    static long * x_statebuf;
    static int x_curindex;
    static long x_maxbit;
    static _CRT_CRITICAL_SECTION x_lockc;
public:
    int field_4;
    int field_8;
    int field_c;
    int field_10;
    char unknown_14[8];
    int field_1c;
    int field_20;
    int field_24;
    int field_28;
    char field_2c;
    char unknown_2d[3];
    int field_30;
    int field_34;
};
