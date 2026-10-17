// MfScLineInfo: bytecode offset -> source line (debug information).
#pragma once
#include "PiccoloRT_api.h"

// Bytecode offset -> source line (8 bytes).
class PICCOLORT_API MfScLineInfo {
public:
    MfScLineInfo();
    MfScLineInfo(int offset, int line) : mOffset(offset), mLine(line) {}
    ~MfScLineInfo() {}
    MfScLineInfo &operator=(MfScLineInfo const &other)
    {
        mOffset = other.mOffset;
        mLine = other.mLine;
        return *this;
    }
    int Line() const;
    int Offset() const;
    int mOffset;    // +0
    int mLine;      // +4
};
