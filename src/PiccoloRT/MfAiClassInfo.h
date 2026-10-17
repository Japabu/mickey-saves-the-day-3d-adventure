// MfAiClassInfo: the interface of the class-info plugin (PscClassInfo.dll),
// header-defined, all pure.
#pragma once
#include "MfAiClassTree.h"
class CWnd;
class MfScClassInfo;

class MfAiClassInfo : public MfAi {
public:
    virtual void ShowClassInfo(CWnd *pParent) = 0;
    virtual int CreateClassTree(CWnd *pParent, unsigned long dwStyle, MfAiClassTree **ppTree) = 0;
};
