// The registry section and entries of the debugger's settings. Each source
// that includes this header has its own copy of the pointers (the breakpoint
// manager's at 0x1001605c, the debugger frame's at 0x100168dc).
#ifndef PDBOPTIONS_H
#define PDBOPTIONS_H

static char *pszDebuggerOptions = "Piccolo Debugger Options";
static char *pszX = "x";
static char *pszY = "y";
static char *pszCX = "cx";
static char *pszCY = "cy";
static char *pszPaneSize = "Pane Size";
static char *pszBreakpointCount = "BreakpointCount";
static char *pszBreakpoint = "Breakpoint";

#endif
