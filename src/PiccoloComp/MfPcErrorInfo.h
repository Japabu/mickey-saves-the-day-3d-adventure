#pragma once
#include "MfIPcErrorInfo.h"
#include "MfString.h"
class MfPcTokenInfo;

// One compile error (0x14 bytes).
class __declspec(dllexport) MfPcErrorInfo : public MfIPcErrorInfo {
public:
    MfPcErrorInfo(MfString const &line, MfString const &module, MfString const &message,
                  MfPcTokenInfo *token)
        : mLine(line), mModule(module), mMessage(message), mToken(token) {}
    virtual MfString const &GetModule() const { return mModule; }
    virtual MfString const &GetMessageA() const { return mMessage; }
    virtual void FormatLine(MfStringList &lines) const;
    static MfString ParseModuleName(MfString const &info);
    static int ParseLineNumber(MfString const &info);

    MfString mLine;             // +0x04 the source line
    MfString mModule;           // +0x08
    MfString mMessage;          // +0x0c
    MfPcTokenInfo *mToken;      // +0x10
};
