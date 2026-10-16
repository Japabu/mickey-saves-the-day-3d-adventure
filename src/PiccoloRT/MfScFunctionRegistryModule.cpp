#include "MfScFunctionRegistryModule.h"
#include "MfString.h"

// 0x10008b40 ?ModuleType@MfScFunctionRegistryModule@@QBEABVMfString@@XZ
MfString const & MfScFunctionRegistryModule::ModuleType() const
{
    return mModuleType;
}
