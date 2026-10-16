// PiccoloRT.dll 0x10007340-0x100074bf. The object starts with the inline
// members of MfScLineInfo (exported class):
// 0x10007340 ??0MfScLineInfo@@QAE@HH@Z (inline, header)
// 0x10007360 ??0MfScLineInfo@@QAE@ABV0@@Z = ??4MfScLineInfo@@QAEAAV0@ABV0@@Z (compiler-generated)
#include "MfScDataMethod.h"
#include "MfScLineInfo.h"
#include <string.h>

// 0x10007380 ??0MfScDataMethod@@QAE@W4MfScMethodType@0@H@Z
MfScDataMethod::MfScDataMethod(MfScMethodType type, int numArgs)
    : m_type(type), m_constants(0), m_numConstants(0), m_numArgs(numArgs)
{
}

// 0x100073b0 ??1MfScDataMethod@@UAE@XZ
MfScDataMethod::~MfScDataMethod()
{
    DeleteConstantPool();
}

// 0x10007400 ?AllocConstantPool@MfScDataMethod@@QAEPAPAVMfScDataHandle@@H@Z
MfScDataHandle **MfScDataMethod::AllocConstantPool(int size)
{
    m_numConstants = size;
    m_constants = new MfScDataHandle *[size];
    memset(m_constants, 0, m_numConstants * sizeof(MfScDataHandle *));
    return m_constants;
}

// 0x10007440 ?DeleteConstantPool@MfScDataMethod@@QAEXXZ
void MfScDataMethod::DeleteConstantPool()
{
    if (m_constants) {
        for (int i = 0; i < m_numConstants; i++) {
            if (m_constants[i])
                m_constants[i]->Release();
        }
        delete[] m_constants;
        m_constants = 0;
    }
}

// 0x10007490 ?SetConstant@MfScDataMethod@@QAEXHPAVMfScDataHandle@@@Z
void MfScDataMethod::SetConstant(int i, MfScDataHandle *h)
{
    if (m_constants[i])
        m_constants[i]->Release();
    m_constants[i] = h;
}
