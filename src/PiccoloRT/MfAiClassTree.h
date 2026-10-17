// MfAiClassTree: the class tree window of the class-info plugin (PscClassInfo.dll).
#pragma once
class CWnd;
class MfScClassInfo;

// own copy: the class-info plugin's MfAi takes the version numbers by pointer, the
// runtime's (MfAiFunctionRegistry.h, mfutil's) by reference; the two cannot be
// included together. Header-defined, all pure.
class MfAi {
public:
    virtual void GetInterfaceName(char *name, int size, int *major, int *minor) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

class MfAiClassTree : public MfAi {
public:
    virtual void InsertClass(MfScClassInfo *info) = 0;
    virtual CWnd *GetWindow() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
};
