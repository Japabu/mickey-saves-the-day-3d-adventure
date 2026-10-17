// MfInterfaceRegistry (mfutil.dll 0x10004760-0x100049a0): static registry of MfAi
// interfaces in an MfDictionary keyed by interface name. Layouts from the code.
#ifndef MFUTIL_MFINTERFACEREGISTRY_H
#define MFUTIL_MFINTERFACEREGISTRY_H

#include "mfutil_api.h"
#include "MfString.h"
#include "MfDictionary.h"
#include "MfAi.h"

class MFUTIL_API MfInterfaceRegistry {
public:
    static int AddInterface(MfAi *pai);
    static int GetInterface(const MfString &name, MfAi **ppai);
    static int RemoveInterface(const MfString &name);
    static void Shutdown();
private:
    static void Initialize();
    static MfDictionary *pdictRegistry;
};

#endif
