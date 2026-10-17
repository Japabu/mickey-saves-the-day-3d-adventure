// PiccoloComp.dll MfPcCompiler.cpp (0x10006c80-0x1000b150): the compiler
// object: errors, the scanner and parser callbacks, labels, loops and switches,
// function prolog/epilog and the assembly of a function's code.
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "MfPcCompiler.h"
#include "MfPcBuffer.h"
#include "MfPcBufferManager.h"
#include "MfPcChunkList.h"
#include "MfPcCodeChunk.h"
#include "MfPcCodeLoader.h"
#include "MfPcConstantTable.h"
#include "MfPcErrorInfo.h"
#include "MfPcExpression.h"
#include "MfPcIdentifier.h"
#include "MfPcLabelGen.h"
#include "MfPcParser.h"
#include "MfPcStringErrorCB.h"
#include "MfPcStringListErrorCB.h"
#include "MfPcStringPool.h"
#include "MfPcSwitchElement.h"
#include "MfPcSymbolTable.h"
#include "MfPcTokenInfo.h"
#include "../PiccoloRT/MfAiFunctionRegistry.h"
#include "MfInterfaceRegistry.h"
#include "MfScBytecodeRoutine.h"
#include "../PiccoloRT/MfScDataHandle.h"
#include "../PiccoloRT/MfScDebugInfo.h"
#include "MfScHandleList.h"
#include "MfScPiccolo.h"
#include "MfScPiccoloObjectFile.h"
#include "MfStack.h"
#include "MfStrStack.h"
#include "MfStringList.h"
#include "MfDictionary.h"
#include "MfScOpcodes.h"
#include "MfVariant.h"

// The compile error messages, by code: the format and how many '%s' it takes.
struct MfPcErrorMessage {
    MfPcErrorMessage(MfString const &format, int numArgs) : mFormat(format), mNumArgs(numArgs) {}
    MfString mFormat;
    int mNumArgs;
};

static MfPcErrorMessage aErrors[] = {
    MfPcErrorMessage("'%s' unexpected outside of loop.", 1),
    MfPcErrorMessage("'%s' unexpected outside of loop.", 1),
    MfPcErrorMessage("'%s' unexpected outside of function.", 1),
    MfPcErrorMessage("Missing ';' before '%s'", 1),
    MfPcErrorMessage("'%s' : unexpected token before ';'", 1),
    MfPcErrorMessage("Function must have a name.", 0),
    MfPcErrorMessage("Error in function arguments.", 0),
    MfPcErrorMessage("Error in function body.", 0),
    MfPcErrorMessage("'%s': Label is already defined.", 1),
    MfPcErrorMessage("'%s': Variable is already defined.", 1),
    MfPcErrorMessage("'%s': Too many initializers.", 1),
    MfPcErrorMessage("'%s': Constant needs constant initializer.", 1),
    MfPcErrorMessage("'%s': Array has too many initializers.", 1),
    MfPcErrorMessage("'%s': An array cannot be a constant.", 1),
    MfPcErrorMessage("'%s': Array sizes must be greater than zero.", 1),
    MfPcErrorMessage("'%s': The array size must be an integer value.", 1),
    MfPcErrorMessage("Assigning a constant value '%s'.", 1),
    MfPcErrorMessage("Invalid LValue '%s'.", 1),
    MfPcErrorMessage("Invalid constant expression.", 0),
    MfPcErrorMessage("'%s': A global constant must appear outside a function.", 1),
    MfPcErrorMessage("Invalid array name declaration.", 0),
    MfPcErrorMessage("Invalid array size expression.", 0),
    MfPcErrorMessage("An array must be an identifier, property, function or array.", 0),
    MfPcErrorMessage("A function must be an identifier, property, function or array.", 0),
    MfPcErrorMessage("A property must be an identifier, property, function, array or 'this'.", 0),
    MfPcErrorMessage("Increment operation valid only on identifiers, properties, or arrays", 0),
    MfPcErrorMessage("Decrement operation valid only on identifiers, properties, or arrays", 0),
    MfPcErrorMessage("Increment operation valid only on identifiers, properties, or arrays", 0),
    MfPcErrorMessage("Decrement operation valid only on identifiers, properties, or arrays", 0),
    MfPcErrorMessage("'%s': Array must have a non-zero size or be initialized", 1),
    MfPcErrorMessage("Invalid use of 'nil' in an expression.", 0),
    MfPcErrorMessage("Character '%s' was unexpected.", 1),
    MfPcErrorMessage("A newline was seen in a string literal.", 0),
    MfPcErrorMessage("A newline was seen in a character literal.", 0),
    MfPcErrorMessage("Extra character '%s' in literal is ignored.", 1),
    MfPcErrorMessage("End-of-file seen before end of comment starting on line '%s' ", 1),
    MfPcErrorMessage("'new' allocation of integer needs zero or one arguments.", 0),
    MfPcErrorMessage("'new' allocation of float needs zero or one arguments.", 0),
    MfPcErrorMessage("'new' allocation of string needs zero or one arguments.", 0),
    MfPcErrorMessage("'new' allocation of thread needs exactly two arguments.", 0),
    MfPcErrorMessage("Constant %s is being redefined. Old value = '%s'.", 2),
    MfPcErrorMessage("Constant must be initialized.", 0),
    MfPcErrorMessage("'%s': unexpected outside of switch.", 1),
    MfPcErrorMessage("'%s': has already been defined for this switch.", 1),
    MfPcErrorMessage("'new' allocation expected 'Object' keyword.", 0),
    MfPcErrorMessage("'new' allocation of object needs zero arguments.", 0),
    MfPcErrorMessage("Unary $ operator requires a property expression.", 0),
    MfPcErrorMessage("The subexpression for the property test must be a property.", 0),
    MfPcErrorMessage("'new' allocation of array needs exactly one argument.", 0),
    MfPcErrorMessage("Invalid 'use' statement. Type '%s' is illegal.", 1),
    MfPcErrorMessage("Error loading constant set '%s'.", 1),
    MfPcErrorMessage("'%s': 'var' declaration not allowed outside a function.", 1),
    MfPcErrorMessage("'%s': 'global var' declaration not allowed outside a function.", 1),
    MfPcErrorMessage("The module contains illegal function definitions.", 0),
    MfPcErrorMessage("The module must contain only function defintions.", 0),
};

MfPcBufferManager *MfPcCompiler::pbmBufferMgr;
MfPcCompiler *MfPcCompiler::pcMainCompiler;

// 0x10006c80 ??0MfPcErrorInfo@@QAE@ABVMfString@@00PAVMfPcTokenInfo@@@Z (inline, dllexport class)
// 0x10006d20 ??0MfPcErrorInfo@@QAE@ABV0@@Z (compiler-generated)
// 0x10006da0 ??4MfPcErrorInfo@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10006de0 ??1MfPcErrorInfo@@QAE@XZ (compiler-generated)
// 0x10006e40 _$E14 (static initializer: aErrors)
// 0x10006e50 _$E11 (constructs aErrors)
// 0x10007990 _$E13 (registers _$E12 with atexit)
// 0x100079a0 _$E12 (destroys aErrors)

// 0x100079d0 ?CallBack@MfPcStringErrorCB@@UAEXPAVMfIPcErrorInfo@@@Z
void MfPcStringErrorCB::CallBack(MfIPcErrorInfo *info)
{
    *mErrors << info->GetModule() << "\n";
    *mErrors << info->GetMessageA() << "\n";
    if (!mBrief) {
        MfStringList lines;
        info->FormatLine(lines);
        MfDblListItem *pli;
        for (MfString *line = lines.FirstString(pli); line; line = lines.NextString(pli))
            *mErrors << *line << "\n";
    }
}

// 0x10007b00 ?CallBack@MfPcStringListErrorCB@@UAEXPAVMfIPcErrorInfo@@@Z
void MfPcStringListErrorCB::CallBack(MfIPcErrorInfo *info)
{
    mErrors->Add(info->GetModule());
    mErrors->Add(info->GetMessageA());
    if (!mBrief) {
        MfStringList lines;
        info->FormatLine(lines);
        mErrors->Merge(lines, MfDblList::LP_AFTER, 0);
    }
}

// 0x10007c00 ??0MfPcCompiler@@QAE@XZ
MfPcCompiler::MfPcCompiler()
    : mBuffer(0)
{
    MfPcParserItem::Initialize();
    mSymbolTable = new MfPcSymbolTable;
    mConstantTable = new MfPcConstantTable;
    mLabelStack = new MfStrStack;
    mLabeler = new MfPcLabelGen;
    mCodeType = 0;
    mFunctions = new MfScHandleList;
    mSwitchLevel = 0;
    mSwitchStack = new MfStack;
    mSwitchTable = 0;
}

// 0x10007da0 ??_GMfStrStack@@UAEPAXI@Z (new MfStrStack)
// 0x10007dc0 ??_GMfStack@@UAEPAXI@Z (new MfStack)
// 0x10007de0 ??_GMfScHandleList@@UAEPAXI@Z (new MfScHandleList)

// 0x10007e00 ??1MfPcCompiler@@UAE@XZ
MfPcCompiler::~MfPcCompiler()
{
    delete mSymbolTable;
    delete mConstantTable;
    delete mLabelStack;
    if (mLabeler)
        delete mLabeler;
    delete mFunctions;
    delete mSwitchStack;
}

// 0x10007f00 ?Initialize@MfPcCompiler@@SAXXZ
void MfPcCompiler::Initialize()
{
    if (!pcMainCompiler) {
        pcMainCompiler = new MfPcCompiler;
        MfPcBufferManager::Init();
        {
            MfString source("global TRUE = 1; global FALSE = 0;");
            MfString type("NULL");
            MfString module("Global Constants");
            pcMainCompiler->Compile(module, type, source, 0, 0);
        }
        MfInterfaceRegistry::AddInterface(pcMainCompiler);
    }
}

// 0x10008000 ?Shutdown@MfPcCompiler@@SAXXZ
void MfPcCompiler::Shutdown()
{
    if (pcMainCompiler) {
        {
            MfString name("PiccoloCompilerI");
            MfInterfaceRegistry::RemoveInterface(name);
        }
        pcMainCompiler->Functions().Clear();
        delete pcMainCompiler;
        pcMainCompiler = 0;
    }
    if (pbmBufferMgr) {
        delete pbmBufferMgr;
        pbmBufferMgr = 0;
        MfPcBufferManager::s_instance = 0;
    }
}

// 0x100080b0 ?GetInterfaceName@MfPcCompiler@@UAEXPADHAAH1@Z
void MfPcCompiler::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    MfString interfaceName("PiccoloCompilerI");
    strncpy(name, interfaceName, size - 1);
    major = 1;
    minor = 0;
}

// 0x10008140 ?GetMainCompiler@MfPcCompiler@@SAPAV1@XZ
MfPcCompiler *MfPcCompiler::GetMainCompiler()
{
    return pcMainCompiler;
}

// 0x10008150 ?ReportObjectFileError@@YAXAAVMfScPiccoloObjectFile@@ABVMfString@@@Z
static void ReportObjectFileError(MfScPiccoloObjectFile &file, MfString const &operation)
{
    MfString message;
    message = MfString("There was an error when the compiler attempted to perform the '") + operation +
              MfString("' operation on\r\n") + MfString("the object file named ") + file.ObjectFileName() +
              MfString("\r\n") + MfString("The error returned was '") + file.ECToString() + MfString("'");
    MessageBoxA(0, message, "Error", MB_ICONEXCLAMATION);
}

// 0x100083a0 ?Compile@MfPcCompiler@@UAEHABVMfString@@00PAVMfPcErrorCallBack@@H@Z
int MfPcCompiler::Compile(MfString const &module, MfString const &type, MfString const &source,
                          MfPcErrorCallBack *errors, int flags)
{
    pcdebug = 1;
    mFlags = flags;
    if (MfScPiccolo::bIsRunning) {
        MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
        if (registry && registry->Find(module, mFunctions) && mFunctions->GetCount() > 0) {
            mNumberOfErrors = 0;
            mNumberOfWarnings = 0;
            mCodeType = 2;
            return 1;
        }
    }
    pcrestart(0);
    if (!mBuffer)
        mBuffer = pbmBufferMgr->GetBuffer();
    mBuffer->SetText((MfString &)source);
    mStringPool = new MfPcStringPool(source.Length());
    PcSetLexCompiler(this);
    PcSetParseCompiler(this);
    PcLexReset();
    mReadPosition = 0;
    mCurrentLineNumber = 1;
    mColumn = 0;
    mPosition = 0;
    mMatchedText = NULL_STRING;
    SetupReturnLabels();
    mSwitchLevel = 0;
    ResetSwitchTables();
    mModuleName = module;
    if (mModuleName == NULL_STRING)
        mModuleName = "<code>";
    mModuleType = type;
    if (mModuleType == NULL_STRING)
        mModuleType = "FILE";
    mErrorCallBack = errors;
    mNumberOfErrors = 0;
    mNumberOfWarnings = 0;
    mCodeType = 0;
    mFunctions->Clear();
    mFunctionName = "<CODE CHUNK>";
    pcparse();
    mFlags = 0;
    mSymbolTable->FlushLocals();
    mConstantTable->Clear();
    MfPcParserItem::FreeItems();
    mLabeler->mNext = 1;
    PcLexReset();
    delete mStringPool;
    pbmBufferMgr->ReleaseBuffer(mBuffer);
    mBuffer = 0;
    if (mFunctions->GetCount() > 0 && mNumberOfErrors == 0) {
        MfScDataHandle *function;
        if (flags & 4) {
            for (function = mFunctions->First(); function; function = mFunctions->Next()) {
                MfScBytecodeRoutine *routine =
                    function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
                delete routine->Detach();
            }
        }
        if ((flags & 2) && mModuleName != "<code>" && mModuleType == "FILE") {
            MfScPiccoloObjectFile file;
            MfScPiccoloObjectFile::MfScPOFErrorCode error = file.Open(mModuleName, 1, 1, ((unsigned int)flags >> 2) & 2);
            if (error == MfScPiccoloObjectFile::kOk) {
                for (function = mFunctions->First(); function; function = mFunctions->Next())
                    file.AddFunction(function);
                error = file.WriteClose();
                if (error != MfScPiccoloObjectFile::kOk)
                    ReportObjectFileError(file, "WriteClose");
            } else
                ReportObjectFileError(file, "Open");
        }
    }
    return mNumberOfErrors == 0;
}

// 0x100087c0 ?CompileInitialCode@MfPcCompiler@@SAHABVMfString@@0PAV2@@Z
int MfPcCompiler::CompileInitialCode(MfString const &module, MfString const &source, MfString *errors)
{
    MfPcCompiler *compiler = GetMainCompiler();
    if (!compiler) {
        *errors << "Missing main compiler!\n";
        return 0;
    }
    MfPcStringErrorCB callBack(*errors, 0);
    return compiler->Compile(module, "NULL", source, &callBack, 0);
}

// 0x10008880 ?CompileError@MfPcCompiler@@QAEXPAVMfPcTokenInfo@@HH@Z
void MfPcCompiler::CompileError(MfPcTokenInfo *token, int warning, int code)
{
    MfString message;
    message.Format(aErrors[code].mFormat);
    CompileError(token, warning, message);
}

// 0x10008910 ?CompileError@MfPcCompiler@@QAEXPAVMfPcTokenInfo@@HHABVMfString@@@Z
void MfPcCompiler::CompileError(MfPcTokenInfo *token, int warning, int code, MfString const &a)
{
    MfString message;
    message.Format(aErrors[code].mFormat, (char *)a);
    CompileError(token, warning, message);
}

// 0x100089a0 ?CompileError@MfPcCompiler@@QAEXPAVMfPcTokenInfo@@HHABVMfString@@1@Z
void MfPcCompiler::CompileError(MfPcTokenInfo *token, int warning, int code, MfString const &a,
                                MfString const &b)
{
    MfString message;
    message.Format(aErrors[code].mFormat, (char *)a, (char *)b);
    CompileError(token, warning, message);
}

// 0x10008a40 ?CompileError@MfPcCompiler@@QAEXPAVMfPcTokenInfo@@HHABVMfString@@11@Z
void MfPcCompiler::CompileError(MfPcTokenInfo *token, int warning, int code, MfString const &a,
                                MfString const &b, MfString const &c)
{
    MfString message;
    message.Format(aErrors[code].mFormat, (char *)a, (char *)b, (char *)c);
    CompileError(token, warning, message);
}

// 0x10008ae0 ?CompileError@MfPcCompiler@@AAEXPAVMfPcTokenInfo@@HABVMfString@@@Z
void MfPcCompiler::CompileError(MfPcTokenInfo *token, int warning, MfString const &message)
{
    if (warning == 0)
        mNumberOfErrors++;
    else if (warning == 1)
        mNumberOfWarnings++;
    if (mErrorCallBack) {
        MfPcTokenInfo current;
        if (!token) {
            current.mLine = mCurrentLineNumber;
            if (mMatchedText.Length() == 0)
                current.mStartColumn = mColumn;
            else
                current.mStartColumn = mColumn - mMatchedText.Length() + 1;
            current.mEndColumn = mColumn;
            current.mText = mMatchedText;
            token = &current;
        }
        MfString line;
        mBuffer->GetLine(token->mLine, line);
        MfString where = MfString("**** ") + mModuleName + MfString("(Line ") + MfString(token->mLine) +
                         MfString("):");
        MfString text = (warning == 0 ? MfString("  ERROR   : ") : MfString("  WARNING : ")) + message;
        MfPcErrorInfo info(line, where, text, token);
        mErrorCallBack->CallBack(&info);
    }
}

// 0x10008dc0 ?ConstSetError@MfPcCompiler@@QAEXABVMfString@@HH0@Z
void MfPcCompiler::ConstSetError(MfString const &file, int warning, int line, MfString const &message)
{
    if (warning == 0)
        mNumberOfErrors++;
    else if (warning == 1)
        mNumberOfWarnings++;
    if (mErrorCallBack) {
        MfPcTokenInfo token;
        token.mLine = line;
        token.mStartColumn = 0;
        token.mEndColumn = 0;
        token.mText = "";
        MfString where = MfString("**** ") + file + MfString("(Line ") + MfString(line) + MfString("):");
        MfString text = (warning == 0 ? MfString("  ERROR   : ") : MfString("  WARNING : ")) + message;
        MfPcErrorInfo info(NULL_STRING, where, text, &token);
        mErrorCallBack->CallBack(&info);
    }
}

// 0x10009050 ?SyntaxError@MfPcCompiler@@QAEXXZ
void MfPcCompiler::SyntaxError()
{
    mMatchedText.StripWhiteSpace();
    MfPcTokenInfo token;
    token.mLine = mCurrentLineNumber;
    if (mMatchedText.Length() == 0)
        token.mStartColumn = mColumn;
    else
        token.mStartColumn = mColumn - mMatchedText.Length() + 1;
    token.mEndColumn = mColumn;
    token.mText = mMatchedText;
    int code = 3;
    if (PiccoloLookupKeyword(mMatchedText))
        code = 4;
    CompileError(&token, 0, code, mMatchedText);
}

// 0x100090d0 ?YYInput@MfPcCompiler@@QAEHPADH@Z
int MfPcCompiler::YYInput(char *buffer, int size)
{
    int left = mBuffer->mLength - mReadPosition;
    if (left <= 0)
        return 0;
    if (size < left)
        left = size;
    memcpy(buffer, mBuffer->mText + mReadPosition, left);
    mReadPosition += left;
    return left;
}

// 0x10009120 ?LexMatchedText@MfPcCompiler@@QAEXPADHPAVMfPcTokenInfo@@@Z
void MfPcCompiler::LexMatchedText(char *text, int length, MfPcTokenInfo *token)
{
    mMatchedText = text;
    mPosition += length;
    mColumn += length;
    token->mLine = mCurrentLineNumber;
    token->mStartColumn = mColumn - length + 1;
    token->mEndColumn = mColumn;
    token->mText = mStringPool->Allocate(mMatchedText.Length() + 1);
    strcpy(token->mText, mMatchedText);
}

// 0x100091b0 ?LexGotCharacter@MfPcCompiler@@QAEXXZ
void MfPcCompiler::LexGotCharacter()
{
    ++mPosition;
    ++mColumn;
}

// 0x100091c0 ?LexGotNewLine@MfPcCompiler@@QAEXXZ
void MfPcCompiler::LexGotNewLine()
{
    mCurrentLineNumber++;
    mBuffer->AddLine(mPosition);
    mColumn = 0;
}

// 0x100091e0 ?StartLoop@MfPcCompiler@@QAEXXZ
void MfPcCompiler::StartLoop()
{
    mLabelStack->Push(mCurrentContinueLabel);
    mLabelStack->Push(mCurrentBreakLabel);
    mLabelStack->Push(mCurrentTestLabel);
    mCurrentContinueLabel = mLabeler->NewLabel();
    mCurrentBreakLabel = mLabeler->NewLabel();
    mCurrentTestLabel = mLabeler->NewLabel();
}

// 0x100092c0 ?EndLoop@MfPcCompiler@@QAEXXZ
void MfPcCompiler::EndLoop()
{
    mCurrentTestLabel = mLabelStack->Pop();
    mCurrentBreakLabel = mLabelStack->Pop();
    mCurrentContinueLabel = mLabelStack->Pop();
}

// 0x10009380 ?StartSwitch@MfPcCompiler@@QAEXXZ
void MfPcCompiler::StartSwitch()
{
    mLabelStack->Push(mCurrentBreakLabel);
    mCurrentBreakLabel = mLabeler->NewLabel();
    mSwitchLevel++;
    if (mSwitchTable)
        mSwitchStack->Push(mSwitchTable);
    mSwitchTable = new MfDblList;
}

// 0x10009430 ?EndSwitch@MfPcCompiler@@QAEXXZ
void MfPcCompiler::EndSwitch()
{
    mCurrentBreakLabel = mLabelStack->Pop();
    mSwitchLevel--;
    delete mSwitchTable;
    mSwitchTable = (MfDblList *)mSwitchStack->Pop();
}

// 0x100094b0 ?AddCase@MfPcCompiler@@QAEXPAVMfPcExpression@@PAVMfString@@@Z
void MfPcCompiler::AddCase(MfPcExpression *value, MfString *label)
{
    MfString caseLabel = mLabeler->NewLabel();
    MfPcSwitchElement *element = new MfPcSwitchElement(0, caseLabel, value);
    MfDblListItem *pli;
    MfPcSwitchElement *last = (MfPcSwitchElement *)mSwitchTable->GetLast(pli);
    if (last && last->mIsDefault == 1)
        mSwitchTable->Insert(element, MfDblList::LP_BEFORE, pli);
    else
        mSwitchTable->Add(element);
    *label = caseLabel;
}

// 0x100095a0 ??_GMfPcSwitchElement@@UAEPAXI@Z (compiler-generated)
// 0x100095c0 ??1MfPcSwitchElement@@UAE@XZ (inline)

// 0x10009610 ?AddDefault@MfPcCompiler@@QAEHPAVMfString@@@Z
int MfPcCompiler::AddDefault(MfString *label)
{
    int added = 0;
    MfString defaultLabel = mLabeler->NewLabel();
    MfDblListItem *pli;
    MfPcSwitchElement *last = (MfPcSwitchElement *)mSwitchTable->GetLast(pli);
    if (!last || last->mIsDefault != 1) {
        mSwitchTable->Add(new MfPcSwitchElement(1, defaultLabel, 0));
        added = 1;
    }
    *label = defaultLabel;
    return added;
}

// 0x100096f0 ?ResetSwitchTables@MfPcCompiler@@AAEXXZ
void MfPcCompiler::ResetSwitchTables()
{
    while (!mSwitchStack->IsEmpty())
        delete (MfDblList *)mSwitchStack->Pop();
    delete mSwitchTable;
    mSwitchTable = 0;
}

// 0x10009740 ?PopRuntimeStackValue@MfPcCompiler@@QAEXPAVMfPcChunkList@@@Z
void MfPcCompiler::PopRuntimeStackValue(MfPcChunkList *chunks)
{
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->AddSP(1);
    chunks->Append(chunk);
}

// 0x100097b0 ?AsmNewArrayInst@MfPcCompiler@@QAEXHPAVMfPcChunkList@@@Z
void MfPcCompiler::AsmNewArrayInst(int size, MfPcChunkList *chunks)
{
    MfVariant value(size);
    int index = mConstantTable->Add(value);
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->New(BC_NEWARRAY, index);
    chunks->Append(chunk);
}

// 0x10009860 ?AsmSetArrayItem@MfPcCompiler@@QAEXHPAVMfPcChunkList@@@Z
void MfPcCompiler::AsmSetArrayItem(int index, MfPcChunkList *chunks)
{
    MfPcCodeChunk *chunk;
    chunk = new MfPcCodeChunk;
    chunk->PushImmediate(index);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->ArrayItem(BC_SETARRAYITEM);
    chunks->Append(chunk);
}

// 0x10009920 ?SetupReturnLabels@MfPcCompiler@@QAEXXZ
void MfPcCompiler::SetupReturnLabels()
{
    mCurrentReturnLabel = mLabeler->NewLabel();
    mCurrentReturn0Label = mLabeler->NewLabel();
}

// 0x100099b0 ?BeginFunction@MfPcCompiler@@QAEXABVMfString@@@Z
void MfPcCompiler::BeginFunction(MfString const &name)
{
    mFunctionName = name;
}

// 0x100099d0 ?EndFunction@MfPcCompiler@@QAEXPAVMfPcChunkList@@HH@Z
void MfPcCompiler::EndFunction(MfPcChunkList *chunks, int isConstant, int numArgs)
{
    if (isConstant && (mFlags & 0x40))
        CompileError(0, 0, 0x35);
    else if (!isConstant && (mFlags & 0x80))
        CompileError(0, 0, 0x36);
    else if (!mNumberOfErrors) {
    chunks->AddLineInfo();
        AssembleJumpInstructions(chunks);
        chunks->Dump();
        MfScDataHandle *function = MfScDataHandle::CreateBytecodeFunction(mFunctionName, numArgs);
        MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
        routine->mCodeSize = chunks->CodeSize();
        routine->mBytecodes = new unsigned char[routine->mCodeSize];
        chunks->CopyCode(routine->mBytecodes);
        chunks->BuildLineInfo(routine);
        chunks->BuildVariableInfo(routine);
        chunks->BuildCodeLevelInfo(routine);
        chunks->ListParameters(routine);
        chunks->BuildConstUsageInfo(routine);
        routine->ModuleName() = mModuleName;
        routine->ModuleType() = mModuleType;
        int numConstants = mConstantTable->mConstants->GetCount();
        if (numConstants == 0) {
            routine->AllocConstantPool(1);
            routine->SetConstant(0, 0);
        } else {
            routine->AllocConstantPool(numConstants);
            mConstantTable->FillConstantPool(routine->GetConstantPool());
        }
        routine->Attach(routine->Detach());
        if (isConstant)
            mCodeType = 2;
        else
            mCodeType = 1;
        mFunctions->Add(function);
        MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
        if (registry)
            registry->Add(function);
        function->Release();
    }
    mSymbolTable->Reset();
    mConstantTable->Dump();
    mConstantTable->Clear();
    mCurrentReturnLabel = NULL_STRING;
    mCurrentReturn0Label = NULL_STRING;
    mLabeler->mNext = 1;
    mFunctionName = "<CODE CHUNK>";
    SetupReturnLabels();
    mSwitchLevel = 0;
    ResetSwitchTables();
}

// 0x10009bd0 ?AssembleJumpInstructions@MfPcCompiler@@AAEXPAVMfPcChunkList@@@Z
void MfPcCompiler::AssembleJumpInstructions(MfPcChunkList *chunks)
{
    FindJumpLabels(chunks);
    chunks->SetJumpSizes(4, 0);
    chunks->AssignOffsets();
    chunks->ComputeDisplacements();
    while (chunks->ShrinkJumps(2)) {
        chunks->AssignOffsets();
        chunks->ComputeDisplacements();
    }
    while (chunks->ShrinkJumps(1)) {
        chunks->AssignOffsets();
        chunks->ComputeDisplacements();
    }
    chunks->AssembleJumps();
    chunks->Walk();
}

// EQUIV 0x10009c60 ?FindJumpLabels@MfPcCompiler@@AAEXPAVMfPcChunkList@@@Z: reviewed: (81% similar) register and slot choice only: the original keeps chunks in edi around the first and last loop (reloading it from its argument slot) and the done flag in ebp, ours keeps chunks in ebp throughout, done in the argument slot and the GetFirst address in esi; same calls, arguments and flow

void MfPcCompiler::FindJumpLabels(MfPcChunkList *chunks)
{
    MfDictionary labels(0x35);
    MfDblListItem *pli;
    MfPcCodeChunk *chunk;
    for (chunk = (MfPcCodeChunk *)chunks->MfDblList::GetFirst(pli); chunk;
         chunk = (MfPcCodeChunk *)chunks->MfDblList::GetNext(pli)) {
        if (chunk->mType == PC_CHUNK_LABEL)
            labels.Add(*((MfPcChunkLabel *)chunk)->mName, pli);
    }
    for (chunk = (MfPcCodeChunk *)chunks->MfDblList::GetFirst(pli); chunk;
         chunk = (MfPcCodeChunk *)chunks->MfDblList::GetNext(pli)) {
        if (chunk->mType == PC_CHUNK_JUMP) {
            MfDictionary visited(0x35);
            MfPcChunkJump *jump = (MfPcChunkJump *)chunk;
            int done = 0;
            do {
                MfDblListItem *item = (MfDblListItem *)labels.Find(*jump->mLabel);
                MfPcCodeChunk *next;
                if (item)
                    next = (MfPcCodeChunk *)item->GetData();
                else
                    next = 0;

                while (next && next->mType != PC_CHUNK_CODE && next->mType != PC_CHUNK_JUMP)
                    next = (MfPcCodeChunk *)chunks->MfDblList::GetNext(item);
                if (next->mType == PC_CHUNK_JUMP && ((MfPcChunkJump *)next)->mKind == MfPcChunkJump::kJmp &&
                    !visited.Find(*jump->mLabel)) {
                    visited.Add(*jump->mLabel, jump);
                    jump = (MfPcChunkJump *)next;
                } else
                    done = 1;
            } while (!done);
            if (jump != chunk)
                *((MfPcChunkJump *)chunk)->mLabel = *jump->mLabel;
        }
    }
    for (chunk = (MfPcCodeChunk *)chunks->First(pli); chunk; chunk = (MfPcCodeChunk *)chunks->Next(pli)) {
        if (chunk->mType == PC_CHUNK_JUMP) {
            MfDblListItem *item = (MfDblListItem *)labels.Find(*((MfPcChunkJump *)chunk)->mLabel);
            ((MfPcChunkJump *)chunk)->mTarget = item ? (MfPcChunkLabel *)item->GetData() : 0;
        }
    }
}

// 0x10009e40 ?GenerateFunctionProlog@MfPcCompiler@@QAEXPAVMfPcChunkList@@@Z
void MfPcCompiler::GenerateFunctionProlog(MfPcChunkList *chunks)
{
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->PushImmediate(mSymbolTable->mMaxLocals);
    chunks->Append(chunk);
    if (mSymbolTable->mMaxLocals > 0) {
        chunk = new MfPcCodeChunk;
        chunk->SubSP(mSymbolTable->mMaxLocals);
        chunks->Append(chunk);
    }
}

// 0x10009f10 ?GenerateFunctionEpilog@MfPcCompiler@@QAEXPAVMfPcChunkList@@@Z
void MfPcCompiler::GenerateFunctionEpilog(MfPcChunkList *chunks)
{
    chunks->Append(new MfPcChunkLabel(mCurrentReturn0Label));
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->Clear(3);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->Push(3, 0);
    chunks->Append(chunk);
    chunks->Append(new MfPcChunkLabel(mCurrentReturnLabel));
    chunk = new MfPcCodeChunk;
    chunk->Return();
    chunks->Append(chunk);
}

// 0x1000a060 ?AppendLineNumber@MfPcCompiler@@QAEXPAVMfPcChunkList@@H@Z
void MfPcCompiler::AppendLineNumber(MfPcChunkList *chunks, int line)
{
    chunks->Append(new MfPcChunkLineInfo(line));
}

// 0x1000a0d0 ?PrependLineNumber@MfPcCompiler@@QAEXPAVMfPcChunkList@@H@Z
void MfPcCompiler::PrependLineNumber(MfPcChunkList *chunks, int line)
{
    chunks->Prepend(new MfPcChunkLineInfo(line));
}

// 0x1000a140 ?AppendBlockLine@MfPcCompiler@@QAEXPAVMfPcChunkList@@HH@Z
void MfPcCompiler::AppendBlockLine(MfPcChunkList *chunks, int line, int isEnd)
{
    chunks->Append(new MfPcChunkBlockLine(line, isEnd));
}

// 0x1000a1b0 ?PrependBlockLine@MfPcCompiler@@QAEXPAVMfPcChunkList@@HH@Z
void MfPcCompiler::PrependBlockLine(MfPcChunkList *chunks, int line, int isEnd)
{
    chunks->Prepend(new MfPcChunkBlockLine(line, isEnd));
}

// 0x1000a220 ?AppendCodeLevel@MfPcCompiler@@QAEXPAVMfPcChunkList@@H@Z
void MfPcCompiler::AppendCodeLevel(MfPcChunkList *chunks, int level)
{
    chunks->Append(new MfPcChunkCodeLevel(level));
}

// 0x1000a290 ?PrependCodeLevel@MfPcCompiler@@QAEXPAVMfPcChunkList@@H@Z
void MfPcCompiler::PrependCodeLevel(MfPcChunkList *chunks, int level)
{
    chunks->Prepend(new MfPcChunkCodeLevel(level));
}

// 0x1000a300 ?AppendVariableInfo@MfPcCompiler@@QAEXPAVMfPcChunkList@@PAVMfPcIdentifier@@H@Z
void MfPcCompiler::AppendVariableInfo(MfPcChunkList *chunks, MfPcIdentifier *variable, int level)
{
    chunks->Append(new MfPcChunkVariableInfo(variable, level));
}

// 0x1000a370 ?PrependVariableInfo@MfPcCompiler@@QAEXPAVMfPcChunkList@@PAVMfPcIdentifier@@H@Z
void MfPcCompiler::PrependVariableInfo(MfPcChunkList *chunks, MfPcIdentifier *variable, int level)
{
    chunks->Prepend(new MfPcChunkVariableInfo(variable, level));
}

// 0x1000a3e0 ?AppendConstUsage@MfPcCompiler@@QAEXPAVMfPcChunkList@@PAVMfPcIdentifier@@@Z
void MfPcCompiler::AppendConstUsage(MfPcChunkList *chunks, MfPcIdentifier *constant)
{
    chunks->Append(new MfPcChunkConstUsage(constant));
}

// 0x1000a450 ?PrependConstUsage@MfPcCompiler@@QAEXPAVMfPcChunkList@@PAVMfPcIdentifier@@@Z
void MfPcCompiler::PrependConstUsage(MfPcChunkList *chunks, MfPcIdentifier *constant)
{
    chunks->Prepend(new MfPcChunkConstUsage(constant));
}

// 0x1000a4c0 ?IsSymbolGlobalPermanent@MfPcCompiler@@UBEHABVMfString@@@Z
int MfPcCompiler::IsSymbolGlobalPermanent(MfString const &name) const
{
    int permanent = 0;
    MfPcIdentifier *id = mSymbolTable->Find(name);
    if (id && id->IsConstant() && id->IsPermanent())
        permanent = 1;
    return permanent;
}

// 0x1000a500 ?GetGlobalSymbolNames@MfPcCompiler@@UBEXPAVMfStringList@@H@Z
void MfPcCompiler::GetGlobalSymbolNames(MfStringList *names, int sort) const
{
    MfDblListItem *pli;
    MfDblList *identifiers = mSymbolTable->mIdentifiers;
    for (MfPcIdentifier *id = (MfPcIdentifier *)identifiers->GetFirst(pli); id;
         id = (MfPcIdentifier *)identifiers->GetNext(pli)) {
        if (id->IsConstant() && id->IsPermanent())
            names->Add(id->mName);
    }
    if (sort)
        names->Sort(1);
}

// 0x1000a580 ?FlushGlobalTemps@MfPcCompiler@@UAEXXZ
void MfPcCompiler::FlushGlobalTemps()
{
    mSymbolTable->FlushGlobalTemps();
}

// 0x1000a590 ?BuildWhileLoop@MfPcCompiler@@QAEXPAVMfPcChunkList@@PAVMfPcExpression@@0H@Z
void MfPcCompiler::BuildWhileLoop(MfPcChunkList *chunks, MfPcExpression *test, MfPcChunkList *body, int line)
{
    chunks->Append(new MfPcChunkLabel(mCurrentContinueLabel));
    AppendLineNumber(chunks, line);
    test->Optimize(this, chunks)->Generate(this, chunks, 1, 0, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmpZero, mCurrentBreakLabel));
    chunks->Merge(*body, MfDblList::LP_AFTER, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, mCurrentContinueLabel));
    chunks->Append(new MfPcChunkLabel(mCurrentBreakLabel));
}

// 0x1000a6e0 ?BuildDoWhileLoop@MfPcCompiler@@QAEXPAVMfPcChunkList@@0PAVMfPcExpression@@H@Z
void MfPcCompiler::BuildDoWhileLoop(MfPcChunkList *chunks, MfPcChunkList *body, MfPcExpression *test, int line)
{
    chunks->Append(new MfPcChunkLabel(mCurrentTestLabel));
    chunks->Merge(*body, MfDblList::LP_AFTER, 0);
    chunks->Append(new MfPcChunkLabel(mCurrentContinueLabel));
    AppendLineNumber(chunks, line);
    test->Optimize(this, chunks)->Generate(this, chunks, 1, 0, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmpNotZero, mCurrentTestLabel));
    chunks->Append(new MfPcChunkLabel(mCurrentBreakLabel));
}

// 0x1000a830 ?BuildForLoop@MfPcCompiler@@QAEXPAVMfPcChunkList@@0PAVMfPcExpression@@10HH@Z
void MfPcCompiler::BuildForLoop(MfPcChunkList *chunks, MfPcChunkList *init, MfPcExpression *test,
                                MfPcExpression *step, MfPcChunkList *body, int line, int endLine)
{
    chunks->Merge(*init, MfDblList::LP_AFTER, 0);
    chunks->Append(new MfPcChunkLabel(mCurrentTestLabel));
    AppendLineNumber(chunks, line);
    MfPcChunkList *testCode = new MfPcChunkList;
    test->Optimize(this, testCode)->Generate(this, testCode, 1, 0, 0);
    if (testCode->GetCount() > 0) {
        chunks->Merge(*testCode, MfDblList::LP_AFTER, 0);
        chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmpZero, mCurrentBreakLabel));
    }
    chunks->Merge(*body, MfDblList::LP_AFTER, 0);
    chunks->Append(new MfPcChunkLabel(mCurrentContinueLabel));
    AppendLineNumber(chunks, endLine);
    MfPcChunkList *stepCode = new MfPcChunkList;
    step->Optimize(this, stepCode)->Generate(this, stepCode, 1, 0, 0);
    if (stepCode->GetCount() > 0) {
        chunks->Merge(*stepCode, MfDblList::LP_AFTER, 0);
        MfPcCodeChunk *chunk = new MfPcCodeChunk;
        chunk->AddSP(1);
        chunks->Append(chunk);
    }
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, mCurrentTestLabel));
    chunks->Append(new MfPcChunkLabel(mCurrentBreakLabel));
}

// 0x1000ab00 ??_GMfPcChunkList@@UAEPAXI@Z (compiler-generated)
// 0x1000ab20 ??1MfPcChunkList@@UAE@XZ (inline)

// 0x1000ab80 ?BuildIfElse@MfPcCompiler@@QAEXPAVMfPcChunkList@@PAVMfPcExpression@@00H@Z
void MfPcCompiler::BuildIfElse(MfPcChunkList *chunks, MfPcExpression *test, MfPcChunkList *thenPart,
                               MfPcChunkList *elsePart, int line)
{
    MfString elseLabel = mLabeler->NewLabel();
    MfString endLabel = mLabeler->NewLabel();
    AppendLineNumber(chunks, line);
    test->Optimize(this, chunks)->Generate(this, chunks, 1, 0, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmpZero, elseLabel));
    chunks->Merge(*thenPart, MfDblList::LP_AFTER, 0);
    if (elsePart) {
        chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, endLabel));
        chunks->Append(new MfPcChunkLabel(elseLabel));
        chunks->Merge(*elsePart, MfDblList::LP_AFTER, 0);
        chunks->Append(new MfPcChunkLabel(endLabel));
    } else
        chunks->Append(new MfPcChunkLabel(elseLabel));
}

// 0x1000ad40 ?BuildSwitch@MfPcCompiler@@QAEXPAVMfPcChunkList@@PAVMfPcExpression@@0H@Z
void MfPcCompiler::BuildSwitch(MfPcChunkList *chunks, MfPcExpression *value, MfPcChunkList *body, int line)
{
    MfString tableLabel = mLabeler->NewLabel();
    AppendLineNumber(chunks, line);
    value->Optimize(this, chunks)->Generate(this, chunks, 1, 0, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, tableLabel));
    MfDblListItem *item;
    MfPcCodeChunk *chunk;
    for (chunk = (MfPcCodeChunk *)body->Last(item); chunk && chunk->mType != PC_CHUNK_BLOCKLINE;
         chunk = (MfPcCodeChunk *)body->Prev(item))
        ;
    body->Insert(new MfPcChunkLineInfo(((MfPcChunkBlockLine *)chunk)->mLine), MfDblList::LP_BEFORE, item);
    chunks->Merge(*body, MfDblList::LP_AFTER, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, mCurrentBreakLabel));
    chunks->Append(new MfPcChunkLabel(tableLabel));
    MfPcChunkList *table = new MfPcChunkList;
    int numCases = 0;
    MfPcSwitchElement *defaultElement = 0;
    MfDblListItem *pli;
    MfPcSwitchElement *element;
    for (element = (MfPcSwitchElement *)mSwitchTable->GetFirst(pli); element;
         element = (MfPcSwitchElement *)mSwitchTable->GetNext(pli)) {
        if (!element->mIsDefault) {
            element->mValue->Optimize(this, table)->Generate(this, table, 1, 0x10, 0);
            numCases++;
        } else
            defaultElement = element;
    }
    for (element = (MfPcSwitchElement *)mSwitchTable->GetFirst(pli); element;
         element = (MfPcSwitchElement *)mSwitchTable->GetNext(pli)) {
        if (!element->mIsDefault)
            table->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, element->mLabel));
    }
    if (defaultElement)
        table->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, defaultElement->mLabel));
    else
        table->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, mCurrentBreakLabel));
    chunk = new MfPcCodeChunk;
    chunk->SwitchTable(numCases);
    table->Prepend(chunk);
    chunks->Merge(*table, MfDblList::LP_AFTER, 0);
    chunks->Append(new MfPcChunkLabel(mCurrentBreakLabel));
}

// 0x1000b0b0 ?UpdatePiccoloDataPathFiles@MfPcCompiler@@UAEXH@Z
void MfPcCompiler::UpdatePiccoloDataPathFiles(int force)
{
    MfPcCodeLoader::UpdatePiccoloDataPathFiles(force);
}

// 0x1000b0c0 ?UpdatePiccoloLibFiles@MfPcCompiler@@UAEXH@Z
void MfPcCompiler::UpdatePiccoloLibFiles(int force)
{
    MfPcCodeLoader::UpdatePiccoloLibFiles(force);
}

// 0x1000b0d0 ?GetConstInitFunction@MfPcCompiler@@UAEP6AHABVMfString@@0PAV2@@ZXZ
int (*MfPcCompiler::GetConstInitFunction())(MfString const &, MfString const &, MfString *)
{
    return CompileInitialCode;
}

// 0x1000b0e0 ?ParseModuleNameFromErrorInfo@MfPcCompiler@@UAE?AVMfString@@ABV2@@Z
MfString MfPcCompiler::ParseModuleNameFromErrorInfo(MfString const &info)
{
    return MfPcErrorInfo::ParseModuleName(info);
}

// 0x1000b110 ?ParseLineNumberFromErrorInfo@MfPcCompiler@@UAEHABVMfString@@@Z
int MfPcCompiler::ParseLineNumberFromErrorInfo(MfString const &info)
{
    return MfPcErrorInfo::ParseLineNumber(info);
}

// 0x1000b120 ?PiccoloLookupKeyword@MfPcCompiler@@UAEHABVMfString@@@Z
int MfPcCompiler::PiccoloLookupKeyword(MfString const &word)
{
    return PcLookupKeyword(word, word.Length());
}

// 0x1000b140 ??_EMfPcChunkList@@WBA@AEPAXI@Z (this adjustor thunk)

// 0x1000d2a0 ?AddRef@MfPcCompiler@@UAEHXZ
int MfPcCompiler::AddRef()
{
    return 1;
}
