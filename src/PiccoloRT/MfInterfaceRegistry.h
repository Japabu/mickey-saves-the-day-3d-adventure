#pragma once
// mfutil.dll: the published interfaces, by name.
class MfAi;
class MfString;

class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int AddInterface(MfAi *ai);
    static int RemoveInterface(const MfString &name);
    static int GetInterface(const MfString &name, MfAi **ai);
};
