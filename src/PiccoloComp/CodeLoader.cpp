// PiccoloComp.dll unit CodeLoader (0x10005ad0-0x10006af2), part of MfPcCodeLoader.cpp
#include "CodeLoader_unit.h"

// 0x10005ad0 ??0MfPcCodeLoader@@QAE@XZ (inline, dllexport class)
// 0x10005ae0 ??4MfPcCodeLoader@@QAEAAV0@ABV0@@Z (compiler-generated)

// 0x10005af0 ?UpdatePiccoloDataPathFiles@MfPcCodeLoader@@SAXH@Z
void MfPcCodeLoader::UpdatePiccoloDataPathFiles(int force)
{
    UpdateFilesInDirectoryPath(MfScPiccolo::fsDataPath, force);
}

// 0x10005b10 ?UpdatePiccoloLibFiles@MfPcCodeLoader@@SAXH@Z
void MfPcCodeLoader::UpdatePiccoloLibFiles(int force)
{
    MfString path;
    {
        MfString name("PICCOLOLIB");
        path.GetVariable(name);
    }
    if (path != NULL_STRING)
        UpdateFilesInDirectoryPath(path, force);
}

// 0x10005bc0 ?UpdateFilesInDirectoryPath@MfPcCodeLoader@@CAXABVMfString@@H@Z
void MfPcCodeLoader::UpdateFilesInDirectoryPath(const MfString &dirPath, int force)
{
    if (dirPath != NULL_STRING) {
        MfStringList messages;
        MfStringList dirs;
        dirPath.Tokenize(g_pcPathSeparator, dirs);
        MfRegularExpression re(".*\\.pc$");
        re.Optimize();
        MfDblListItem *it;
        for (MfString *dir = dirs.FirstString(it); dir; dir = dirs.NextString(it)) {
            MfDirectory d;
            if (d.Open(*dir) == 1) {
                for (MfDirectoryFileInfo *fi = d.GetFirstFile(); fi; fi = d.GetNextFile()) {
                    MfString name = fi->GetFileName();
                    if (re.Matches(name) == 1)
                        UpdateSingleFile(fi->GetFilePath(), force, 0);
                }
                d.Close();
            } else {
                messages.Clear();
                MfString text("The following directory could not be opened:");
                MfString module("Source File Reader");
                MfString product("Piccolo");
                messages.Append(text);
                messages.Append(*dir);
                MfLogger::Log(1, product, module, messages);
            }
        }
    }
}

// Compiles the source file into its .pobj next to it when the object file is
// missing or older (or when forced); the compiled functions go to handles.
// EQUIV 0x10005f60 ?UpdateSingleFile@MfPcCodeLoader@@SAHABVMfString@@HPAVMfScHandleList@@@Z: reviewed: (89% similar, 2 bytes of the body differ) the address of the ECToString result is pushed from edx instead of eax; the unwind funclets of messages keep their cast temporary 3 slots deeper (temporary numbering of the inline list helpers); same instructions, frame and EH states otherwise
int MfPcCodeLoader::UpdateSingleFile(const MfString &sourcePath, int force, MfScHandleList *handles)
{
    int result = 1;
    MfStringList messages;
    if (handles)
        handles->Clear();
    MfString drive;
    MfString directory;
    MfString name;
    MfString ext;
    MfString objectPath;
    sourcePath.SplitPath(&drive, &directory, &name, &ext);
    objectPath.MakePath(drive, directory, name, ".pobj");
    HANDLE sourceFile = CreateFile(sourcePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL, 0);
    if (sourceFile == INVALID_HANDLE_VALUE) {
        messages.Clear();
        MfString text("The following Piccolo source file could not be opened:");
        MfString title("Cannot open Piccolo Source file.");
        MfString module("Source File Reader");
        MfString product("Piccolo");
        messages.Append(title);
        messages.Append(text);
        messages.Append(sourcePath);
        MfLogger::Log(1, product, module, messages);
        return 0;
    }
    HANDLE objectFile = CreateFile(objectPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL, 0);
    BY_HANDLE_FILE_INFORMATION sourceInfo;
    BY_HANDLE_FILE_INFORMATION objectInfo;
    GetFileInformationByHandle(sourceFile, &sourceInfo);
    CloseHandle(sourceFile);
    MfFileTime sourceTime(sourceInfo.ftLastWriteTime);
    MfFileTime objectTime;
    if (objectFile != INVALID_HANDLE_VALUE) {
        GetFileInformationByHandle(objectFile, &objectInfo);
        CloseHandle(objectFile);
        objectTime = objectInfo.ftLastWriteTime;
    }
    if (force || objectFile == INVALID_HANDLE_VALUE || CompareFileTime(sourceTime, objectTime) > 0) {
        MfString source;
        if (source.ReadFromFile(sourcePath) == 1) {
            MfPcCompiler *compiler = MfPcCompiler::GetMainCompiler();
            MfStringList errors;
            MfPcStringListErrorCB errorCB(errors, 1);
            if (compiler->Compile(sourcePath, "FILE", source, &errorCB, 1)) {
                if (compiler->CodeType() == 2 && compiler->Functions().GetCount() > 0) {
                    MfScPiccoloObjectFile pob;
                    if (pob.Open(objectPath, 1, 1, 0) == MfScPiccoloObjectFile::kOk) {
                        for (MfScDataHandle *f = compiler->Functions().First(); f; f = compiler->Functions().Next())
                            pob.AddFunction(f);
                        if (pob.WriteClose() != MfScPiccoloObjectFile::kOk) {
                            MfString message;
                            message = MfString("Error writing Piccolo object file.\n")
                                    + MfString("There was an error attempting to write the Piccolo object file when\n")
                                    + MfString("compiling the file ") + sourcePath + MfString("\n")
                                    + MfString("The error returned was '") + pob.ECToString() + MfString("'");
                            MfLogger::Log(2, "Piccolo", "Source File Reader", message);
                        }
                    }
                    if (handles)
                        handles->Copy(compiler->Functions());
                }
                compiler->ClearCodeType();
                compiler->Functions().Clear();
            } else {
                {
                    MfString text("The following errors were recorded:");
                    MfString title("Error Compile Piccolo Code.");
                    messages.Append(title);
                    messages.Append(text);
                }
                messages.Merge(errors, MfDblList::LP_AFTER, 0);
                MfLogger::Log(1, "Piccolo", "Source File Reader", messages);
                result = 0;
            }
        } else {
            messages.Clear();
            {
                MfString text("The following Piccolo source file could not be opened:");
                MfString title("Error reading Piccolo Source file.");
                MfString module("Source File Reader");
                MfString product("Piccolo");
                messages.Append(title);
                messages.Append(text);
                messages.Append(sourcePath);
                MfLogger::Log(1, product, module, messages);
            }
            result = 0;
        }
    }
    return result;
}
