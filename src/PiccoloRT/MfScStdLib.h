#pragma once
class MfDblList;
class MfScDataHandle;

// How the standard library plugin (PscStdlib) makes script lists; the default
// one, used until the plugin registers its own, makes none.
class __declspec(dllexport) MfScStdLibListInterface {
public:
    virtual int CreateList(MfDblList **list, MfScDataHandle **handle);
    virtual int GetList(MfDblList **list, MfScDataHandle *handle);
};

class __declspec(dllexport) MfScStdLib {
public:
    static void RegisterListInterface(MfScStdLibListInterface *);
    static MfScStdLibListInterface *GetListInterface();
private:
    friend class MfScPiccolo;
    static MfScStdLibListInterface *pslliListInterface;
    static MfScStdLibListInterface slliDefault;
};
