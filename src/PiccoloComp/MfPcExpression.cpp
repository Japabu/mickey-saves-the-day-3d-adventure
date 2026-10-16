// PiccoloComp.dll MfPcExpression.cpp (0x1000c670-0x1000cb70)
#include "MfPcExpression.h"
#include "MfPcLiteralExpression.h"

// 0x1000c670 ??1MfPcExpression@@UAE@XZ
MfPcExpression::~MfPcExpression()
{
    delete mTokenInfo;
}

// 0x1000c6d0 ??_GMfPcExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000c6f0 ?CreateTokenInfo@MfPcExpression@@QAEPAVMfPcTokenInfo@@XZ
MfPcTokenInfo *MfPcExpression::CreateTokenInfo()
{
    mTokenInfo = new MfPcTokenInfo;
    return mTokenInfo;
}

// 0x1000c710 ?LiteralValue@MfPcExpression@@QAEPAVMfVariant@@XZ
MfVariant *MfPcExpression::LiteralValue()
{
    if (mKind == PC_EXPR_LITERAL)
        return ((MfPcLiteralExpression *)this)->mValue;
    return 0;
}

// 0x1000c720 ?Description@MfPcExpression@@QAE?AVMfString@@XZ
MfString MfPcExpression::Description()
{
    switch (mKind) {
    case PC_EXPR_NONE:
        return MfString("Unknown expression type");
    case PC_EXPR_NULL:
        return MfString("Null expression");
    case PC_EXPR_LITERAL:
        return MfString("Constant");
    case PC_EXPR_THIS:
        return MfString("'this' object");
    case PC_EXPR_IDENTIFIER:
        return MfString("Identifier");
    case PC_EXPR_ARRAY:
        return MfString("Array lookup []");
    case PC_EXPR_FUNCTION:
        return MfString("Function call ()");
    case PC_EXPR_PROPERTY:
        return MfString("Property expression x.y");
    case PC_EXPR_POSTINCR:
        return MfString("Post increment ++");
    case PC_EXPR_POSTDECR:
        return MfString("Post decrement --");
    case PC_EXPR_PREINCR:
        return MfString("Pre-increment ++");
    case PC_EXPR_PREDECR:
        return MfString("Pre-decrement --");
    case PC_EXPR_NEGATE:
        return MfString("Negation -");
    case PC_EXPR_LOGNOT:
        return MfString("Logical NOT !");
    case PC_EXPR_BITNOT:
        return MfString("Binary NOT ~");
    case PC_EXPR_ALLOC:
        return MfString("Object Allocation, new");
    case PC_EXPR_MULT:
        return MfString("Multiplication *");
    case PC_EXPR_DIV:
        return MfString("Division /");
    case PC_EXPR_MOD:
        return MfString("Modulo %");
    case PC_EXPR_ADD:
        return MfString("Addition +");
    case PC_EXPR_SUB:
        return MfString("Subtraction -");
    case PC_EXPR_LEFTSHIFT:
        return MfString("Shift Left <<");
    case PC_EXPR_RIGHTSHIFT:
        return MfString("Shift Right >>");
    case PC_EXPR_LT:
        return MfString("Relational <");
    case PC_EXPR_GT:
        return MfString("Relational >");
    case PC_EXPR_LTE:
        return MfString("Relational <=");
    case PC_EXPR_GTE:
        return MfString("Relational >=");
    case PC_EXPR_EQUALITY:
        return MfString("Relational ==");
    case PC_EXPR_NOTEQUAL:
        return MfString("Relational !=");
    case PC_EXPR_BITAND:
        return MfString("Binary AND &");
    case PC_EXPR_BITXOR:
        return MfString("Binary XOR ^");
    case PC_EXPR_BITOR:
        return MfString("Binary OR |");
    case PC_EXPR_LOGAND:
        return MfString("Logical AND &&");
    case PC_EXPR_LOGOR:
        return MfString("Logical OR ||");
    case PC_EXPR_CONDITIONAL:
        return MfString("Conditional ?:");
    case PC_EXPR_ASSIGNMENT:
        return MfString("Assignment operator");
    case PC_EXPR_COMMA:
        return MfString("Comma operator");
    }
    return MfString("What the !@#$%%");
}
