#pragma once
class MfString;
class MfStringList;

// What an error callback gets for each error: the module, the message and
// the offending source line formatted with a marker.
class MfIPcErrorInfo {
public:
    virtual MfString const &GetModule() const = 0;
    virtual MfString const &GetMessageA() const = 0;
    virtual void FormatLine(MfStringList &lines) const = 0;
};
