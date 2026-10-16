#pragma once

// PiccoloRT.dll: bytecode offset -> block nesting level (8 bytes).
class MfScCodeLevelInfo {
public:
    MfScCodeLevelInfo(int offset, int level) : mOffset(offset), mLevel(level) {}
    ~MfScCodeLevelInfo() {}
    int mOffset;    // +0
    int mLevel;     // +4
};
