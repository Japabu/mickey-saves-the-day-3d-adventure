// MfInterfaceRegistry (mfutil.dll): the published interfaces, by name.
#ifndef RUNTIMELIB_MFINTERFACEREGISTRY_H
#define RUNTIMELIB_MFINTERFACEREGISTRY_H
class MfAi;
class MfString;

class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int AddInterface(MfAi *ai);
    static int RemoveInterface(MfString const &name);
    static int GetInterface(const MfString &name, MfAi **ai);
};

#endif
