#pragma once
class MfAi;
class MfString;

// mfutil.dll: the published interfaces, by name.
class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int AddInterface(MfAi *ai);
    static int RemoveInterface(MfString const &name);
};
