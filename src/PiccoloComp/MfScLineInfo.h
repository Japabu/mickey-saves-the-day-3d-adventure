#pragma once

// PiccoloRT.dll: bytecode offset -> source line (8 bytes).
class __declspec(dllimport) MfScLineInfo {
public:
    MfScLineInfo();
    MfScLineInfo(int offset, int line) : mOffset(offset), mLine(line) {}
    ~MfScLineInfo() {}
    int Line() const;
    int Offset() const;
    int mOffset;    // +0
    int mLine;      // +4
};
