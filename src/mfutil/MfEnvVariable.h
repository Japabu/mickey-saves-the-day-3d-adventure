// MfEnvVariable (mfutil.dll 0x10003740-0x10003a00): a named process environment
// variable (getenv/_putenv); MfRegVariable keeps it in the registry instead.
// Reconstructed declarations (no PDB): layouts from the code, signatures from the
// export names.
#ifndef MFUTIL_MFENVVARIABLE_H
#define MFUTIL_MFENVVARIABLE_H

#include "MfString.h"

// size 0xc, vtable 0x1001742c: ??_E, ReadVariable, WriteVariable.
// The constructor/destructor are inline (MfEnvVariable's unit keeps the exported copies).
class MFUTIL_API MfEnvVariable {
public:
    MfEnvVariable(const MfString &name) : m_name(name) { ReadVariable(); }
    virtual ~MfEnvVariable() {}
    void Set(const MfString &value) { m_value = value; WriteVariable(); }
    operator MfString();
    MfString Get() const;
    void SetName(const MfString &name);
    MfString GetName() const;
protected:
    void SetValueDirect(const MfString &value) { m_value = value; }
    virtual void ReadVariable();
    virtual void WriteVariable();

    MfString m_name;    // +4
    MfString m_value;   // +8
};

#endif
