#pragma once
// MfListValueSearchItr: what MfListValue::Search matches the list items with.
#include "MfActorLib_api.h"
#include "MfListItemInterface.h"

// Matches the list values that compare equal to the key.
class MFACTOR_API MfListValueSearchItr : public MfListItemInterface {
public:
    MfListValueSearchItr() {}
    virtual int Matches(void *pKey, void *pData);
};
