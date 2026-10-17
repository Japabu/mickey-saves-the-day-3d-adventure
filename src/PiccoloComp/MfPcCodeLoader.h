// MfPcCodeLoader: compiles the Piccolo source files of the data and library
// paths whose object files are out of date. No data members, all static.
#ifndef MFPCCODELOADER_H
#define MFPCCODELOADER_H
#include "PiccoloComp_api.h"
class MfScHandleList;
class MfString;

class PICCOLOCOMP_API MfPcCodeLoader {
public:
    // 0x10005ad0 (inline, emitted by CodeLoader.cpp); operator= at 0x10005ae0 is
    // compiler-generated.
    MfPcCodeLoader() {}
    ~MfPcCodeLoader();
    static void __cdecl UpdatePiccoloDataPathFiles(int force);
    static void __cdecl UpdatePiccoloLibFiles(int force);
    static int __cdecl UpdateSingleFile(const MfString &sourcePath, int force, MfScHandleList *handles);
private:
    static void __cdecl UpdateFilesInDirectoryPath(const MfString &dirPath, int force);
};

#endif
