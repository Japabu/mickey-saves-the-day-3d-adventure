// MfScCodeLevelInfo: bytecode offset -> block nesting level (debug information).
#pragma once

// Bytecode offset -> block nesting level (8 bytes).
class MfScCodeLevelInfo {
public:
    MfScCodeLevelInfo() : mOffset(-1), mLevel(-1) {}
    ~MfScCodeLevelInfo() {}
    MfScCodeLevelInfo operator=(MfScCodeLevelInfo const &other)
    {
        mOffset = other.mOffset;
        mLevel = other.mLevel;
        return *this;
    }
    int mOffset;    // +0
    int mLevel;     // +4
};
