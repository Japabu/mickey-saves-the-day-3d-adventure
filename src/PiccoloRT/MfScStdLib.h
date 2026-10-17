// MfScStdLib: the list interface the standard library plugin registers.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfScStdLibListInterface.h"
class MfDblList;
class MfScDataHandle;

class PICCOLORT_API MfScStdLib {
public:
    static void RegisterListInterface(MfScStdLibListInterface *);
    static MfScStdLibListInterface *GetListInterface();
private:
    friend class MfScPiccolo;
    static MfScStdLibListInterface *pslliListInterface;
    static MfScStdLibListInterface slliDefault;
};
