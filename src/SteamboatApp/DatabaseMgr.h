#pragma once
#include <string>
template <class T0> class MBAllocator;
class IMBFileSpec;
class IMBResourceFile;

class DatabaseMgr {
public:
    class Database {
    public:
        Database(IMBResourceFile *);
        unsigned long AddDBRef();
        unsigned long ReleaseDBRef();
        ~Database();
        int field_0;
        int field_4;
    };
    DatabaseMgr();
    bool AddDatabase(char const *);
    bool Initialize(std::basic_string<char,std::char_traits<char>,MBAllocator<char> > const &, std::basic_string<char,std::char_traits<char>,MBAllocator<char> > const &);
    bool RemoveDatabase(char const *);
    ~DatabaseMgr();
protected:
    bool AddDirDatabase(IMBFileSpec *, char const *);
    bool AddZipDatabase(IMBFileSpec *, char const *);
public:
    char field_0;
    char field_1;
    char unknown_2[2];
    int field_4;
    char field_8;
    char unknown_9[3];
    int field_c;
    int field_10;
    int field_14;
};
