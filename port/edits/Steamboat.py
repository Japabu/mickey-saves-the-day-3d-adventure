# Steamboat edits for the port (see port/rewrite.py, EDITS).
EDITS = {
    'Steamboat/GameApp.cpp': [
        # 64-bit: the window keeps the game engine pointer in GWL_USERDATA, which holds a
        # LONG (32 bits); the pointer-sized calls keep all of it.
        ('SetWindowLong(hWnd, GWL_USERDATA, (LONG)game);',
         'SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)game);', 1),
        ('(IMBGameEngine *)GetWindowLong(hWnd, GWL_USERDATA);',
         '(IMBGameEngine *)GetWindowLongPtr(hWnd, GWLP_USERDATA);', 1),
        # One executable: mbruntime's operator new and delete, which are the same as these
        # (MBmalloc_release, MBfree_release), are the ones linked.
        ('void *__cdecl operator new(unsigned int size)\n{',
         '__attribute__((weak)) void *__cdecl operator new(unsigned int size)\n{', 1),
        ('void __cdecl operator delete(void *ptr)\n{',
         '__attribute__((weak)) void __cdecl operator delete(void *ptr)\n{', 1),
    ],
}
