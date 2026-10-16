// MfInterfaceRegistry (mfutil.dll): the registry of the application
// interfaces, looked up by name.
#pragma once
class MfAi;
class MfString;

class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int __cdecl GetInterface(const MfString &, MfAi **);
};
