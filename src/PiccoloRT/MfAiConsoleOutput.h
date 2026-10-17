// MfAiConsoleOutput: a console output (RuntimeLib's) that MfScConsoleManager prints to.
#pragma once
class MfString;

// Console output interface (RuntimeLib); only called through the vtable.
class MfAiConsoleOutput {
public:
    virtual ~MfAiConsoleOutput() {}
    virtual void AddRef() = 0;
    virtual void Release() = 0;
    virtual void Print(const MfString &s) = 0;
    virtual void PrintNl(const MfString &s) = 0;
    virtual void Clear() = 0;
};
