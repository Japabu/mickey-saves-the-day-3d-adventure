// PiccoloComp.dll MfPcConstSet.cpp (0x1000b810-0x1000bf70): constant set
// files.
#include <stdio.h>
#include "MfPcConstSet.h"
#include "MfPcCompiler.h"
#include "MfPcConstSetParser.h"
#include "MfPcSymbolTable.h"
#include "MfPathSearch.h"
#include "MfScPiccolo.h"

// The constant set error messages, by code: the format and how many '%s' it takes.
struct MfPcConstSetMessage {
    MfPcConstSetMessage(MfString const &format, int numArgs) : mFormat(format), mNumArgs(numArgs) {}
    MfString mFormat;
    int mNumArgs;
};

static MfPcConstSetMessage aErrors[] = {
    MfPcConstSetMessage("That was no error. That was my mom!!", 0),
    MfPcConstSetMessage("Cannot open constant set file '%s'.", 1),
    MfPcConstSetMessage("Cannot find constant set file '%s'.", 1),
    MfPcConstSetMessage("The identifier '%s' is not defined.", 1),
    MfPcConstSetMessage("The identifier '%s' must be a constant.", 1),
    MfPcConstSetMessage("The constant expression must evaluate to a constant value.", 0),
    MfPcConstSetMessage("The constant '%s' already has a value.", 1),
};

// 0x1000b810 _$E11 (static initializer: aErrors)
// 0x1000b820 _$E8 (constructs aErrors)
// 0x1000b9d0 _$E10 (registers _$E9 with atexit)
// 0x1000b9e0 _$E9 (destroys aErrors)

// 0x1000ba00 ??0MfPcConstSet@@QAE@ABVMfString@@PAVMfPcCompiler@@PAX@Z
MfPcConstSet::MfPcConstSet(MfString const &name, MfPcCompiler *compiler, void *declarations)
    : mDeclarations(declarations), mCompiler(compiler), mName(name), mFile(0), mLine(1)
{
}

// 0x1000ba80 ??_GMfPcConstSet@@UAEPAXI@Z (compiler-generated)

// 0x1000baa0 ??1MfPcConstSet@@UAE@XZ
MfPcConstSet::~MfPcConstSet()
{
}

// 0x1000baf0 ?Load@MfPcConstSet@@QAEHXZ
int MfPcConstSet::Load()
{
    int loaded = 0;
    MfString file = mName + MfString(".pcs");
    MfString path(MfScPiccolo::fsDataPath);
    {
        MfPathSearch search(file, path);
        mPath = search.Search();
    }
    if (mPath == NULL_STRING) {
        MfString lib;
        lib.GetVariable("PICCOLOLIB");
        if (lib != NULL_STRING) {
            MfPathSearch search(file, lib);
            mPath = search.Search();
        }
    }
    if (mPath != NULL_STRING) {
        mFile = fopen(mPath, "r");
        if (!mFile) {
            Error(0, 1, mPath);
            return 0;
        }
        mLine = 1;
        csrestart(0);
        CsSetLexConstSet(this);
        CsSetParseConstSet(this);
        CsLexReset();
        csparse();
        fclose(mFile);
        mFile = 0;
        loaded = 1;
    } else
        Error(0, 2, file);
    return loaded;
}

// 0x1000bd70 ?LexGotCharacter@MfPcConstSet@@QAEXXZ
void MfPcConstSet::LexGotCharacter()
{
}

// 0x1000bd80 ?LexGotNewLine@MfPcConstSet@@QAEXXZ
void MfPcConstSet::LexGotNewLine()
{
    mLine++;
}

// 0x1000bd90 ?YYInput@MfPcConstSet@@QAEHPADH@Z
int MfPcConstSet::YYInput(char *buffer, int size)
{
    return fread(buffer, 1, size, mFile);
}

// 0x1000bdb0 ?SyntaxError@MfPcConstSet@@QAEXXZ
void MfPcConstSet::SyntaxError()
{
    MfString message("Syntax Error");
    mCompiler->ConstSetError(mPath, 0, mLine, message);
}

// 0x1000be20 ?Error@MfPcConstSet@@QAEXHH@Z
void MfPcConstSet::Error(int warning, int code)
{
    MfString message;
    message.Format(aErrors[code].mFormat);
    mCompiler->ConstSetError(mPath, warning, mLine, message);
}

// 0x1000beb0 ?Error@MfPcConstSet@@QAEXHHABVMfString@@@Z
void MfPcConstSet::Error(int warning, int code, MfString const &arg)
{
    MfString message;
    message.Format(aErrors[code].mFormat, (char *)arg);
    mCompiler->ConstSetError(mPath, warning, mLine, message);
}

// 0x1000bf50 ?Find@MfPcConstSet@@QAEPAVMfPcIdentifier@@ABVMfString@@@Z
MfPcIdentifier *MfPcConstSet::Find(MfString const &name)
{
    return mCompiler->mSymbolTable->Find(name);
}
