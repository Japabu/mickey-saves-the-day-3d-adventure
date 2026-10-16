// PiccoloComp.dll MfPcCodeChunk.cpp (0x10004330-0x10005ad0): one instruction of
// the generated code and the emitters that assemble it.
#include <string.h>
#include "MfPcCodeChunk.h"
#include "MfScBytecodes.h"

// 0x10004330 ??0MfPcCodeChunk@@QAE@XZ
MfPcCodeChunk::MfPcCodeChunk()
    : mSize(0), mType(PC_CHUNK_CODE), mCode(0)
{
    mText = new MfString;
}

// 0x100043b0 ??_GMfPcCodeChunk@@UAEPAXI@Z

// 0x100043d0 ??0MfPcCodeChunk@@QAE@H@Z
MfPcCodeChunk::MfPcCodeChunk(int type)
    : mSize(0), mType(type), mOffset(0), mCode(0)
{
    mText = new MfString;
}

// 0x10004450 ??1MfPcCodeChunk@@UAE@XZ
MfPcCodeChunk::~MfPcCodeChunk()
{
    delete mText;
    if (mCode)
        delete mCode;
}

// 0x100044d0 ?CopyCode@MfPcCodeChunk@@QAEXAAPAE@Z
void MfPcCodeChunk::CopyCode(unsigned char *&p)
{
    if (mCode) {
        memcpy(p, mCode, mSize);
        p += mSize;
    }
}

#define BCSIZE(op) MfScBytecodes::aiBytecodeSize[op]

// 0x10004510 ?Push@MfPcCodeChunk@@QAEHHH@Z
int MfPcCodeChunk::Push(int kind, int value)
{
    unsigned char *p;
    switch (kind) {
    case 0:
        if (NumBytes(value) > 2 && value < 0)
            return 1;
        p = AllocCode(BCSIZE(2));
        *p = 2;
        p++;
        Put16(value, p);
        *mText << "\tpush\t\t[CPP + " << value << "]";
        return 0;
    case 1:
        if (NumBytes(value) > 1)
            return 1;
        p = AllocCode(BCSIZE(1));
        *p = 1;
        p++;
        Put8(value, p);
        *mText << "\tpush\t\t[FP + " << value << "]";
        return 0;
    case 3:
        p = AllocCode(BCSIZE(7));
        *p = 7;
        p++;
        *mText << "\tpush\t\tREG0";
        return 0;
    case 4:
        p = AllocCode(BCSIZE(8));
        *p = 8;
        p++;
        *mText << "\tpush\t\tREG1";
        return 0;
    case 5:
        p = AllocCode(BCSIZE(9));
        *p = 9;
        p++;
        *mText << "\tpush\t\tREG2";
        return 0;
    case 6:
        p = AllocCode(BCSIZE(10));
        *p = 10;
        p++;
        *mText << "\tpush\t\tREG3";
        return 0;
    case 7:
        p = AllocCode(BCSIZE(15));
        *p = 15;
        p++;
        *mText << "\tpush\t\tTHIS_OBJ";
        return 0;
    }
    return 2;
}

// 0x10004730 ?PushImmediate@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::PushImmediate(int value)
{
    int n = NumBytes(value);
    unsigned char *p = AllocCode(n + 1);
    *mText << "\tpushim\t\t" << value;
    if (n == 1) {
        *p = 4;
        p++;
        Put8(value, p);
        *mText << "\t; 8-bit";
    } else if (n == 2) {
        *p = 5;
        p++;
        Put16(value, p);
        *mText << "\t; 16-bit";
    } else {
        *p = 6;
        p++;
        Put32(value, p);
        *mText << "\t; 32-bit";
    }
    return 0;
}

// 0x10004810 ?Pop@MfPcCodeChunk@@QAEHHH@Z
int MfPcCodeChunk::Pop(int kind, int value)
{
    unsigned char *p;
    switch (kind) {
    case 1:
        if (NumBytes(value) > 1)
            return 1;
        p = AllocCode(BCSIZE(3));
        *p = 3;
        p++;
        Put8(value, p);
        *mText << "\tpop\t\t[FP + " << value << "]";
        return 0;
    case 3:
        p = AllocCode(BCSIZE(11));
        *p = 11;
        p++;
        *mText << "\tpop\t\tREG0";
        return 0;
    case 4:
        p = AllocCode(BCSIZE(12));
        *p = 12;
        p++;
        *mText << "\tpop\t\tREG1";
        return 0;
    case 5:
        p = AllocCode(BCSIZE(13));
        *p = 13;
        p++;
        *mText << "\tpop\t\tREG2";
        return 0;
    case 6:
        p = AllocCode(BCSIZE(14));
        *p = 14;
        p++;
        *mText << "\tpop\t\tREG3";
        return 0;
    }
    return 2;
}

// 0x100049a0 ?Clear@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::Clear(int reg)
{
    switch (reg) {
    case 3:
        *AllocCode(BCSIZE(0x10)) = 0x10;
        *mText << "\tclear\t\tREG0";
        return 0;
    case 4:
        *AllocCode(BCSIZE(0x11)) = 0x11;
        *mText << "\tclear\t\tREG1";
        return 0;
    case 5:
        *AllocCode(BCSIZE(0x12)) = 0x12;
        *mText << "\tclear\t\tREG2";
        return 0;
    case 6:
        *AllocCode(BCSIZE(0x13)) = 0x13;
        *mText << "\tclear\t\tREG3";
        return 0;
    }
    return 2;
}

// 0x10004a80 ?StackOp@MfPcCodeChunk@@QAEHE@Z
int MfPcCodeChunk::StackOp(unsigned char op)
{
    if (op == 0x14) {
        *AllocCode(BCSIZE(0x14)) = 0x14;
        *mText = "\tswap";
        return 0;
    }
    if (op == 0x15) {
        *AllocCode(BCSIZE(0x15)) = 0x15;
        *mText = "\tdup";
        return 0;
    }
    return 3;
}

// 0x10004af0 ?AddSP@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::AddSP(int n)
{
    int size = NumBytes(n);
    if (n >= 0) {
        *mText << "\tadd\t\tSP + " << n;
        if (size == 1) {
            unsigned char *p = AllocCode(BCSIZE(0x2c));
            *p = 0x2c;
            p++;
            Put8(n, p);
            *mText << "\t; 8-bit";
            return 0;
        }
        if (size == 2) {
            unsigned char *p = AllocCode(BCSIZE(0x2d));
            *p = 0x2d;
            p++;
            Put16(n, p);
            *mText << "\t; 16-bit";
            return 0;
        }
    }
    return 1;
}

// 0x10004bc0 ?SubSP@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::SubSP(int n)
{
    int size = NumBytes(n);
    if (n >= 0) {
        *mText << "\tsub\t\tSP - " << n;
        if (size == 1) {
            unsigned char *p = AllocCode(BCSIZE(0x2e));
            *p = 0x2e;
            p++;
            Put8(n, p);
            *mText << "\t; 8-bit";
            return 0;
        }
        if (size == 2) {
            unsigned char *p = AllocCode(BCSIZE(0x2f));
            *p = 0x2f;
            p++;
            Put16(n, p);
            *mText << "\t; 16-bit";
            return 0;
        }
    }
    return 1;
}

// 0x10004c90 ?Incr@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::Incr(int n)
{
    unsigned char *p = AllocCode(BCSIZE(0x1c));
    if (NumBytes(n) > 1)
        return 1;
    *p = 0x1c;
    p++;
    Put8(n, p);
    *mText << "\tincr\t\t" << n;
    return 0;
}

// 0x10004d10 ?Arithmetic@MfPcCodeChunk@@QAEHE@Z
int MfPcCodeChunk::Arithmetic(unsigned char op)
{
    unsigned char *p = AllocCode(BCSIZE(op));
    switch (op) {
    case 0x16: *mText << "\tadd"; break;
    case 0x17: *mText << "\tsub"; break;
    case 0x18: *mText << "\tmult"; break;
    case 0x19: *mText << "\tdiv"; break;
    case 0x1a: *mText << "\tmod"; break;
    case 0x1b: *mText << "\tneg"; break;
    case 0x1d: *mText << "\tand"; break;
    case 0x1e: *mText << "\tor"; break;
    case 0x1f: *mText << "\txor"; break;
    case 0x20: *mText << "\tnot"; break;
    case 0x21: *mText << "\tlshift"; break;
    case 0x22: *mText << "\trshift"; break;
    case 0x23: *mText << "\tcmplt"; break;
    case 0x24: *mText << "\tcmplte"; break;
    case 0x25: *mText << "\tcmpgt"; break;
    case 0x26: *mText << "\tcmpgte"; break;
    case 0x27: *mText << "\tcmpeq"; break;
    case 0x28: *mText << "\tcmpneq"; break;
    case 0x29: *mText << "\tcmpand"; break;
    case 0x2a: *mText << "\tcmpor"; break;
    case 0x2b: *mText << "\tcmpnot"; break;
    default:
        return 3;
    }
    *p = op;
    return 0;
}

// 0x10004fd0 ?Invoke@MfPcCodeChunk@@QAEHE@Z
int MfPcCodeChunk::Invoke(unsigned char op)
{
    if (op == 0x30) {
        *AllocCode(BCSIZE(0x30)) = 0x30;
        *mText << "\tinvokefunc";
        return 0;
    }
    if (op == 0x31) {
        *AllocCode(BCSIZE(0x31)) = 0x31;
        *mText << "\tinvokemeth";
        return 0;
    }
    return 3;
}

// 0x10005040 ?Return@MfPcCodeChunk@@QAEHXZ
int MfPcCodeChunk::Return()
{
    *AllocCode(BCSIZE(0x33)) = 0x33;
    *mText << "\tret";
    return 0;
}

// 0x10005070 ?GetProp@MfPcCodeChunk@@QAEHEH@Z
int MfPcCodeChunk::GetProp(unsigned char op, int value)
{
    unsigned char *p = AllocCode(BCSIZE(op));
    if (NumBytes(value) > 2 || value < 0)
        return 1;
    if (op == 0x34) {
        *p = op;
        p++;
        *mText << "\tgetprop\t\t";
    } else if (op == 0x35) {
        *p = op;
        p++;
        *mText << "\tgetprop1\t";
    } else if (op == 0x4d) {
        *p = op;
        p++;
        *mText << "\tgetproptest\t";
    } else if (op == 0x4e) {
        *p = op;
        p++;
        *mText << "\tgetproptest1\t";
    } else
        return 3;
    Put16(value, p);
    *mText << "[CPP + " << value << "]";
    return 0;
}

// 0x10005180 ?PropStack@MfPcCodeChunk@@QAEHE@Z
int MfPcCodeChunk::PropStack(unsigned char op)
{
    unsigned char *p = AllocCode(BCSIZE(op));
    switch (op) {
    case 0x52: *p = op; *mText << "\tgetpropstack"; return 0;
    case 0x53: *p = op; *mText << "\tgetpropstack1"; return 0;
    case 0x54: *p = op; *mText << "\tgetpropstacktest"; return 0;
    case 0x55: *p = op; *mText << "\tgetpropstacktest1"; return 0;
    case 0x56: *p = op; *mText << "\tsetpropstack"; return 0;
    case 0x57: *p = op; *mText << "\tsetpropstack1"; return 0;
    }
    return 3;
}

// 0x10005270 ?ArrayItem@MfPcCodeChunk@@QAEHE@Z
int MfPcCodeChunk::ArrayItem(unsigned char op)
{
    unsigned char *p = AllocCode(BCSIZE(op));
    if (op == 0x38) {
        *p = op;
        *mText << "\tgetarrayitem";
        return 0;
    }
    if (op == 0x39) {
        *p = op;
        *mText << "\tsetarrayitem";
        return 0;
    }
    return 3;
}

// 0x100052e0 ?Var@MfPcCodeChunk@@QAEHEH@Z
int MfPcCodeChunk::Var(unsigned char op, int value)
{
    unsigned char *p = AllocCode(BCSIZE(op));
    if (NumBytes(value) > 2 || value < 0)
        return 1;
    if (op == 0x3a) {
        *p = op;
        p++;
        *mText << "\tgetvar\t\t";
    } else if (op == 0x3b) {
        *p = op;
        p++;
        *mText << "\tsetvar\t\t";
    } else if (op == 0x58) {
        *p = op;
        p++;
        *mText << "\tsetglobalvar\t";
    } else if (op == 0x59) {
        *p = op;
        p++;
        *mText << "\tcreateglobalvar\t";
    } else
        return 3;
    Put16(value, p);
    *mText << "[CPP + " << value << "]";
    return 0;
}

// 0x100053f0 ?New@MfPcCodeChunk@@QAEHEH@Z
int MfPcCodeChunk::New(unsigned char op, int value)
{
    unsigned char *p;
    switch (op) {
    case 0x3c:
        p = AllocCode(BCSIZE(0x3c));
        *p = 0x3c;
        p++;
        *mText << "\tnew";
        return 0;
    case 0x3d:
        p = AllocCode(BCSIZE(0x3d));
        *p = 0x3d;
        p++;
        *mText << "\tnewinteger";
        return 0;
    case 0x3e:
        p = AllocCode(BCSIZE(0x3e));
        *p = 0x3e;
        p++;
        *mText << "\tnewfloat";
        return 0;
    case 0x3f:
        p = AllocCode(BCSIZE(0x3f));
        *p = 0x3f;
        p++;
        *mText << "\tnewstring";
        return 0;
    case 0x41:
        p = AllocCode(BCSIZE(0x41));
        *p = 0x41;
        p++;
        *mText << "\tnewthread";
        return 0;
    case 0x40:
        p = AllocCode(BCSIZE(0x40));
        if (NumBytes(value) > 2 || value < 0)
            return 1;
        *p = 0x40;
        p++;
        *mText << "\tnewarray\t";
        Put16(value, p);
        *mText << "[CPP + " << value << "]";
        return 0;
    case 0x4f:
        p = AllocCode(BCSIZE(0x4f));
        *p = 0x4f;
        p++;
        *mText << "\tnewarray_dyn";
        return 0;
    }
    return 3;
}

// 0x10005640 ?Jmp@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::Jmp(int offset)
{
    int n = NumBytes(offset);
    unsigned char *p = AllocCode(n + 1);
    *mText << "\tjmp\t\t" << offset;
    if (n == 1) {
        *p = 0x42;
        p++;
        Put8(offset, p);
        *mText << "\t; 8-bit";
    } else if (n == 2) {
        *p = 0x43;
        p++;
        Put16(offset, p);
        *mText << "\t; 16-bit";
    } else {
        *p = 0x44;
        p++;
        Put32(offset, p);
        *mText << "\t; 32-bit";
    }
    return 0;
}

// 0x10005720 ?JmpZero@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::JmpZero(int offset)
{
    int n = NumBytes(offset);
    unsigned char *p = AllocCode(n + 1);
    *mText << "\tjmpzero\t\t" << offset;
    if (n == 1) {
        *p = 0x45;
        p++;
        Put8(offset, p);
        *mText << "\t; 8-bit";
    } else if (n == 2) {
        *p = 0x46;
        p++;
        Put16(offset, p);
        *mText << "\t; 16-bit";
    } else {
        *p = 0x47;
        p++;
        Put32(offset, p);
        *mText << "\t; 32-bit";
    }
    return 0;
}

// 0x10005800 ?JmpNotZero@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::JmpNotZero(int offset)
{
    int n = NumBytes(offset);
    unsigned char *p = AllocCode(n + 1);
    *mText << "\tjmpnotzero\t" << offset;
    if (n == 1) {
        *p = 0x48;
        p++;
        Put8(offset, p);
        *mText << "\t; 8-bit";
    } else if (n == 2) {
        *p = 0x49;
        p++;
        Put16(offset, p);
        *mText << "\t; 16-bit";
    } else {
        *p = 0x4a;
        p++;
        Put32(offset, p);
        *mText << "\t; 32-bit";
    }
    return 0;
}

// 0x100058e0 ?AllocCode@MfPcCodeChunk@@QAEPAEH@Z
unsigned char *MfPcCodeChunk::AllocCode(int size)
{
    mSize = size;
    if (mCode) {
        *mText = NULL_STRING;
        delete mCode;
    }
    mCode = new unsigned char[mSize];
    return mCode;
}

// 0x10005920 ?Put8@MfPcCodeChunk@@QAEXDAAPAE@Z
void MfPcCodeChunk::Put8(char v, unsigned char *&p)
{
    *p = v;
    p++;
}

// 0x10005940 ?Put16@MfPcCodeChunk@@QAEXHAAPAE@Z
void MfPcCodeChunk::Put16(int v, unsigned char *&p)
{
    *p = v;
    p++;
    *p = v >> 8;
    p++;
}

// 0x10005960 ?Put32@MfPcCodeChunk@@QAEXHAAPAE@Z
void MfPcCodeChunk::Put32(int v, unsigned char *&p)
{
    *p = v;
    p++;
    *p = v >> 8;
    p++;
    *p = v >> 16;
    p++;
    *p = v >> 24;
    p++;
}

// 0x100059a0 ?NumBytes@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::NumBytes(int v)
{
    if (v >= -128 && v <= 127)
        return 1;
    if (v >= -32768 && v <= 32767)
        return 2;
    return 4;
}

// 0x100059e0 ?SwitchTable@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::SwitchTable(int n)
{
    if (NumBytes(n) > 2 && n < 0)
        return 1;
    unsigned char *p = AllocCode(BCSIZE(0x50));
    *p = 0x50;
    p++;
    Put16(n, p);
    *mText << "\tswitchtable\t" << n;
    return 0;
}

// 0x10005a50 ?SwitchEntry@MfPcCodeChunk@@QAEHH@Z
int MfPcCodeChunk::SwitchEntry(int value)
{
    if (NumBytes(value) > 2 && value < 0)
        return 1;
    unsigned char *p = AllocCode(BCSIZE(0x51));
    *p = 0x51;
    p++;
    Put16(value, p);
    *mText << "\tswitchentry\t[CPP + " << value << "]";
    return 0;
}

