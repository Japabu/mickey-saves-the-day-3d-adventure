#pragma once
template <class T0> class field_;
template <class T0> class unknown_;
class MfScHandleList;
class MfString;

class MfPcCodeLoader {
public:
    MfPcCodeLoader & operator=(MfPcCodeLoader const &);
    MfPcCodeLoader();
    static int __cdecl UpdateSingleFile(MfString const &, int, MfScHandleList *);
    static void __cdecl UpdatePiccoloDataPathFiles(int);
    static void __cdecl UpdatePiccoloLibFiles(int);
    ~MfPcCodeLoader();
private:
    static void __cdecl UpdateFilesInDirectoryPath(MfString const &, int);
public:
    char field_0;
};
