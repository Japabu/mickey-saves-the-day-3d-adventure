#pragma once
#include "PiccoloComp_api.h"
#include "MfAiCompiler.h"
#include "MfString.h"
class MfPcBuffer;
class MfPcBufferManager;
class MfPcChunkList;
class MfPcConstantTable;
class MfPcErrorCallBack;
class MfPcExpression;
class MfPcIdentifier;
class MfPcLabelGen;
class MfPcSymbolTable;
class MfPcTokenInfo;
class MfScHandleList;
class MfDblList;
class MfStack;
class MfStrStack;
class MfStringHeap;
struct MfPcStringPool;
class MfStringList;

// The Piccolo script compiler (0x74 bytes). The parser (yacc) and the scanner
// (lex) call back into it to build the code of each function; one instance,
// pcMainCompiler, is published as "PiccoloCompilerI".
class PICCOLOCOMP_API MfPcCompiler : public MfAiCompiler {
public:
    MfPcCompiler();
    virtual ~MfPcCompiler();

    // MfAi
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release() { return 0; }
    // MfAiCompiler
    virtual int Compile(MfString const &module, MfString const &type, MfString const &source,
                        MfPcErrorCallBack *errors, int flags);
    virtual int Flags() { return mFlags; }
    virtual int CodeType() const { return mCodeType; }
    virtual void ClearCodeType() { mCodeType = 0; }
    virtual MfScHandleList &Functions() { return *mFunctions; }
    virtual int IsSymbolGlobalPermanent(MfString const &name) const;
    virtual void GetGlobalSymbolNames(MfStringList *names, int flags) const;
    virtual void FlushGlobalTemps();
    virtual int (*GetConstInitFunction())(MfString const &, MfString const &, MfString *);
    virtual int NumberOfErrors() const { return mNumberOfErrors; }
    virtual int NumberOfWarnings() const { return mNumberOfWarnings; }
    virtual MfString const &ModuleName() const { return mModuleName; }
    virtual MfString const &ModuleType() const { return mModuleType; }
    virtual void UpdatePiccoloDataPathFiles(int force);
    virtual void UpdatePiccoloLibFiles(int force);
    virtual int PiccoloLookupKeyword(MfString const &word);
    virtual MfString ParseModuleNameFromErrorInfo(MfString const &info);
    virtual int ParseLineNumberFromErrorInfo(MfString const &info);

    static void Initialize();
    static void Shutdown();
    static MfPcCompiler *GetMainCompiler();
    static void SetBufferManager(MfPcBufferManager *manager) { pbmBufferMgr = manager; }
    static int CompileInitialCode(MfString const &source, MfString const &module, MfString *errors);

    int GetCurrentLineNumber() const { return mCurrentLineNumber; }
    MfPcSymbolTable &SymbolTable() const { return *mSymbolTable; }
    MfPcConstantTable &ConstantTable() const { return *mConstantTable; }
    MfString &CurrentContinueLabel() { return mCurrentContinueLabel; }
    MfString &CurrentBreakLabel() { return mCurrentBreakLabel; }
    MfString &CurrentTestLabel() { return mCurrentTestLabel; }
    MfString &CurrentReturnLabel() { return mCurrentReturnLabel; }
    MfString &CurrentReturn0Label() { return mCurrentReturn0Label; }
    MfPcLabelGen &Labeler() { return *mLabeler; }
    int SwitchLevel() const { return mSwitchLevel; }

    // errors
    void CompileError(MfPcTokenInfo *token, int warning, int code);
    void CompileError(MfPcTokenInfo *token, int warning, int code, MfString const &a);
    void CompileError(MfPcTokenInfo *token, int warning, int code, MfString const &a, MfString const &b);
    void CompileError(MfPcTokenInfo *token, int warning, int code, MfString const &a, MfString const &b,
                      MfString const &c);
    void ConstSetError(MfString const &name, int warning, int code, MfString const &a);
    void SyntaxError();
    // scanner
    int YYInput(char *buffer, int size);
    void LexMatchedText(char *text, int length, MfPcTokenInfo *token);
    void LexGotCharacter();
    void LexGotNewLine();
    // statements
    void StartLoop();
    void EndLoop();
    void StartSwitch();
    void EndSwitch();
    void AddCase(MfPcExpression *value, MfString *label);
    int AddDefault(MfString *label);
    void PopRuntimeStackValue(MfPcChunkList *chunks);
    void AsmNewArrayInst(int size, MfPcChunkList *chunks);
    void AsmSetArrayItem(int index, MfPcChunkList *chunks);
    void SetupReturnLabels();
    void BeginFunction(MfString const &name);
    void EndFunction(MfPcChunkList *chunks, int numArgs, int flags);
    void GenerateFunctionProlog(MfPcChunkList *chunks);
    void GenerateFunctionEpilog(MfPcChunkList *chunks);
    void AppendLineNumber(MfPcChunkList *chunks, int line);
    void PrependLineNumber(MfPcChunkList *chunks, int line);
    void AppendBlockLine(MfPcChunkList *chunks, int line, int isEnd);
    void PrependBlockLine(MfPcChunkList *chunks, int line, int isEnd);
    void AppendCodeLevel(MfPcChunkList *chunks, int level);
    void PrependCodeLevel(MfPcChunkList *chunks, int level);
    void AppendVariableInfo(MfPcChunkList *chunks, MfPcIdentifier *variable, int level);
    void PrependVariableInfo(MfPcChunkList *chunks, MfPcIdentifier *variable, int level);
    void AppendConstUsage(MfPcChunkList *chunks, MfPcIdentifier *constant);
    void PrependConstUsage(MfPcChunkList *chunks, MfPcIdentifier *constant);
    void BuildWhileLoop(MfPcChunkList *chunks, MfPcExpression *test, MfPcChunkList *body, int line);
    void BuildDoWhileLoop(MfPcChunkList *chunks, MfPcChunkList *body, MfPcExpression *test, int line);
    void BuildForLoop(MfPcChunkList *chunks, MfPcChunkList *init, MfPcExpression *test,
                      MfPcExpression *step, MfPcChunkList *body, int line, int endLine);
    void BuildIfElse(MfPcChunkList *chunks, MfPcExpression *test, MfPcChunkList *thenPart,
                     MfPcChunkList *elsePart, int line);
    void BuildSwitch(MfPcChunkList *chunks, MfPcExpression *value, MfPcChunkList *body, int line);

private:
    void CompileError(MfPcTokenInfo *token, int warning, MfString const &message);
    void AssembleJumpInstructions(MfPcChunkList *chunks);
    void FindJumpLabels(MfPcChunkList *chunks);
    void ResetSwitchTables();

    static MfPcBufferManager *pbmBufferMgr;
    static MfPcCompiler *pcMainCompiler;

public:
    MfPcBuffer *mBuffer;                // +0x04 source being compiled
    int mReadPosition;                  // +0x08 YYInput's position in mBuffer
    int mCurrentLineNumber;             // +0x0c
    int mColumn;                        // +0x10
    int mPosition;                      // +0x14 characters scanned
    MfString mMatchedText;              // +0x18 text of the last token
    MfPcStringPool *mStringPool;        // +0x1c token texts
    MfString mFunctionName;             // +0x20
    MfString mModuleName;               // +0x24
    MfString mModuleType;               // +0x28
    int mNumberOfErrors;                // +0x2c
    int mNumberOfWarnings;              // +0x30
    MfPcErrorCallBack *mErrorCallBack;  // +0x34
    MfPcSymbolTable *mSymbolTable;      // +0x38
    MfPcConstantTable *mConstantTable;  // +0x3c
    MfStrStack *mLabelStack;            // +0x40 saved loop labels
    MfString mCurrentContinueLabel;     // +0x44
    MfString mCurrentBreakLabel;        // +0x48
    MfString mCurrentTestLabel;         // +0x4c
    MfString mCurrentReturnLabel;       // +0x50
    MfString mCurrentReturn0Label;      // +0x54
    MfPcLabelGen *mLabeler;             // +0x58
    int mSwitchLevel;                   // +0x5c
    MfStack *mSwitchStack;              // +0x60
    MfDblList *mSwitchTable;            // +0x64 MfPcSwitchElement *
    int mCodeType;                      // +0x68
    MfScHandleList *mFunctions;         // +0x6c compiled routines
    int mFlags;                         // +0x70
};
