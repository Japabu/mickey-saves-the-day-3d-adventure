// The error messages of the actor requests (MfReportError): per code the
// tokens of the text (format directives like %s, %d, %f, %underline_line,
// %state_position, %break and %<table> or literal text), a log category, a
// title and the level from which it is reported. The names are invented.
#ifndef RUNTIMELIB_MFERRORMESSAGES_H
#define RUNTIMELIB_MFERRORMESSAGES_H
#include "MfStringList.h"

struct MfErrorMessage {
    MfStringList mTokens;                                   // 0x00
    int mCategory;                                          // 0x14 MfLogger level
    MfString mTitle;                                        // 0x18
    int mLevel;                                             // 0x1c
};

class MfErrorTable {
public:
    enum { NUM_MESSAGES = 82 };
    static MfErrorMessage sMessages[NUM_MESSAGES];          // 100f4b70
};

// the names a %<table> token can expand to: per table 30 names
class MfErrorText {
public:
    static MfString sNames[10][30];                         // 100f46c0
    static MfStringList s55c8;
    static MfStringList s55e8;
    static MfStringList s55b0;
    static MfStringList sTables;                            // the table names
    static int sReportLevel;                                // 100f55dc
};

void MfReportError(MfString actorName, int code, ...);

#endif
