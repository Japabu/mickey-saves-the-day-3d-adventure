# Steamboat edits for the port (see port/rewrite.py, EDITS).
EDITS = {
    # 64-bit: the window keeps the game engine pointer in GWL_USERDATA, which holds a LONG
    # (32 bits); the pointer-sized calls keep all of it.
    'Steamboat/GameApp.cpp': [
        ('SetWindowLong(hWnd, GWL_USERDATA, (LONG)game);',
         'SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)game);', 1),
        ('(IMBGameEngine *)GetWindowLong(hWnd, GWL_USERDATA);',
         '(IMBGameEngine *)GetWindowLongPtr(hWnd, GWLP_USERDATA);', 1),
    ],
}
