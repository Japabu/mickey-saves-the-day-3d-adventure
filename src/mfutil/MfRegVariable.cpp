// MfRegVariable (mfutil.dll 0x1000b6b0-0x1000bba0)
#include <windows.h>
#include "MfRegVariable.h"

// 0x1000b6b0 ??0MfRegVariable@@QAE@ABV0@@Z (implicit copy constructor)
// 0x1000b730 ??4MfRegVariable@@QAEAAV0@ABV0@@Z (implicit assignment)
// 0x1000b770 ??_EMfRegVariable@@UAEPAXI@Z (vector deleting destructor)

// 0x1000b7c0 ??0MfRegVariable@@QAE@PAXABVMfString@@1@Z
MfRegVariable::MfRegVariable(void *hkey, const MfString &subkey, const MfString &name)
    : MfEnvVariable(name), m_hkey(hkey), m_subkey(subkey)
{
    ReadVariable();
}

// 0x1000b850 ??1MfRegVariable@@UAE@XZ
MfRegVariable::~MfRegVariable()
{
}

// EQUIV 0x1000b8c0 ?ReadVariable@MfRegVariable@@MAEXXZ: reviewed: register allocation and order of independent lea/push only
void MfRegVariable::ReadVariable()
{
    MfString value;
    HKEY key;
    DWORD size;
    DWORD type;

    if (RegOpenKeyExA((HKEY)m_hkey, m_subkey, 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
        SetValueDirect(value);
        return;
    }
    if (RegQueryValueExA(key, GetName(), 0, &type, 0, &size) != ERROR_SUCCESS) {
        SetValueDirect(value);
        RegCloseKey(key);
        return;
    }
    if (type == REG_SZ) {
        char *buf = new char[size + 1];
        if (RegQueryValueExA(key, GetName(), 0, &type, (LPBYTE)buf, &size) != ERROR_SUCCESS) {
            delete[] buf;
            SetValueDirect(value);
            RegCloseKey(key);
            return;
        }
        value = buf;
        delete[] buf;
    }
    SetValueDirect(value);
    RegCloseKey(key);
}

// 0x1000ba60 ?WriteVariable@MfRegVariable@@MAEXXZ
void MfRegVariable::WriteVariable()
{
    MfString unused;
    HKEY key;
    DWORD disposition;

    if (RegCreateKeyExA((HKEY)m_hkey, m_subkey, 0, "", 0, KEY_ALL_ACCESS, 0, &key,
                        &disposition) != ERROR_SUCCESS)
        return;
    RegSetValueExA(key, GetName(), 0, REG_SZ, (const BYTE *)(char *)operator MfString(),
                   operator MfString().Length() + 1);
    RegCloseKey(key);
}
