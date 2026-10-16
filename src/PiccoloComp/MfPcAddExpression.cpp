// MfPcAddExpression.cpp (PiccoloComp.dll 0x10001000-0x10001770)
// The inline members of the exported MfPcCompiler / MfPcStringErrorCB /
// MfPcStringListErrorCB classes (including their implicit copy constructors,
// operator= and vector deleting destructor) were emitted into this object:
// 0x10001030, 0x10001320-0x10001500, 0x10001500, 0x10001640, 0x10001720.
#include "MfPcAddExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"
#include "MfPcCompiler.h"
#include "MfPcStringErrorCB.h"
#include "MfPcStringListErrorCB.h"

// 0x10001000 ??0MfPcAddExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcAddExpression::MfPcAddExpression(MfPcExpression *l, MfPcExpression *r)
    : MfPcBinaryExpression(l, r)
{
    mKind = PC_EXPR_ADD;
}

// 0x10001040 ?Operator@MfPcAddExpression@@UAEEXZ (inline)
// 0x10001050 ??_GMfPcAddExpression@@UAEPAXI@Z (compiler-generated)

// 0x10001070 ??1MfPcAddExpression@@UAE@XZ
MfPcAddExpression::~MfPcAddExpression()
{
}

// 0x10001080 ?Optimize@MfPcAddExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
MfPcExpression *MfPcAddExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mRight = mRight->Optimize(compiler, chunks);
    MfVariant *lv = mLeft->LiteralValue();
    MfVariant *rv = mRight->LiteralValue();
    if (lv && rv) {
        if (lv->IsNumber() && rv->IsNumber()) {
            if (lv->IsInt() && rv->IsInt()) {
                int v = (int)*rv;
                v += (int)*lv;
                return new MfPcLiteralExpression(v);
            } else {
                float f = (float)*rv;
                f += (float)*lv;
                return new MfPcLiteralExpression(f);
            }
        } else if (lv->IsString() || rv->IsString()) {
            MfString s = MfString(*lv) + MfString(*rv);
            return new MfPcLiteralExpression(s);
        }
    }
    return this;
}


// Matched by this code:
// 0x10001030 ?Release@MfPcCompiler@@UAEHXZ
// 0x10001320 ??0MfPcStringErrorCB@@QAE@AAVMfString@@H@Z
// 0x10001340 ?Errors@MfPcStringErrorCB@@QBEABVMfString@@XZ
// 0x10001350 ??0MfPcStringErrorCB@@QAE@ABV0@@Z
// 0x10001380 ??0MfPcStringListErrorCB@@QAE@AAVMfStringList@@H@Z
// 0x100013a0 ??0MfPcStringListErrorCB@@QAE@ABV0@@Z
// 0x100013d0 ?Flags@MfPcCompiler@@UAEHXZ
// 0x100013e0 ?GetCurrentLineNumber@MfPcCompiler@@QBEHXZ
// 0x100013f0 ?SymbolTable@MfPcCompiler@@QBEAAVMfPcSymbolTable@@XZ
// 0x10001400 ?ConstantTable@MfPcCompiler@@QBEAAVMfPcConstantTable@@XZ
// 0x10001410 ?CodeType@MfPcCompiler@@UBEHXZ
// 0x10001420 ?ClearCodeType@MfPcCompiler@@UAEXXZ
// 0x10001430 ?Functions@MfPcCompiler@@UAEAAVMfScHandleList@@XZ
// 0x10001440 ?SetBufferManager@MfPcCompiler@@SAXPAVMfPcBufferManager@@@Z
// 0x10001450 ?NumberOfErrors@MfPcCompiler@@UBEHXZ
// 0x10001460 ?NumberOfWarnings@MfPcCompiler@@UBEHXZ
// 0x10001470 ?ModuleName@MfPcCompiler@@UBEABVMfString@@XZ
// 0x10001480 ?ModuleType@MfPcCompiler@@UBEABVMfString@@XZ
// 0x10001490 ?CurrentContinueLabel@MfPcCompiler@@QAEAAVMfString@@XZ
// 0x100014a0 ?CurrentBreakLabel@MfPcCompiler@@QAEAAVMfString@@XZ
// 0x100014b0 ?CurrentTestLabel@MfPcCompiler@@QAEAAVMfString@@XZ
// 0x100014c0 ?CurrentReturnLabel@MfPcCompiler@@QAEAAVMfString@@XZ
// 0x100014d0 ?CurrentReturn0Label@MfPcCompiler@@QAEAAVMfString@@XZ
// 0x100014e0 ?Labeler@MfPcCompiler@@QAEAAVMfPcLabelGen@@XZ
// 0x100014f0 ?SwitchLevel@MfPcCompiler@@QBEHXZ
// 0x10001500 ??0MfPcCompiler@@QAE@ABV0@@Z
// 0x10001640 ??4MfPcCompiler@@QAEAAV0@ABV0@@Z
// 0x10001720 ??_EMfPcCompiler@@UAEPAXI@Z