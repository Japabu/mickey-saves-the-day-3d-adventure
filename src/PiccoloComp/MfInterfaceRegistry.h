#pragma once
class MfAi;
class MfString;

// mfutil.dll: the published interfaces, by name.
class __declspec(dllimport) MfInterfaceRegistry {   // own copy: src/mfutil/MfInterfaceRegistry.h includes src/mfutil/MfString.h, which conflicts with the older MfString kept here
public:
    static int AddInterface(MfAi *ai);
    static int RemoveInterface(MfString const &name);
};
