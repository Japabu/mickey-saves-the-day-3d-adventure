// MfRegVariable (mfutil.dll 0x1000b6b0-0x1000bba0): an MfEnvVariable whose value
// lives in the registry (string value <name> under key <hkey>\<subkey>).
// Reconstructed declarations (no PDB): layouts from the code, signatures from the
// export names.
#ifndef MFUTIL_MFREGVARIABLE_H
#define MFUTIL_MFREGVARIABLE_H

#include "MfString.h"
#include "MfEnvVariable.h"

// size 0x14, vtable 0x10017694
class MFUTIL_API MfRegVariable : public MfEnvVariable {
public:
    MfRegVariable(void *hkey, const MfString &subkey, const MfString &name);
    virtual ~MfRegVariable();
protected:
    virtual void ReadVariable();
    virtual void WriteVariable();

    void *m_hkey;       // +0xc (HKEY)
    MfString m_subkey;  // +0x10
};

#endif
