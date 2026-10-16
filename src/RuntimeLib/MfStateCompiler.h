// MfStateCompiler: reads a behaviour file (the yacc parser and lex scanner of
// the "mf_hfsm" text format call back into it for the input and the
// position) and hands the machines it builds to an MfFSMManager. RTTI name;
// vtable 100b6094; unit 10084df0-10085380.
#ifndef RUNTIMELIB_MFSTATECOMPILER_H
#define RUNTIMELIB_MFSTATECOMPILER_H
#include "MfString.h"
class MfFSMManager;
class MfStatePropertyList;

class MfStateCompiler {
public:
    MfStateCompiler();
    virtual ~MfStateCompiler();

    // parses text; mode 1 makes a new manager for the machines, else they go
    // to manager
    void Compile(const MfString &name, const MfString &text, const MfString &dir,
                 const MfString &file, int, int mode, MfFSMManager *manager);
    // the parser's result: the list of the machines read
    void Finish(MfStatePropertyList *machines);
    // the scanner reads up to size characters of the text
    int ReadInput(char *buffer, int size);
    // the scanner's position
    void NextChar();
    void NextLine();
    void Advance(const char *token, int);
    // shows the line the parser stopped at
    void SyntaxError();

    MfString mName;                                                             // 0x04
    MfString mText;                                                             // 0x08
    int mPos;                                                                   // 0x0c read position in mText
    int mLine;                                                                  // 0x10
    int mColumn;                                                                // 0x14
    int mChar;                                                                  // 0x18
    MfString m1c;                                                               // 0x1c
    int m20, m24, m28;                                                          // 0x20
    MfFSMManager *mManager;                                                     // 0x2c
    int mMode;                                                                  // 0x30
    MfString mDir;                                                              // 0x34
    MfString mFile;                                                             // 0x38
};

// the compiler that is running (the scanner and parser use it)
extern MfStateCompiler *gStateCompiler;

#endif
