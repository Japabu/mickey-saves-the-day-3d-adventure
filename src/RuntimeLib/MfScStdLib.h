// MfScStdLib (PiccoloRT.dll): the standard library; registers how script
// lists are made and read. As in src/MfPoLibrary.
#ifndef RUNTIMELIB_MFSCSTDLIB_H
#define RUNTIMELIB_MFSCSTDLIB_H
class MfDblList;
class MfScDataHandle;

class __declspec(dllimport) MfScStdLibListInterface {
public:
    virtual int CreateList(MfDblList **list, MfScDataHandle **handle);
    virtual int GetList(MfDblList **list, MfScDataHandle *handle);
};

class __declspec(dllimport) MfScStdLib {
private:
    friend class MfPoSpace;
    static MfScStdLibListInterface *pslliListInterface;
};

#endif
