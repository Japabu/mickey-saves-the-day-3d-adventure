#pragma once
#include "MfPcParserItem.h"
#include "MfString.h"
class MfPcIdentifier;

// Kinds of code chunks (MfPcCodeChunk::mType).
enum {
    PC_CHUNK_CODE = 0,          // an instruction
    PC_CHUNK_JUMP = 1,          // MfPcChunkJump
    PC_CHUNK_LABEL = 2,         // MfPcChunkLabel
    PC_CHUNK_LINEINFO = 3,      // MfPcChunkLineInfo
    PC_CHUNK_CODELEVEL = 4,     // MfPcChunkCodeLevel
    PC_CHUNK_VARIABLEINFO = 5,  // MfPcChunkVariableInfo
    PC_CHUNK_BLOCKLINE = 6,     // MfPcChunkBlockLine
    PC_CHUNK_CONSTUSAGE = 7     // MfPcChunkConstUsage
};

// One instruction of the generated code: its bytecode (mCode, mSize bytes) and
// its assembler text (*mText). The emitters return 0 = ok, 1 = operand out of
// range, 2 = bad register or addressing kind, 3 = bad opcode.
class MfPcCodeChunk : public MfPcParserItem {
public:
    MfPcCodeChunk();
    MfPcCodeChunk(int type);
    virtual ~MfPcCodeChunk();
    void CopyCode(unsigned char *&p);
    int Push(int kind, int value);      // kind: 0 [CPP+v], 1 [FP+v], 3..6 REG0..3, 7 THIS_OBJ
    int PushImmediate(int value);
    int Pop(int kind, int value);       // kind: 1 [FP+v], 3..6 REG0..3
    int Clear(int reg);                 // reg 3..6
    int StackOp(unsigned char op);      // swap, dup
    int AddSP(int n);
    int SubSP(int n);
    int Incr(int n);
    int Arithmetic(unsigned char op);
    int Invoke(unsigned char op);
    int Return();
    int GetProp(unsigned char op, int value);
    int PropStack(unsigned char op);
    int ArrayItem(unsigned char op);
    int Var(unsigned char op, int value);
    int New(unsigned char op, int value);
    int Jmp(int offset);
    int JmpZero(int offset);
    int JmpNotZero(int offset);
    unsigned char *AllocCode(int size);
    void Put8(char v, unsigned char *&p);
    void Put16(int v, unsigned char *&p);
    void Put32(int v, unsigned char *&p);
    int NumBytes(int v);                // 1, 2 or 4
    int SwitchTable(int n);
    int SwitchEntry(int value);

    int mSize;                  // +0x0c bytes of code
    int mType;                  // +0x10 PC_CHUNK_*
    int mOffset;                // +0x14 offset in the routine
    MfString *mText;            // +0x18 listing text
    unsigned char *mCode;       // +0x1c
};

// Source line of the code that follows (0x24 bytes).
class MfPcChunkLineInfo : public MfPcCodeChunk {
public:
    MfPcChunkLineInfo(int line);
    virtual ~MfPcChunkLineInfo();
    int mLine;                  // +0x20
};

// Block nesting level of the code that follows (0x24 bytes).
class MfPcChunkCodeLevel : public MfPcCodeChunk {
public:
    MfPcChunkCodeLevel(int level);
    virtual ~MfPcChunkCodeLevel();
    int mLevel;                 // +0x20
};

// Begin or end of a block (0x28 bytes).
class MfPcChunkBlockLine : public MfPcCodeChunk {
public:
    MfPcChunkBlockLine(int line, int isEnd);
    virtual ~MfPcChunkBlockLine();
    int mLine;                  // +0x20
    int mIsEnd;                 // +0x24
};

// A named constant the code uses (0x24 bytes).
class MfPcChunkConstUsage : public MfPcCodeChunk {
public:
    MfPcChunkConstUsage(MfPcIdentifier *constant);
    virtual ~MfPcChunkConstUsage();
    MfPcIdentifier *mConstant;  // +0x20
};

// A jump target (0x24 bytes).
class MfPcChunkLabel : public MfPcCodeChunk {
public:
    MfPcChunkLabel(MfString &name);
    virtual ~MfPcChunkLabel();
    MfString *mName;            // +0x20
};

// A (conditional) jump to a label (0x30 bytes).
class MfPcChunkJump : public MfPcCodeChunk {
public:
    enum { kJmp = 0, kJmpZero = 1, kJmpNotZero = 2 };
    MfPcChunkJump(int kind, MfString &label);
    virtual ~MfPcChunkJump();
    void Assemble();
    int DisplacementSize();
    int mKind;                  // +0x20
    int mDisplacement;          // +0x24
    MfString *mLabel;           // +0x28
    MfPcChunkLabel *mTarget;    // +0x2c
};

// A local variable: name, frame slot and code level (0x2c bytes).
class MfPcChunkVariableInfo : public MfPcCodeChunk {
public:
    MfPcChunkVariableInfo(MfPcIdentifier *variable, int level);
    virtual ~MfPcChunkVariableInfo();
    MfString mName;             // +0x20
    int mSlot;                  // +0x24
    int mLevel;                 // +0x28
};
