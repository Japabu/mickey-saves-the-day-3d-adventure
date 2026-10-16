#pragma once
#include "MfScDataObject.h"
class MfAiFunctionRegistry;
class MfAiSourceResolver;
class MfDblList;
class MfDblListItem;
class MfScPiccoloDll;
class MfScDataHandle;
class MfScFunctionRegistry;
class MfScSourceResolver;
class MfScThread;
class MfString;
class MfStringList;

class __declspec(dllexport) MfScPiccolo : public MfScDataObject {
private:
    virtual ~MfScPiccolo();
public:
    static MfAiFunctionRegistry * __cdecl GetFunctionRegistry();
    static MfAiSourceResolver * __cdecl GetSourceResolver();
    static MfString const & __cdecl GetDataPath();
    static int __cdecl InitializeExtensions(MfString *);
    static int __cdecl InitializePlugins(MfString *);
    static int __cdecl IsRunning();
    static void __cdecl FlushPiccoloDataPathFunctions();
    static void __cdecl Initialize();
    static void __cdecl LoadPiccoloDataPathFunctions(int);
    static void __cdecl LoadPiccoloLibFunctions(int);
    static void __cdecl LoadPiccoloModulesFromDirectory(MfString const &, int, int);
    static void __cdecl ReadSinglePiccoloModule(MfString const &, int, int);
    static void __cdecl SetAsMessageThread();
    static void __cdecl SetDataPath(MfString const &);
    static void __cdecl Shutdown(MfString *);
    static void __cdecl ShutdownExtensions();
    static void __cdecl StartRunning();
    static void __cdecl StopRunning();
private:
    MfScDataHandle * PMaddVariable(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMcallStack(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMdoesVariableExist(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMfirstExtension(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMfirstPlugin(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMflushFunctions(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMgetClassName(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMgetDataPath(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMgetFunctionsInDataPath(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMgetInternalClassName(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMloadFunctions(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMnextExtension(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMnextPlugin(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMqueryPropertyNames(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMremoveVariable(MfScDataHandle *, int, MfScDataHandle * *);
    MfScDataHandle * PMsetDataPath(MfScDataHandle *, int, MfScDataHandle * *);
    MfScPiccolo() : MfScDataObject(kObject), mPluginPos(0), mExtensionPos(0) {}
    static int __cdecl LoadPiccoloDll(MfString const &, int (__cdecl*)(MfString const &,MfString const &,MfString *), MfString *, MfDblList * *, int);
    static int __cdecl RetrieveDllNames(MfString *, MfString const &, MfStringList *);
    static void __cdecl InitPluginList(MfDblList * *);
    static void __cdecl InitializePiccoloObject();
    static void __cdecl ShutdownPiccoloObject();
    friend class MfScFileProvider;
    friend class MfScPiccoloObjectFile;
    static int bInitialized;
    static int bIsRunning;
    static MfString fsDataPath;
    static MfScPiccolo * pdoPiccolo;
    static MfScFunctionRegistry * pfrRegistry;
    static MfDblList * plExtensions;
    static MfDblList * plPlugins;
    static MfScSourceResolver * psrResolver;
    static MfScThread * pthrdMainThread;
public:
    MfDblListItem *mPluginPos;          // +0x14 the position of PMfirstPlugin/PMnextPlugin
    MfDblListItem *mExtensionPos;       // +0x18 of PMfirstExtension/PMnextExtension
};
