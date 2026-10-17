// MfCbRegistryFunctionEnumerator: the callback of MfAiFunctionRegistry::EnumFunctions.
#pragma once
class MfScDataHandle;

// Called once per function; returning 0 stops the enumeration.
class MfCbRegistryFunctionEnumerator {
public:
    virtual int Enumerate(MfScDataHandle *function, void *data) = 0;
};
