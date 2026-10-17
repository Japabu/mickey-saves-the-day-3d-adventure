// LiiExemplar: the tag type of the constructors that build a list lookup key.
#pragma once

// Tag type for the constructor that builds a lookup key (MfScClassInfo,
// MfScFunctionRegistryEntry, MfScFunctionRegistryModule). Passed by value: its
// destructor gives the callers their EH states.
class LiiExemplar {
public:
    ~LiiExemplar() {}
};
