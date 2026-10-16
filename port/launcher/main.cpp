// main.cpp - the Steamboat executable's entry point: the arguments become the Windows command
// line, the installer's files are put in place on first start (vfs::install_defaults), then
// the game's WinMain runs as Windows would start it, with the arguments after the program name.
#include <windows.h>
#include <string>

extern "C" void port_set_command_line(int argc, char **argv);
namespace win32 {
namespace vfs {
void install_defaults();
}
}  // namespace win32

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

int main(int argc, char **argv)
{
    port_set_command_line(argc, argv);
    win32::vfs::install_defaults();
    std::string args;
    for (int i = 1; i < argc; i++) {
        if (i > 1)
            args += " ";
        args += argv[i];
    }
    return WinMain(GetModuleHandleA(0), 0, (LPSTR)args.c_str(), SW_SHOWNORMAL);
}
