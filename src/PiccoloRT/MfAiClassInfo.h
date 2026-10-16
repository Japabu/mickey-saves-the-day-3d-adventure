#pragma once
class CWnd;
class MfScClassInfo;

// Interfaces of the class-info plugin (PscClassInfo.dll), header-defined, all pure.
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

class MfAiClassInfo : public MfAi {
public:
    virtual void ShowClassInfo(CWnd *pParent) = 0;
    virtual int CreateClassTree(CWnd *pParent, unsigned long dwStyle, MfAiClassTree **ppTree) = 0;
};
