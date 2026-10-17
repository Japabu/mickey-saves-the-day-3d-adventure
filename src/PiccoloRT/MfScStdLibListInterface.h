// MfScStdLibListInterface: how the standard library plugin makes script lists.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
class MfDblList;
class MfScDataHandle;

// How the standard library plugin (PscStdlib) makes script lists; the default
// one, used until the plugin registers its own, makes none.
class PICCOLORT_API MfScStdLibListInterface {
public:
    virtual int CreateList(MfDblList **list, MfScDataHandle **handle);
    virtual int GetList(MfDblList **list, MfScDataHandle *handle);
};
