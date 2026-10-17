#pragma once
class MfAiFunctionRegistry;
class MfPcCodeLoader;
class MfPcCompiler;
class MfPcConstSet;
class MfString;

// PiccoloRT.dll: the script runtime's globals.
class __declspec(dllimport) MfScPiccolo {   // own copy: src/PiccoloRT/MfScPiccolo.h has no friends for the compiler (fsDataPath, bIsRunning) and opens MfScDataObject.h/PiccoloRT's MfString
    friend class MfPcCodeLoader;
    friend class MfPcCompiler;
    friend class MfPcConstSet;
public:
    static MfAiFunctionRegistry *GetFunctionRegistry();
private:
    static int bIsRunning;
    static MfString fsDataPath;
};
