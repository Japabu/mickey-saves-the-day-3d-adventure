#pragma once
#include <windows.h>
#include "MfAiSourceResolver.h"

// The source type "FILE": script modules are files.
class MfScFileProvider : public MfAiSourceType {
public:
    MfScFileProvider();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int GetType(MfString &type);
    virtual int ReadSource(MfString &module, MfString &text, int flags);
    virtual int WriteSource(const MfString &file, MfString &text, int flags);
    virtual int SplitPath(const MfString &path, MfStringList *dirs);
    virtual int IsSameModule(const MfString &module, const MfString &name);
    virtual ~MfScFileProvider();

    // does the file exist?
    static int FileExists(const MfString &file);
    // lets the user find the file name in a directory; dir gets the directory
    static int BrowseForFile(const MfString &name, MfString *dir);

private:
    // the hidden window that closes the file dialog of BrowseForFile
    static void RegisterHiddenClass();
    static UINT CALLBACK FileDialogHook(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK HiddenWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

    // the file name ReadSource looks for (name and extension of the module)
    static MfString sFileName;

private:
    int mRefCount;                  // +0x04
};
