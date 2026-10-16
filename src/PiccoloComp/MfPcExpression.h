#pragma once
#include "MfPcParserItem.h"
#include "MfPcTokenInfo.h"
#include "MfString.h"
class MfPcChunkList;
class MfPcCompiler;
class MfVariant;

// Expression kinds (MfPcExpression::mKind).
enum {
    PC_EXPR_NONE = 0,
    PC_EXPR_NULL = 1,
    PC_EXPR_LITERAL = 2,
    PC_EXPR_THIS = 3,
    PC_EXPR_NILOBJ = 4,
    PC_EXPR_IDENTIFIER = 5,
    PC_EXPR_ARRAY = 6,
    PC_EXPR_FUNCTION = 7,
    PC_EXPR_PROPERTY = 8,
    PC_EXPR_POSTINCR = 9,
    PC_EXPR_POSTDECR = 0xa,
    PC_EXPR_PREINCR = 0xb,
    PC_EXPR_PREDECR = 0xc,
    PC_EXPR_NEGATE = 0xd,
    PC_EXPR_LOGNOT = 0xe,
    PC_EXPR_BITNOT = 0xf,
    PC_EXPR_PROPERTYTEST = 0x10,
    PC_EXPR_ALLOC = 0x11,
    PC_EXPR_MULT = 0x12,
    PC_EXPR_DIV = 0x13,
    PC_EXPR_MOD = 0x14,
    PC_EXPR_ADD = 0x15,
    PC_EXPR_SUB = 0x16,
    PC_EXPR_LEFTSHIFT = 0x17,
    PC_EXPR_RIGHTSHIFT = 0x18,
    PC_EXPR_LT = 0x19,
    PC_EXPR_GT = 0x1a,
    PC_EXPR_LTE = 0x1b,
    PC_EXPR_GTE = 0x1c,
    PC_EXPR_EQUALITY = 0x1d,
    PC_EXPR_NOTEQUAL = 0x1e,
    PC_EXPR_BITAND = 0x1f,
    PC_EXPR_BITXOR = 0x20,
    PC_EXPR_BITOR = 0x21,
    PC_EXPR_LOGAND = 0x22,
    PC_EXPR_LOGOR = 0x23,
    PC_EXPR_CONDITIONAL = 0x24,
    PC_EXPR_ASSIGNMENT = 0x25,
    PC_EXPR_COMMA = 0x26
};

// An expression of the parse tree (0x14 bytes). Optimize folds constants and
// returns the expression to generate; Generate appends the bytecode to a
// chunk list (load: push the value, else store into it).
class MfPcExpression : public MfPcParserItem {
public:
    MfPcExpression() : mKind(PC_EXPR_NONE), mTokenInfo(0) {}
    virtual ~MfPcExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks) { return this; }
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual int IsLValue() { return 0; }
    MfPcTokenInfo *CreateTokenInfo();
    MfVariant *LiteralValue();
    MfString Description();

    int mKind;                  // +0x0c PC_EXPR_*
    MfPcTokenInfo *mTokenInfo;  // +0x10
};
