// PiccoloRT.dll 0x1000f230-0x100132b0: the runtime's startup and shutdown,
// its plugins and extensions, and the script object "Piccolo".
#include <windows.h>
#include "MfScPiccolo.h"
#include "MfScPiccoloDll.h"
#include "MfScStdLib.h"
#include "MfLogger.h"
#include "MfString.h"
#include "MfDblList.h"
#include "MfScDataHandle.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"
#include "MfScDebugger.h"
#include "MfScFunctionRegistry.h"
#include "MfScHandleDictIterator.h"
#include "MfStringList.h"
#include "MfScBytecodeRoutine.h"
#include "MfScObject.h"
#include "MfScRoutine.h"
#include "MfScVmCallStackList.h"
#include "MfScSourceResolver.h"
#include "MfScBreakpointManager.h"
#include "MfScConsoleManager.h"
#include "MfScThreadManager.h"
#include "MfScVmManager.h"
#include "MfScDiagnostics.h"
#include "MfAiCompiler.h"
#include "MfInterfaceRegistry.h"
#include "MfScFileProvider.h"
#include "MfDirectory.h"
#include "MfScPiccoloObjectFile.h"
#include "MfScHandleList.h"
#include "MfScVirtualMachine.h"
#include "MfRegularExpression.h"
#include "../mfutil/UtilityApp.h"

extern __declspec(dllimport) MfNullString NULL_STRING;

// separates the directories of PICCOLOPATH and PICCOLOLIB (";")
extern const char *const gPathSeparator;

// collects the names of the global functions that came from the data path
class InternalGlobFuncIterator : public MfScHandleDictIterator {
public:
    virtual void EnumHandle(const MfString &name, MfScDataHandle *handle);

    MfStringList mNames;            // +0x04
};

// 0x1000f230 ?RegisterListInterface@MfScStdLib@@SAXPAVMfScStdLibListInterface@@@Z
void MfScStdLib::RegisterListInterface(MfScStdLibListInterface *listInterface)
{
    pslliListInterface = listInterface;
    if (!listInterface)
        pslliListInterface = &slliDefault;
}

// 0x1000f250 ?GetListInterface@MfScStdLib@@SAPAVMfScStdLibListInterface@@XZ
MfScStdLibListInterface *MfScStdLib::GetListInterface()
{
    return pslliListInterface;
}

MfScStdLibListInterface *MfScStdLib::pslliListInterface = &MfScStdLib::slliDefault;

// 0x1000f260 _$E48 (static initializer of fsDataPath, with the three below)
// 0x1000f270 _$E44
// 0x1000f290 _$E47
// 0x1000f2a0 _$E45
MfString MfScPiccolo::fsDataPath((MfStringSize(256)));
int MfScPiccolo::bInitialized;                     // 0x10035238
MfScThread *MfScPiccolo::pthrdMainThread;
MfDblList *MfScPiccolo::plPlugins;
MfDblList *MfScPiccolo::plExtensions;
MfScPiccolo *MfScPiccolo::pdoPiccolo;
MfScSourceResolver *MfScPiccolo::psrResolver;
MfScFunctionRegistry *MfScPiccolo::pfrRegistry;
int MfScPiccolo::bIsRunning;                       // 0x10035254

// 0x1000f2c0 _$E53 (static initializer of sPiccoloName, with the three below)
// 0x1000f2d0 _$E50
// 0x1000f2f0 _$E52
// 0x1000f300 _$E51
static MfString sPiccoloName("Piccolo");

// 0x1000f310 ??0MfScPiccoloDll@@QAE@ABVMfString@@@Z
MfScPiccoloDll::MfScPiccoloDll(const MfString &name)
    : mName(name), mModule(0)
{
}

// 0x1000f330 ??1MfScPiccoloDll@@QAE@XZ
MfScPiccoloDll::~MfScPiccoloDll()
{
    mModule = 0;
}

// 0x1000f340 ?Load@MfScPiccoloDll@@QAEHP6AHABVMfString@@0PAV2@@ZAAV2@PAH@Z
int MfScPiccoloDll::Load(int (*callback)(const MfString &, const MfString &, MfString *), MfString &errors,
                         int *initialized)
{
    errors = NULL_STRING;
    int result = 1;
    *initialized = 0;
    mModule = LoadLibrary(mName);
    if (mModule) {
        int (__stdcall *load)() = (int (__stdcall *)())GetProc("_PscLoad@0");
        if (load && !load()) {
            FreeLibrary(mModule);
            mModule = 0;
        } else {
            int (__stdcall *initialize)(void *, MfString *) =
                (int (__stdcall *)(void *, MfString *))GetProc("_PscInitialize@8");
            if (initialize) {
                MfString message;
                if (!initialize(callback, &message)) {
                    errors << message << '\n';
                    result = 0;
                    FreeLibrary(mModule);
                    mModule = 0;
                } else
                    *initialized = 1;
            } else {
                errors << "Missing PscInitialize in " << mName << '\n';
                MfLogger::Log(1, "Piccolo", "Plugin Initializer", errors);
                result = 0;
                FreeLibrary(mModule);
                mModule = 0;
            }
        }
    } else {
        errors << "Cannot read Plugin Library: " << mName << '\n';
        MfLogger::Log(1, "Piccolo", "Plugin Initializer", errors);
        result = 0;
    }
    return result;
}

// 0x1000f5d0 ?Unload@MfScPiccoloDll@@QAEHAAVMfString@@@Z
int MfScPiccoloDll::Unload(MfString &errors)
{
    errors = NULL_STRING;
    int result = 1;
    int (__stdcall *shutdown)(MfString *) = (int (__stdcall *)(MfString *))GetProc("_PscShutdown@4");
    if (shutdown) {
        MfString message;
        if (!shutdown(&message)) {
            errors << message << '\n';
            result = 0;
        }
    } else {
        errors << "Missing PscShutdown in " << mName << '\n';
        MfLogger::Log(1, "Piccolo", "Plugin Initializer", errors);
        result = 0;
    }
    void (__stdcall *unload)() = (void (__stdcall *)())GetProc("_PscUnload@0");
    if (unload)
        unload();
    return result;
}

// 0x1000f770 ?GetProc@MfScPiccoloDll@@QAEP6GHXZABVMfString@@@Z
FARPROC MfScPiccoloDll::GetProc(const MfString &name)
{
    if (mModule)
        return GetProcAddress(mModule, name);
    return 0;
}

// 0x1000fa70 ?SetAsMessageThread@MfScPiccolo@@SAXXZ
void MfScPiccolo::SetAsMessageThread()
{
    ((MfScThread *)MfThread::GetThread())->mVmEnv->SetAsMessageThread();
}

// 0x1000fa90 ?LoadPiccoloDataPathFunctions@MfScPiccolo@@SAXH@Z
void MfScPiccolo::LoadPiccoloDataPathFunctions(int flags)
{
    if (fsDataPath != NULL_STRING)
        LoadPiccoloModulesFromDirectory(fsDataPath, 5, flags);
}

// EQUIV 0x10010ae0 ?InitializePlugins@MfScPiccolo@@SAHPAVMfString@@@Z: reviewed: the locals get other stack slots and ours merges the two Delete/GetCurrent tails of the name loop into one; same calls, strings and flow (slot uses checked: class-info and compiler names stay apart)
// loads the plugins (psc*.dll) found on the path: the class-info plugin and the
// compiler first, so that the others can use the compiler's constant initializer
int __cdecl MfScPiccolo::InitializePlugins(MfString *errors)
{
    int result = 1;
    MfAiCompiler *compiler;
    MfConstInitFunction constInit = 0;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        constInit = compiler->GetConstInitFunction();
        compiler->Release();
    }
    *errors = NULL_STRING;
    MfStringList names;
    if (!RetrieveDllNames(errors, "^psc.*\\.dll$", &names))
        return 0;
    InitPluginList(&plPlugins);
    MfString classInfoDll;
    MfString compilerDll;
    MfDblListItem *pos;
    MfString *name = (MfString *)names.GetFirst(pos);
    while (name) {
        MfString baseName;
        MfString extension;
        MfString fileName;
        name->SplitPath(0, 0, &baseName, &extension);
        fileName = baseName + extension;
        if (fileName.CaseCompare("PscClassInfo.dll") == 0) {
            classInfoDll = *name;
            names.Delete(pos);
            name = (MfString *)names.GetCurrent(pos);
        } else if (fileName.CaseCompare("PscCompiler.dll") == 0) {
            compilerDll = *name;
            names.Delete(pos);
            name = (MfString *)names.GetCurrent(pos);
        } else
            name = (MfString *)names.GetNext(pos);
    }
    if (classInfoDll != NULL_STRING)
        LoadPiccoloDll(classInfoDll, constInit, errors, &plPlugins, 1);
    if (compilerDll != NULL_STRING) {
        LoadPiccoloDll(compilerDll, constInit, errors, &plPlugins, 1);
        if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
            constInit = compiler->GetConstInitFunction();
            compiler->Release();
        }
    }
    for (name = (MfString *)names.GetFirst(pos); name; name = (MfString *)names.GetNext(pos))
        result = LoadPiccoloDll(*name, constInit, errors, &plPlugins, 1);
    return result;
}

// EQUIV 0x10011180 ?RetrieveDllNames@MfScPiccolo@@CAHPAVMfString@@ABV2@PAVMfStringList@@@Z: reviewed: the original keeps names in ebp (reloaded at the loop top) and tests with test, ours keeps 0 in ebp and compares with cmp; same calls, strings and flow
// collects the paths of the DLLs in the plugin folders whose names match pattern
int __cdecl MfScPiccolo::RetrieveDllNames(MfString *, const MfString &pattern, MfStringList *names)
{
    names->Clear();
    UtilityApp app;
    MfStringList folders;
    app.PluginPath(&folders);
    MfDblListItem *pos;
    MfString *folder = (MfString *)folders.GetFirst(pos);
    while (folder) {
        MfDirectory dir;
        if (*folder == NULL_STRING) {
            MfStringList errors;
            {
                MfString line6("where <appname> is the name of your executable, without the .EXE extension.");
                MfString line5("   \\HKEY_CURRENT_USER\\Software\\MotionFactory\\<appname>\\PATHS\\PLUGINS");
                MfString line4("A plugin folder can be specified for an application with the following key:");
                MfString line3("   \\HKEY_CURRENT_USER\\Software\\MotionFactory\\Global Data\\PATHS\\PLUGINS");
                MfString line2("Specify a plugin folder in the Windows registry. The global PLUGIN key is");
                MfString line1("There are no plugin folder defined.");
                MfString title("Plugin Initializer");
                MfString source("Piccolo");
                errors + line1 + line2 + line3 + line4 + line5 + line6;
                MfLogger::Log(1, source, title, errors);
            }
            MfScDataObject::CheckClassInfoInterface();
            return 0;
        }
        if (dir.Open(*folder) == 0) {
            MfString message = MfString("The folder named '") + *folder + MfString("'");
            MfStringList errors;
            MfString line3("cannot be opened");
            MfString line1("Cannot open a plugin folder.");
            MfString title("Plugin Initializer");
            MfString source("Piccolo");
            errors + line1 + message + line3;
            MfLogger::Log(1, source, title, errors);
        }
        MfRegularExpression dllNames(pattern);
        dllNames.Optimize();
        for (MfDirectoryFileInfo *info = dir.First(); info; info = dir.Next()) {
            MfString name = info->GetFileName();
            if (dllNames.Matches(name) == 1)
                names->Add(info->GetFilePath());
        }
        dir.Close();
        folder = (MfString *)folders.GetNext(pos);
    }
    return 1;
}

// 0x10011820 ?InitPluginList@MfScPiccolo@@CAXPAPAVMfDblList@@@Z
void MfScPiccolo::InitPluginList(MfDblList **list)
{
    if (!*list)
        *list = new MfDblList;
}

// 0x10011860 ?LoadPiccoloDll@MfScPiccolo@@CAHABVMfString@@P6AH00PAV2@@Z1PAPAVMfDblList@@H@Z
int MfScPiccolo::LoadPiccoloDll(const MfString &name, int (*callback)(const MfString &, const MfString &, MfString *),
                                MfString *errors, MfDblList **list, int)
{
    MfDblList *dlls = *list;
    int result = 1;
    MfScPiccoloDll *dll = new MfScPiccoloDll(name);
    MfString message;
    int initialized = 0;
    int loaded = dll->Load(callback, message, &initialized);
    if (!loaded) {
        *errors << message;
        result = 0;
    }
    if (loaded == 1 && initialized == 1)
        dlls->Add(dll);
    else if (dll)
        delete dll;
    MfScDataObject::CheckClassInfoInterface();
    return result;
}

// 0x10012370 ?PMfirstPlugin@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMfirstPlugin(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    MfScPiccoloDll *dll = (MfScPiccoloDll *)plPlugins->GetFirst(mPluginPos);
    if (dll)
        result = MfScDataHandle::CreateData(dll->mName, 0);
    return result;
}

// 0x100123a0 ?PMnextPlugin@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMnextPlugin(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    if (mPluginPos) {
        MfScPiccoloDll *dll = (MfScPiccoloDll *)plPlugins->GetNext(mPluginPos);
        if (dll)
            result = MfScDataHandle::CreateData(dll->mName, 0);
    }
    return result;
}

// 0x100123d0 ?PMfirstExtension@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMfirstExtension(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    MfScPiccoloDll *dll = (MfScPiccoloDll *)plExtensions->GetFirst(mExtensionPos);
    if (dll)
        result = MfScDataHandle::CreateData(dll->mName, 0);
    return result;
}

// 0x10012400 ?PMnextExtension@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMnextExtension(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    if (mExtensionPos) {
        MfScPiccoloDll *dll = (MfScPiccoloDll *)plExtensions->GetNext(mExtensionPos);
        if (dll)
            result = MfScDataHandle::CreateData(dll->mName, 0);
    }
    return result;
}

// EQUIV 0x100127a0 ?PMgetFunctionsInDataPath@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: function bytes identical; only the unwind funclet keeps the list's base-class temporary in another frame slot
// a list of objects (name, module, function, objfile) for the global
// functions that came from object files
MfScDataHandle *MfScPiccolo::PMgetFunctionsInDataPath(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    MfDblList *list;
    MfScStdLib::pslliListInterface->CreateList(&list, &result);
    InternalGlobFuncIterator iterator;
    MfScVmEnv::EnumGlobalHandles(iterator);
    MfDblListItem *pos;
    for (MfString *name = (MfString *)iterator.mNames.GetFirst(pos); name;
         name = (MfString *)iterator.mNames.GetNext(pos)) {
        MfScDataHandle *function;
        MfScVmEnv::GetGlobalVariable(*name, &function);
        if (function) {
            MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(function->GetData());
            if (routine) {
                MfScDataObject *entry = new MfScDataObject(0);
                MfScDataHandle *entryHandle = MfScDataHandle::CreateData(entry);
                entry->SetProperty(entryHandle, "name", MfScDataHandle::CreateData(routine->Name(), 0), 1);
                entry->SetProperty(entryHandle, "module",
                                   MfScDataHandle::CreateData(routine->ModuleName(), 0), 1);
                entry->SetProperty(entryHandle, "function", function, 0);
                entry->SetProperty(entryHandle, "objfile",
                                   MfScDataHandle::CreateData(routine->ObjectFileName(), 0), 1);
                list->Add(entryHandle);
                entryHandle->Release();
            }
            function->Release();
        }
    }
    return result;
}

// 0x10012a60 ?PMgetDataPath@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMgetDataPath(MfScDataHandle *, int, MfScDataHandle **)
{
    return MfScDataHandle::CreateData(fsDataPath, 0);
}

// 0x100132b0 ?GetFunctionRegistry@MfScPiccolo@@SAPAVMfAiFunctionRegistry@@XZ
MfAiFunctionRegistry *MfScPiccolo::GetFunctionRegistry()
{
    MfAiFunctionRegistry *registry = pfrRegistry;
    if (MfScDebugger::pdbgTheDebugger
        && !MfScDebugger::pdbgTheDebugger->GetInterface(2, (MfAi **)&registry))
        registry->Release();
    return registry;
}


// 0x1000fc40 ?EnumHandle@InternalGlobFuncIterator@@UAEXABVMfString@@PAVMfScDataHandle@@@Z
void InternalGlobFuncIterator::EnumHandle(const MfString &name, MfScDataHandle *handle)
{
    if (handle && handle != pdhUndefined && handle->IsType(0x800)) {
        MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(handle->GetData());
        if (routine && (routine->Flags() & 1))
            mNames.Add(name);
    }
}

// EQUIV 0x1000fca0 ?FlushPiccoloDataPathFunctions@MfScPiccolo@@SAXXZ: reviewed: function bytes identical; only the unwind funclet keeps the list's base-class temporary in another frame slot
void MfScPiccolo::FlushPiccoloDataPathFunctions()
{
    InternalGlobFuncIterator iterator;
    MfScVmEnv::EnumGlobalHandles(iterator);
    MfDblListItem *pos;
    for (MfString *name = (MfString *)iterator.mNames.GetFirst(pos); name;
         name = (MfString *)iterator.mNames.GetNext(pos))
        MfScVmEnv::ClearGlobalVariable(*name, 0);
}

// 0x1000fd90 ??1InternalGlobFuncIterator@@UAE@XZ

// EQUIV 0x1000fe30 ?LoadPiccoloModulesFromDirectory@MfScPiccolo@@SAXABVMfString@@HH@Z: reviewed: function bytes identical; only the unwind funclet keeps the list's base-class temporary in another frame slot
void __cdecl MfScPiccolo::LoadPiccoloModulesFromDirectory(const MfString &directory, int flags, int reportErrors)
{
    MfRegularExpression objectFiles(".*\\.pobj$");
    objectFiles.Optimize();
    MfStringList errors;
    MfDirectory dir;
    if (dir.Open(directory) == 1) {
        for (MfDirectoryFileInfo *info = dir.First(); info; info = dir.Next()) {
            MfString name = info->GetFileName();
            if (objectFiles.Matches(name) == 1)
                ReadSinglePiccoloModule(info->GetFilePath(), flags, reportErrors);
        }
        dir.Close();
    } else if (reportErrors == 1) {
        errors.Clear();
        MfString message("The following folder could not be opened:");
        MfString title("Function Reader");
        MfString source("Piccolo");
        errors + message + directory;
        MfLogger::Log(1, source, title, errors);
    }
}

// EQUIV 0x100100f0 ?ReadSinglePiccoloModule@MfScPiccolo@@SAXABVMfString@@HH@Z: reviewed: 87% by bytes; with the null test of the handle before the dynamic_cast written out (a ternary), the only differences left are the inlined First()/Next() storing their result once after the join (original) or in each arm (here), its slot (0x40 vs 0x3c) and the slots of the other locals; calls, arguments, conditions and order agree. Was: near miss (86%); review 2026-10-16 fixed the order of side effects: the three Log calls take the list expression itself, so the original logs before the expression temporaries die; left: the original tests the function for null again before the dynamic_cast and keeps it at 0x40 (`if (function)` reproduces the test but not the slot), libMain starts from a zeroed esi, and the message strings get other slots
void __cdecl MfScPiccolo::ReadSinglePiccoloModule(const MfString &fileName, int flags, int reportErrors)
{
    MfStringList errors;
    MfScPiccoloObjectFile objectFile;
    MfScPiccoloObjectFile::MfScPOFErrorCode error = objectFile.Open(fileName, 0, 1, 0);
    if (error == MfScPiccoloObjectFile::kPOFOk) {
        MfScHandleList functions;
        error = objectFile.GrabFunctions(&functions);
        if (error == MfScPiccoloObjectFile::kPOFOk) {
            MfScDataHandle *libMain = 0;
            for (MfScDataHandle *function = functions.First(); function; function = functions.Next()) {
                MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
                if (flags == 6 && routine->Name() == "LibMain") {
                    libMain = function;
                    function->AddRef();
                    continue;
                }
                if (flags == 5)
                    routine->mFlags |= 1;
                MfScDataHandle *existing;
                if (MfScVmEnv::GetGlobalVariable(routine->Name(), &existing) == 0) {
                    MfScBytecodeRoutine *old;
                    if (existing)
                        old = dynamic_cast<MfScBytecodeRoutine *>(existing->GetData());
                    else
                        old = 0;
                    if (!old) {
                        errors.Clear();
                        MfString line3("that was not a function. The object name is ");
                        MfString line2("An attempt was made to replace a Piccolo Object ");
                        MfString line1("Attempting to replace a non-function.");
                        MfString title("Function Reader");
                        MfString source("Piccolo");
                        errors + line1 + line2 + line3 + routine->Name();
                        MfLogger::Log(2, source, title, errors);
                        continue;
                    }
                    if (reportErrors == 1) {
                        MfString message = MfString("The function named '") + routine->Name() +
                                           MfString("' that was previously defined in the module");
                        errors.Clear();
                        MfString line3("is being replaced by the function defined in the module");
                        MfString line1("Replacing an existing function.");
                        MfString title("Function Reader");
                        MfString source("Piccolo");
                        MfLogger::Log(1, source, title,
                                      errors + line1 +
                                          (MfString("The function named '") + routine->Name() +
                                           MfString("' that was previously defined in the module")) +
                                          (MfString("    ") + old->ObjectFileName()) + line3 +
                                          (MfString("    ") + routine->ObjectFileName()));
                    }
                    existing->Release();
                }
                MfScVmEnv::SetGlobalVariable(routine->Name(), function, 0, 0);
            }
            if (libMain) {
                MfScVirtualMachine vm;
                MfScDataHandle *result = vm.ExecuteFunction(libMain, 0);
                if (result)
                    result->Release();
                libMain->Release();
            }
        } else {
            errors.Clear();
            MfString line3("The error return value was");
            MfString line2("The file was named");
            MfString line1("Cannot retrieve functions from a Piccolo Object file.");
            MfString title("Function Reader");
            MfString source("Piccolo");
            MfLogger::Log(2, source, title,
                          errors + line1 + line2 + fileName + line3 + MfScPiccoloObjectFile::ECToString(error));
        }
    } else {
        errors.Clear();
        MfString line3("The error return value was");
        MfString line2("The file was named");
        MfString line1("Could not open Piccolo Object file.");
        MfString title("Function Reader");
        MfString source("Piccolo");
        MfLogger::Log(2, source, title,
                      errors + line1 + line2 + fileName + line3 + MfScPiccoloObjectFile::ECToString(error));
    }
}

// 0x10011960 ?ShutdownExtensions@MfScPiccolo@@SAXXZ
void MfScPiccolo::ShutdownExtensions()
{
    MfString errors;
    if (plExtensions) {
        MfDblListItem *pos = 0;
        for (MfScPiccoloDll *dll = (MfScPiccoloDll *)plExtensions->GetFirst(pos); dll;
             dll = (MfScPiccoloDll *)plExtensions->GetNext(pos)) {
            dll->Unload(errors);
            delete dll;
        }
        delete plExtensions;
        plExtensions = 0;
    }
}

// registers a method (taking numArgs arguments) of the object "Piccolo"
#define PICCOLO_METHOD(name, method, numArgs) \
    { \
        MfString fullName("Piccolo::" name); \
        MfString propertyName(name); \
        pdoPiccolo->SetProperty(0, propertyName, \
                                MfScDataHandle::CreateData((MfScMethod)&MfScPiccolo::method, fullName, numArgs), 1); \
    }

// 0x10011bd0 ?InitializePiccoloObject@MfScPiccolo@@CAXXZ
void __cdecl MfScPiccolo::InitializePiccoloObject()
{
    if (!pdoPiccolo) {
        pdoPiccolo = new MfScPiccolo;
        pdoPiccolo->Name() = "Piccolo<class>";
        MfScDataHandle *handle = MfScDataHandle::CreateData(pdoPiccolo);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(sPiccoloName, handle, 1, 0);
        PICCOLO_METHOD("firstPlugin", PMfirstPlugin, 0);
        PICCOLO_METHOD("nextPlugin", PMnextPlugin, 0);
        PICCOLO_METHOD("firstExtension", PMfirstExtension, 0);
        PICCOLO_METHOD("nextExtension", PMnextExtension, 0);
        PICCOLO_METHOD("AddVariable", PMaddVariable, 2);
        PICCOLO_METHOD("RemoveVariable", PMremoveVariable, 1);
        PICCOLO_METHOD("DoesVariableExist", PMdoesVariableExist, 1);
        PICCOLO_METHOD("LoadFunctions", PMloadFunctions, 0);
        PICCOLO_METHOD("FlushFunctions", PMflushFunctions, 0);
        PICCOLO_METHOD("GetFunctionsInDataPath", PMgetFunctionsInDataPath, 0);
        PICCOLO_METHOD("GetDataPath", PMgetDataPath, 0);
        PICCOLO_METHOD("SetDataPath", PMsetDataPath, 1);
        PICCOLO_METHOD("CallStack", PMcallStack, 0);
        PICCOLO_METHOD("QueryPropertyNames", PMqueryPropertyNames, 1);
        PICCOLO_METHOD("GetClassName", PMgetClassName, 1);
        PICCOLO_METHOD("GetInternalClassName", PMgetInternalClassName, 1);
    }
}

// 0x10012310 ?ShutdownPiccoloObject@MfScPiccolo@@CAXXZ
void MfScPiccolo::ShutdownPiccoloObject()
{
    if (pdoPiccolo) {
        MfScDataHandle *handle;
        MfScVmEnv::GetGlobalVariable(sPiccoloName, &handle);
        if (handle) {
            handle->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(sPiccoloName, 0);
            handle->Release();
        }
        pdoPiccolo = 0;
    }
}

// the script environment of the calling thread
#define THREAD_ENV() (((MfScThread *)MfThread::GetThread())->mVmEnv)

// 0x10012430 ?PMaddVariable@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMaddVariable(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScVmEnv *env;
    if (argv[0] == 0 || argv[1] == 0) {
        env = THREAD_ENV();
        env->ThrowException(6, "Both parameters must be non-nil.");
        return 0;
    }
    if (argv[0]->IsType(0x100)) {
        MfScVmEnv::SetGlobalVariable(*(MfScObjString *)argv[0]->GetData(), argv[1], 0, 0);
        MfScDataHandle *value = argv[1];
        value->AddRef();
        return value;
    }
    env = THREAD_ENV();
    env->ThrowException(11, "The first parameter must be a string.");
    return 0;
}

// 0x10012540 ?PMremoveVariable@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMremoveVariable(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *old = 0;
    MfScVmEnv *env;
    if (argv[0] == 0) {
        env = THREAD_ENV();
        env->ThrowException(6, "Specify a non-nil variable name.");
        return 0;
    }
    if (argv[0]->IsType(0x100)) {
        MfScObjString *name = (MfScObjString *)argv[0]->GetData();
        MfScVmEnv::GetGlobalVariable(*name, &old);
        MfScVmEnv::SetGlobalVariable(*name, 0, 0, 0);
        return old;
    }
    env = THREAD_ENV();
    env->ThrowException(11, "The first parameter must be a string.");
    return 0;
}

// 0x10012660 ?PMdoesVariableExist@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMdoesVariableExist(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = MfScDataHandle::CreateData(0);
    MfScVmEnv *env = THREAD_ENV();
    if (argv[0] == 0) {
        env->ThrowException(6, "Specify a non-nil variable name.");
        return 0;
    }
    if (argv[0]->IsType(0x100)) {
        MfScDataHandle *value;
        if (env->GetVariable(*(MfScObjString *)argv[0]->GetData(), &value) == MfScVariableStack::kVarOk) {
            value->Release();
            result->SetInt(1);
        }
        return result;
    }
    env->ThrowException(11, "The first parameter must be a string.");
    return 0;
}

// 0x10012a80 ?PMsetDataPath@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMsetDataPath(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScVmEnv *env;
    if (argv[0] == 0) {
        env = THREAD_ENV();
        env->ThrowException(6, "Specify a non-nil data path.");
        return 0;
    }
    if (argv[0]->IsType(0x100)) {
        MfScDataHandle *old = MfScDataHandle::CreateData(fsDataPath, 0);
        fsDataPath = *(MfScObjString *)argv[0]->GetData();
        return old;
    }
    env = THREAD_ENV();
    env->ThrowException(11, "The data path must be a string.");
    return 0;
}

// 0x10013200 ?PMgetInternalClassName@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMgetInternalClassName(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result;
    if (argv[0])
        result = MfScDataHandle::CreateData(argv[0]->TypeInfo(), 0);
    else
        result = MfScDataHandle::CreateData(MfString("nil"), 0);
    return result;
}

// EQUIV 0x10012e80 ?PMqueryPropertyNames@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: function bytes identical; only the unwind funclet keeps the list's base-class temporary in another frame slot
MfScDataHandle *MfScPiccolo::PMqueryPropertyNames(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        MfScDataObject *object = dynamic_cast<MfScDataObject *>(argv[0]->GetData());
        if (object) {
            MfStringList names;
            object->GetPropertyNames(&names, 0);
            object->GetInstanceProperties(&names, 1);
            MfDblList *list;
            MfScStdLib::pslliListInterface->CreateList(&list, &result);
            MfDblListItem *pos;
            for (MfString *name = (MfString *)names.GetFirst(pos); name; name = (MfString *)names.GetNext(pos)) {
                MfScDataHandle *handle = MfScDataHandle::CreateData(*name, 0);
                list->Add(handle);
                handle->Release();
            }
        } else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(15, "The object does not have properties.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "Specify a non-nil object.");
    }
    return result;
}

// 0x10013060 ?PMgetClassName@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScPiccolo::PMgetClassName(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(0x7c0)) {
            MfScDataObject *object = dynamic_cast<MfScDataObject *>(argv[0]->GetData());
            if (object)
                result = MfScDataHandle::CreateData(object->GetClassNameA(), 0);
            else {
                MfString message("Cannot get the class name.");
                MfScThread *thread = (MfScThread *)MfThread::GetThread();
                thread->mVmEnv->ThrowException(15, message);
                return 0;
            }
        } else if (argv[0]->IsType(0x10))
            result = MfScDataHandle::CreateData(MfString("DATA_INT"), 0);
        else if (argv[0]->IsType(0x20))
            result = MfScDataHandle::CreateData(MfString("DATA_FLOAT"), 0);
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "Specify a non-nil object.");
    }
    return result;
}

// EQUIV 0x10012bb0 ?PMcallStack@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: cmp and mov in the loop's GetNext swapped (independent, same flags); EH funclets match
MfScDataHandle *MfScPiccolo::PMcallStack(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    MfDblList *list;
    MfScStdLib::pslliListInterface->CreateList(&list, &result);
    MfScVmCallStackList stack;
    MfScThread *thread = (MfScThread *)MfThread::GetThread();
    thread->mVmEnv->GetCurrentCallStack(&stack);
    MfDblListItem *pos;
    for (MfScVmCallStackEntry *entry = (MfScVmCallStackEntry *)stack.GetFirst(pos); entry;
         entry = (MfScVmCallStackEntry *)stack.GetNext(pos)) {
        MfScDataObject *call = new MfScDataObject;
        MfScDataHandle *handle = MfScDataHandle::CreateData(call);
        call->SetProperty(handle, "name", MfScDataHandle::CreateData(entry->mFunction, 0), 1);
        call->SetProperty(handle, "module", MfScDataHandle::CreateData(entry->mModule, 0), 1);
        call->SetProperty(handle, "offset", MfScDataHandle::CreateData(entry->mOffset), 1);
        call->SetProperty(handle, "line", MfScDataHandle::CreateData(entry->mLine), 1);
        list->Add(handle);
        handle->Release();
    }
    return result;
}

// 0x10012e00 ??1MfScVmCallStackList@@UAE@XZ
// 0x10012e60 ??_GMfScVmCallStackList@@UAEPAXI@Z

// 0x10011a20 ?Shutdown@MfScPiccolo@@SAXPAVMfString@@@Z
void MfScPiccolo::Shutdown(MfString *errors)
{
    if (!bInitialized && !pthrdMainThread)
        return;
    MfScDataHandle::ShutdownGCTimeoutThread();
    psrResolver->Release();
    psrResolver = 0;
    pfrRegistry->Clear();
    pfrRegistry->Release();
    pfrRegistry = 0;
    *errors = NULL_STRING;
    if (plPlugins) {
        MfDblListItem *pos = 0;
        for (MfScPiccoloDll *dll = (MfScPiccoloDll *)plPlugins->GetFirst(pos); dll;
             dll = (MfScPiccoloDll *)plPlugins->GetNext(pos)) {
            MfString message;
            if (!dll->Unload(message))
                *errors << message;
            MfScDataHandle::GarbageCollect();
            delete dll;
        }
        delete plPlugins;
        plPlugins = 0;
    }
    delete pthrdMainThread;
    pthrdMainThread = 0;
    MfScBreakpointManager::Shutdown();
    delete MfScConsoleManager::pcmManager;
    ShutdownPiccoloObject();
    MfScObjString::ClearProperties();
    MfScObjInteger::ClearClassInfo();
    MfScObjFloat::ClearClassInfo();
    MfScDataHandle::GarbageCollect();
    MfScDataObject::DestroyClassInfo();
    MfScVmEnv::Shutdown();
    MfScDataHandle::GarbageCollect();
    MfScDataHandle::Shutdown();
    MfScThreadManager::Shutdown();
    MfScVmManager::Shutdown();
    MfThread::ReleaseTls();
    MfScDiagnostics::ClearAllOutput();
    bInitialized = 0;
}

// EQUIV 0x10010f70 ?InitializeExtensions@MfScPiccolo@@SAHPAVMfString@@@Z: reviewed: the MfString destructor is called through a register in ours and the unwind funclets keep the list temporary in other frame slots; same calls and flow
int MfScPiccolo::InitializeExtensions(MfString *errors)
{
    int result = 1;
    MfConstInitFunction constInit = 0;
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        constInit = compiler->GetConstInitFunction();
        compiler->Release();
    }
    *errors = NULL_STRING;
    MfStringList names;
    if (!RetrieveDllNames(errors, "^pxt.*\\.dll$", &names))
        return 0;
    InitPluginList(&plExtensions);
    MfDblListItem *pos;
    for (MfString *name = (MfString *)names.GetFirst(pos); name; name = (MfString *)names.GetNext(pos))
        result = LoadPiccoloDll(*name, constInit, errors, &plExtensions, 1);
    return result;
}

// EQUIV 0x1000f7a0 ?Initialize@MfScPiccolo@@SAXXZ: reviewed: function bytes identical; only the unwind funclet keeps the list's base-class temporary in another frame slot
void MfScPiccolo::Initialize()
{
    if (fsDataPath == NULL_STRING) {
        MfString path;
        path.GetVariable("PICCOLOPATH");
        if (path != NULL_STRING) {
            MfStringList dirs;
            path.Tokenize(gPathSeparator, dirs);
            if (dirs.GetCount() > 0) {
                MfDblListItem *pos;
                fsDataPath = *(MfString *)dirs.GetFirst(pos);
            }
        }
    }
    MfScVmManager::Startup();
    MfScThreadManager::Startup();
    MfScBreakpointManager::Startup();
    psrResolver = new MfScSourceResolver;
    pfrRegistry = new MfScFunctionRegistry;
    MfScFileProvider *provider = new MfScFileProvider;
    MfString type;
    provider->GetType(type);
    psrResolver->Register(type, provider);
    provider->Release();
    pthrdMainThread = new MfScThread(1);
    MfScObjString::Initialize();
    MfScObjInteger::AddClassInfo();
    MfScObjFloat::AddClassInfo();
    InitializePiccoloObject();
    MfScConsoleManager::Exemplar exemplar;
    new MfScConsoleManager(exemplar);
    bInitialized = 1;
}

// EQUIV 0x1000fac0 ?LoadPiccoloLibFunctions@MfScPiccolo@@SAXH@Z: reviewed: function bytes identical; only the unwind funclet keeps the list's base-class temporary in another frame slot
void MfScPiccolo::LoadPiccoloLibFunctions(int flags)
{
    MfString lib;
    lib.GetVariable("PICCOLOLIB");
    if (lib != NULL_STRING) {
        MfStringList dirs;
        lib.Tokenize(gPathSeparator, dirs);
        MfDblListItem *pos;
        for (MfString *dir = (MfString *)dirs.GetFirst(pos); dir; dir = (MfString *)dirs.GetNext(pos))
            LoadPiccoloModulesFromDirectory(*dir, 6, flags);
    }
}
