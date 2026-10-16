#pragma once
#include <stdio.h>
#include "MfString.h"
class MfPcCompiler;
class MfPcIdentifier;

// A constant set file (.pcs) named by a 'use' statement: found on the data
// path or PICCOLOLIB and read with its own scanner and grammar (0x1c bytes).
class MfPcConstSet {
public:
    MfPcConstSet(MfString const &name, MfPcCompiler *compiler, void *declarations);
    virtual ~MfPcConstSet();
    int Load();
    void LexGotCharacter();
    void LexGotNewLine();
    int YYInput(char *buffer, int size);
    void SyntaxError();
    void Error(int warning, int code);
    void Error(int warning, int code, MfString const &arg);
    MfPcIdentifier *Find(MfString const &name);

    void *mDeclarations;        // +0x04
    MfPcCompiler *mCompiler;    // +0x08
    MfString mName;             // +0x0c
    MfString mPath;             // +0x10
    FILE *mFile;                // +0x14
    int mLine;                  // +0x18
};
