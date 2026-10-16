// PiccoloRT.dll 0x10002060-0x100028b5 (vc5 /O2 /GX). No PDB; names from exports.
// The object first emits the compiler-generated and inline members of the
// exported classes declared in MfScBytecodeRoutine.h:
// 0x10002060 ??_EMfScBytecodeRoutine@@UAEPAXI@Z (compiler-generated)
// 0x100020b0 ?SetCodeSize@MfScDebugInfo@@QAEXH@Z (inline, header; folded)
// 0x100020c0 ?GetBytecodes@MfScBytecodeRoutine@@QBEPAEXZ = ?NumCodeLevelInfo@MfScDebugInfo@@QBEHXZ (inline, header)
// 0x100020d0 ?GetOrdinal@MfScExtRoutine@@QBEHXZ (inline, header)
// 0x100020e0 ??0MfScDebugInfo@@QAE@ABV0@@Z (compiler-generated)
// 0x10002130 ??_EMfScDebugInfo@@UAEPAXI@Z (compiler-generated)
// 0x10002180 ?GetClassNameA@MfScDataObject@@UAE?AVMfString@@XZ (inline, header)
// 0x100021b0 ?HasInstanceProperties@MfScDataObject@@QBEHXZ (inline, header)
// 0x100021c0 ?GetPropTokenTable@MfScDataObject@@UAEPAVMfScPropTokenTable@@XZ (original ?GetPropTokenTable@MfScDataObject@@...) = ?GetPropertyExtensions@MfScDataObject@@... (inline, header)
// 0x100021d0 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z (inline, header)
// 0x100021e0 ?Flags@MfScDataObject@@QBEIXZ = ?GetConstantPool@MfScDataMethod@@... (inline, header)
// 0x100021f0 ??0MfScDataObject@@QAE@ABV0@@Z (compiler-generated)
// 0x10002220 ??_FMfScDataObject@@QAEXXZ (compiler-generated)
// 0x10002230 ??_EMfScDataObject@@UAEPAXI@Z (compiler-generated)
// 0x10002280 ??0MfScObject@@QAE@I@Z (inline, header)
// 0x100022a0 ??0MfScObject@@QAE@ABV0@@Z (compiler-generated)
// 0x100022d0 ??1MfScObject@@UAE@XZ (implicit)
// 0x100022e0 ??_FMfScObject@@QAEXXZ (compiler-generated)
// 0x100022f0 ??_EMfScObject@@UAEPAXI@Z (compiler-generated)
// 0x10002340 ?GetPropTokenTable@MfScObjString@@UAEPAVMfScPropTokenTable@@XZ (original ?GetPropTokenTable@MfScObjString@@...) = ?PropertyTokenTable@MfScObjString@@... (inline, header)
// 0x10002350 ?IsReadOnly@MfScObjString@@QBEHXZ = ?Params@MfScDataMethod@@QBEHXZ (inline, header)
// 0x10002360 ?SetReadOnly@MfScObjString@@QAEXXZ (inline, header)
// 0x10002370 ?SetReadWrite@MfScObjString@@QAEXXZ (inline, header)
// 0x10002380 ?GetClassNameA@MfScObjString@@MAE?AVMfString@@XZ (inline, header)
// EQUIV 0x100023b0 ??0MfScObjString@@QAE@ABV0@@Z: proven (renaming-equivalent); bytes differ: (compiler-generated): (87% similar) ??0MfScObjString@@QAE@ABV0@@Z: 0 bytes differ (with --eh 1001cc20,1001cc28); same tool issue: the unwind funclet's call to ??1MfScObject (0x100022d0) is resolved as a thunk to ??1MfScDataObject
// 0x10002430 ??4MfScObjString@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10002480 ??_FMfScObjString@@QAEXXZ (compiler-generated)
// 0x10002490 ??_EMfScObjString@@UAEPAXI@Z (compiler-generated)
#include "MfScBytecodeRoutine_unit.h"

// 0x100024e0 ??0MfScBytecodeRoutine@@QAE@H@Z
MfScBytecodeRoutine::MfScBytecodeRoutine(int numArgs)
    : MfScDataMethod(kNone, numArgs), m_bytecodes(0), m_unk24(0), m_debugInfo(0)
{
    m_str28 = new MfString;
    m_str2c = new MfString;
    m_str30 = new MfString;
    m_str38 = new MfString;
}

// 0x100025d0 ??1MfScBytecodeRoutine@@UAE@XZ
MfScBytecodeRoutine::~MfScBytecodeRoutine()
{
    if (m_bytecodes)
        delete[] m_bytecodes;
    delete m_str28;
    delete m_str2c;
    delete m_str30;
    delete m_str38;
    delete m_debugInfo;
}

// 0x100026a0 ?Attach@MfScBytecodeRoutine@@QAEXPAVMfScDebugInfo@@@Z
void MfScBytecodeRoutine::Attach(MfScDebugInfo *info)
{
    delete m_debugInfo;
    m_debugInfo = info;
    if (info)
        info->SetCodeSize(m_codeSize);
}

// 0x100026d0 ?Detach@MfScBytecodeRoutine@@QAEPAVMfScDebugInfo@@XZ
MfScDebugInfo *MfScBytecodeRoutine::Detach()
{
    MfScDebugInfo *info = m_debugInfo;
    m_debugInfo = 0;
    return info;
}

// EQUIV 0x100026e0 ?MemoryUsage@MfScBytecodeRoutine@@QAEIH@Z: reviewed: the member loads of two summed lengths come in another order (a commutative sum); otherwise identical
unsigned int MfScBytecodeRoutine::MemoryUsage(int what)
{
    unsigned int size = 0;
    if (what & 1)
        size = m_name->Length() + m_str28->Length() + m_str2c->Length()
             + (m_str30->Length() + 4);
    if (what & 2)
        size += m_codeSize;
    if (what & 4) {
        for (int i = 0; i < m_numConstants; i++) {
            MfScDataHandle *h = m_constants[i];
            if (h) {
                if (h->IsType(0x10))
                    size += 4;
                else if (h->IsType(0x20))
                    size += 4;
                else if (h->IsType(0x100))
                    size += ((MfScObjString *)h->GetData())->Length() + 1;
            }
        }
    }
    if (m_debugInfo && (what & 8))
        size += m_debugInfo->MemoryUsage();
    return size;
}

// 0x10002790 ??0MfScBytecodes@@QAE@XZ (inline, header; folded)

// aiBytecodeSize (data at 0x10032200): the length in bytes of each instruction
// (opcode and operands), by opcode (MfScOpcodes.h)
int MfScBytecodes::aiBytecodeSize[256] = {
    1, 2, 3, 2, 2, 3, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x00
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,  // 0x10
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 3,  // 0x20
    1, 1, 1, 1, 3, 3, 3, 3, 1, 1, 3, 3, 1, 1, 1, 1,  // 0x30
    3, 1, 2, 3, 5, 2, 3, 5, 2, 3, 5, 1, 1, 3, 3, 1,  // 0x40
    3, 3, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1,  // 0x50
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x70
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x80
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x90
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xa0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xb0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xc0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xd0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xe0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xf0
};

// 0x100027a0 ?BytecodeSize@MfScBytecodes@@SAHE@Z
int MfScBytecodes::BytecodeSize(unsigned char op)
{
    return aiBytecodeSize[op];
}

// 0x100027c0 ?IsJump@MfScBytecodes@@SAHE@Z
int MfScBytecodes::IsJump(unsigned char op)
{
    return op >= 0x42 && op <= 0x4a;
}

// 0x100027e0 ?JumpOffset@MfScBytecodes@@SAHPAE@Z
int MfScBytecodes::JumpOffset(unsigned char *code)
{
    int offset = 0;
    switch (*code) {
    case 0x42:
    case 0x45:
    case 0x48:
        offset = (signed char)code[1];
        break;
    case 0x43:
    case 0x46:
    case 0x49: {
        unsigned char lo = *++code;
        offset = ((signed char)*++code << 8) | lo;
        break;
    }
    case 0x44:
    case 0x47:
    case 0x4a: {
        unsigned char b0 = *++code;
        char b1 = *++code;
        char b2 = *++code;
        char b3 = *++code;
        offset = ((((b3 << 16) | b2) << 8 | b1) << 8) | b0;
        break;
    }
    }
    return offset;
}

// 0x10002880 ?IsUnconditionalJump@MfScBytecodes@@SAHE@Z
int MfScBytecodes::IsUnconditionalJump(unsigned char op)
{
    return op >= 0x42 && op <= 0x44;
}

// 0x100028a0 ?IsJumpZero@MfScBytecodes@@SAHE@Z
int MfScBytecodes::IsJumpZero(unsigned char op)
{
    return op >= 0x45 && op <= 0x47;
}


// Also matched by this code:
// piccolocomp.dll 0x10011990 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z
// piccolocomp.dll 0x10011a30 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z
// piccolocomp.dll 0x10016b60 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z
// piccolocomp.dll 0x10016b80 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z
// piccoloconsole.dll 0x1001cba0 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z
// runtimelib.dll 0x10030100 ?SetCodeSize@MfScDebugInfo@@QAEXH@Z
// runtimelib.dll 0x1009cc60 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z
// runtimelib.dll 0x1009cc80 ?SetClassInfoInterface@MfScDataObject@@SAXPAVMfAiClassInfo@@@Z
// 0x10002ad0 ?FilloutPropertyNameList@MfScDataObject@@UAEXPAVMfStringList@@@Z (empty; folded with MfScDiagnosticCallBack::CallBack)
void MfScDataObject::FilloutPropertyNameList(MfStringList *) {}
