// MfScVmEnv (PiccoloRT.dll): the environment of a running script (global
// variables, exceptions).
#pragma once
class MfScDataHandle;
class MfString;

class __declspec(dllimport) MfScVariableStack {
public:
    enum MfScVariableErrorCode { VEC_NO_ERROR };
};

class __declspec(dllimport) MfScVmEnv {
public:
    static MfScVariableStack::MfScVariableErrorCode __cdecl SetGlobalVariable(const MfString &, MfScDataHandle *, int, int);
    static MfScVariableStack::MfScVariableErrorCode __cdecl GetGlobalVariable(const MfString &, MfScDataHandle **);
    static MfScVariableStack::MfScVariableErrorCode __cdecl ClearGlobalVariable(const MfString &, int);
    MfScVariableStack::MfScVariableErrorCode GetVariable(const MfString &, MfScDataHandle **) const;
    void ThrowException(int, const MfString &);
};
