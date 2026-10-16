// MfScStdLib (PiccoloRT.dll): where the standard library plugin (PscStdlib)
// registers how script lists are made. The script classes that build lists
// read the private interface pointer itself (the imported data symbol).
#pragma once
class MfDblList;
class MfScDataHandle;

class __declspec(dllimport) MfScStdLibListInterface {
public:
    virtual int CreateList(MfDblList **list, MfScDataHandle **handle);
    virtual int GetList(MfDblList **list, MfScDataHandle *handle);
};

class __declspec(dllimport) MfScStdLib {
public:
    static void RegisterListInterface(MfScStdLibListInterface *);
    static MfScStdLibListInterface *GetListInterface();
private:
    friend class MfPoActivity;
    friend class MfPoActor;
    friend class MfPoBase;
    friend class MfPoSkill;
    static MfScStdLibListInterface *pslliListInterface;
    static MfScStdLibListInterface slliDefault;
};
