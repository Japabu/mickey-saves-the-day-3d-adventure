// MfStateCompiler (RuntimeLib.dll 10084df0-10085380).
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "MfStateCompiler.h"
#include "MfFSMManager.h"
#include "MfFiniteStateMachine.h"
#include "MfMsgTypeTable.h"
#include "MfStateProperty.h"

class MfNullString;
extern __declspec(dllimport) MfNullString NULL_STRING;

// the scanner and parser of behaviour files (1009c940-)
void ReleaseParseStrings();                                 // 1009cc50
void yyrestart(FILE *input);                                // 1009c940
void SetScanCompiler(MfStateCompiler *compiler);            // 1009cc60
void SetParseCompiler(MfStateCompiler *compiler);           // 1009cc80
int yyparse();                                              // 1009cc90

MfStateCompiler *gStateCompiler;
// the message types (made and deleted with the compiler)
MfMsgTypeTable *g_msgTypes;

// 0x10084df0 ??0MfStateCompiler@@QAE@XZ
MfStateCompiler::MfStateCompiler()
{
    gStateCompiler = this;
    mPos = 0;
    mLine = 0;
    m20 = 0;
    m24 = 0;
    mManager = 0;
    g_msgTypes = new MfMsgTypeTable;
}

// 0x10084ec0 ??1MfStateCompiler@@UAE@XZ
MfStateCompiler::~MfStateCompiler()
{
    delete g_msgTypes;
    g_msgTypes = 0;
    gStateCompiler = 0;
}

// 0x10084f70 ?SyntaxError@MfStateCompiler@@QAEXXZ
void MfStateCompiler::SyntaxError()
{
    int pos = mChar - 1;
    MfString unused;
    MfString message;
    char line[256];
    if (pos < 0)
        pos = 0;
    message = "Syntax Error on line: ";
    message << mLine << " ";
    char *text = (char *)mText;
    char *p = text + pos;
    while (*p != '\n' && pos > 0) {
        p--;
        pos--;
    }
    int i = 0;
    p++;
    for (; *p != '\n' && i < 255; i++)
        line[i] = *p++;
    line[i] = 0;
    message << " \n";
    message << line << "\nBehavior: " << mFile;
    MessageBox(0, (char *)message, "Syntax Error", MB_ICONEXCLAMATION);
}

// 0x100850c0 ?ReadInput@MfStateCompiler@@QAEHPADH@Z
int MfStateCompiler::ReadInput(char *buffer, int size)
{
    int left = mText.Length() - mPos;
    if (left <= 0)
        return 0;
    if (size >= left)
        size = left;
    memcpy(buffer, (char *)mText + mPos, size);
    mPos += size;
    return size;
}

// 0x10085120 ?NextChar@MfStateCompiler@@QAEXXZ
void MfStateCompiler::NextChar()
{
    mChar++;
    mColumn++;
}

// 0x10085130 ?NextLine@MfStateCompiler@@QAEXXZ
void MfStateCompiler::NextLine()
{
    mLine++;
    mColumn = 0;
}

// 0x10085140 ?Advance@MfStateCompiler@@QAEXPBDH@Z
void MfStateCompiler::Advance(const char *token, int)
{
    mChar += strlen(token);
    mColumn += strlen(token);
}

// 0x10085180 ?Compile@MfStateCompiler@@QAEXABVMfString@@000HHPAVMfFSMManager@@@Z
void MfStateCompiler::Compile(const MfString &name, const MfString &text, const MfString &dir,
                              const MfString &file, int a5, int mode, MfFSMManager *manager)
{
    mDir = dir;
    mFile = file;
    mMode = mode;
    ReleaseParseStrings();
    yyrestart(0);
    mManager = manager;
    mText = text;
    SetScanCompiler(this);
    SetParseCompiler(this);
    mPos = 0;
    mLine = 1;
    mColumn = 0;
    mChar = 0;
    m1c = NULL_STRING;
    mName = name;
    if (mName == NULL_STRING)
        mName = "<state code>";
    m20 = 0;
    m24 = 0;
    m28 = a5;
    yyparse();
    ReleaseParseStrings();
}

// 0x10085250 ?Finish@MfStateCompiler@@QAEXPAVMfStatePropertyList@@@Z
void MfStateCompiler::Finish(MfStatePropertyList *machines)
{
    if (mMode == 1) {
        mManager = new MfFSMManager(mDir, 0, 0, 0);
        machines->mOwnsItems = 0;
        delete machines;
    } else {
        MfDblListItem *pli;
        mManager->AddMachine((MfFiniteStateMachine *)machines->GetFirst(pli));
        machines->mOwnsItems = 0;
        delete machines;
    }
}
