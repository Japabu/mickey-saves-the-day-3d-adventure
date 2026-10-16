// MfScPiccoloObjectFile (PiccoloRT.dll): a .pobj file of compiled script
// functions. Only what RuntimeLib calls.
#ifndef RUNTIMELIB_MFSCPICCOLOOBJECTFILE_H
#define RUNTIMELIB_MFSCPICCOLOOBJECTFILE_H
class MfScDataHandle;
class MfScHandleList;
class MfString;

class __declspec(dllimport) MfScPiccoloObjectFile {
public:
    enum MfScPOFErrorCode { kPOFOk = 0 };

    // opens file; writing if write is set
    MfScPiccoloObjectFile(const MfString &file, int write, int, unsigned int);
    virtual ~MfScPiccoloObjectFile();
    MfScPOFErrorCode ReadClose();
    MfScPOFErrorCode WriteClose();
    // the text of the last error code
    MfString ECToString();
    MfScPOFErrorCode AddFunction(MfScDataHandle *function);
    // the functions of a module
    MfScPOFErrorCode LookupFunctions(const MfString &module, MfScHandleList *functions);

    char m04[0x18];                                         // 0x04
    int mStatus;                                            // 0x1c RuntimeLib reads only when this is 0, writes only when not
    char m20[0x18];                                         // 0x20
};

#endif
