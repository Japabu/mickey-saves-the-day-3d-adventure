// MfCbRegistryModuleEnumerator: the callback of MfAiFunctionRegistry::EnumModules.
#pragma once
class MfString;

// Called once per module; returning 0 stops the enumeration.
class MfCbRegistryModuleEnumerator {
public:
    virtual int Enumerate(const MfString &module, const MfString &moduleType, void *data) = 0;
};
