// PscDebugger.dll 0x10008370-0x10008421: the glyphs of a breakpoint the user set.
#include "PscDebugger.h"

// 0x10008370 ?ShowGlyph@MfPdbSetBreakpoint@@QAEXH@Z
void MfPdbSetBreakpoint::ShowGlyph(int remove)
{
    if (remove == 1) {
        if (mEnabled) {
            CPiccoloWnd::UnsetGlyphOnLineForModule(mModule, mLine, kGlyphBreakpoint);
            MfPeCodeEditor::RemoveGlyphAtLineForModule(mModule, mLine, kGlyphBreakpoint);
        } else {
            CPiccoloWnd::UnsetGlyphOnLineForModule(mModule, mLine, kGlyphDisabledBreakpoint);
            MfPeCodeEditor::RemoveGlyphAtLineForModule(mModule, mLine, kGlyphDisabledBreakpoint);
        }
    } else {
        if (mEnabled) {
            CPiccoloWnd::SetGlyphOnLineForModule(mModule, mLine, kGlyphBreakpoint);
            MfPeCodeEditor::AddGlyphAtLineForModule(mModule, mLine, kGlyphBreakpoint);
        } else {
            CPiccoloWnd::SetGlyphOnLineForModule(mModule, mLine, kGlyphDisabledBreakpoint);
            MfPeCodeEditor::AddGlyphAtLineForModule(mModule, mLine, kGlyphDisabledBreakpoint);
        }
    }
}
