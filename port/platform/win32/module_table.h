// module_table.h - what the Windows loader knew of a DLL, for the modules linked into the
// executable. port/flags.py (module_table) writes one table per module from its objects;
// each registers itself before main, and module.cpp loads them as Windows loaded the DLLs.
// Plain C, without windows.h: the generated tables include only this.
#ifndef PORT_MODULE_TABLE_H
#define PORT_MODULE_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

// A function GetProcAddress finds: a C name, or the symbol of a free C++ function
// (_Z<length><name>...), as the module's source names it (without the module prefix
// the build gives some names, see RENAMES in port/flags.py).
struct port_export {
    const char *name;
    void (*address)(void);
};

struct port_module {
    const char *dll;                               // "PxtMath.dll", "Steamboat.exe"; 0: the platform
                                                   // (the Windows system DLLs)
    int (*dll_main)(void *, unsigned, void *);     // its DllMain, or 0
    const char *const *imports;                    // the DLLs it imports from, 0-terminated
    const struct port_export *exports;
    unsigned export_count;
    int at_start;                                  // loaded with the executable, not by LoadLibrary
};

void port_module_register(const struct port_module *m);
// Loads the modules the executable imports from: their DllMain runs, the modules they
// import from first.
void port_modules_start(void);

#ifdef __cplusplus
}
#endif

#endif
