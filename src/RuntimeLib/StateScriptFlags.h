#pragma once

class StateScriptFlags {
public:
    StateScriptFlags & operator=(StateScriptFlags const &);
    static int __cdecl GetFlags();
    static void __cdecl ClearFlag(int);
    static void __cdecl SetFlag(int);
private:
    friend class MfFSMManager;
    friend class MfFiniteStateMachine;
    static int iFlags;
public:
    char field_0;
};
