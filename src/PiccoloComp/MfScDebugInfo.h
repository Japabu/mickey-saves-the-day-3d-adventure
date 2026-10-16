#pragma once
class MfScCodeLevelInfo;
class MfScConstUsageInfo;
class MfScLineInfo;
class MfScVariableDebugInfo;

// PiccoloRT.dll: debug information of a bytecode routine (0x28 bytes).
class __declspec(dllimport) MfScDebugInfo {
public:
    MfScDebugInfo();
    virtual ~MfScDebugInfo();
    int InitializeLineInfo(int);
    void SetLineInfo(MfScLineInfo const &, int);
    int InitializeVarDebugInfo(int);
    void SetVarDebugInfo(MfScVariableDebugInfo const &, int);
    int InitializeCodeLevelInfo(int);
    void SetCodeLevelInfo(MfScCodeLevelInfo const &, int);
    int InitializeConstUsageInfo(int);
    void SetConstUsageInfo(MfScConstUsageInfo const &, int);
private:
    int mCodeSize;                          // +0x04
    MfScLineInfo *mLineInfo;                // +0x08
    int mNumLineInfo;                       // +0x0c
    MfScVariableDebugInfo *mVarDebugInfo;   // +0x10
    int mNumVarDebugInfo;                   // +0x14
    MfScCodeLevelInfo *mCodeLevelInfo;      // +0x18
    int mNumCodeLevelInfo;                  // +0x1c
    MfScConstUsageInfo *mConstUsageInfo;    // +0x20
    int mNumConstUsageInfo;                 // +0x24
};
