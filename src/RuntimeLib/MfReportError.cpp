// MfReportError: the error report of an actor request, formatted from the
// message table (RuntimeLib.dll 100205d0-10020d30).
#include "MfErrorMessages.h"
#include <stdarg.h>
#include "MfLogger.h"

class MfNullString;
extern __declspec(dllimport) MfNullString NULL_STRING;

// 0x100205d0 _$E15 (original $E: the static initializer of sNames, with 100205e0-10020610)
// 0x100205e0 _$E11
// 0x10020600 _$E14
// 0x10020610 _$E12
MfString MfErrorText::sNames[10][30];
// 0x10020640 _$E20 (original $E: the static initializer of s55c8, with 10020650-10020670)
// 0x10020650 _$E17
// 0x10020660 _$E19
// 0x10020670 _$E18
MfStringList MfErrorText::s55c8;
// 0x10020690 _$E25 (original $E: the static initializer of s55e8, with 100206a0-100206c0)
// 0x100206a0 _$E22
// 0x100206b0 _$E24
// 0x100206c0 _$E23
MfStringList MfErrorText::s55e8;
// 0x100206e0 _$E30 (original $E: the static initializer of s55b0, with 100206f0-10020710)
// 0x100206f0 _$E27
// 0x10020700 _$E29
// 0x10020710 _$E28
MfStringList MfErrorText::s55b0;
// 0x10020730 _$E35 (original $E: the static initializer of sTables, with 10020740-10020760)
// 0x10020740 _$E32
// 0x10020750 _$E34
// 0x10020760 _$E33
MfStringList MfErrorText::sTables;
int MfErrorText::sReportLevel;

int FindName(MfStringList *list, MfString name);

// EQUIV 0x10020780 ?MfReportError@@YAXVMfString@@HZZ: reviewed: the step to the next token (the list item's next pointer) goes through ecx and then eax in the original and straight into eax here; registers only, same loads, stores and calls
void MfReportError(MfString actorName, int code, ...)
{
    MfString text;
    int column = 0;
    int pending = 0;
    MfString token;
    if (MfErrorText::sReportLevel < MfErrorTable::sMessages[code].mLevel)
        return;
    va_list args;
    va_start(args, code);
    MfDblListItem *pli;
    MfString *part = (MfString *)MfErrorTable::sMessages[code].mTokens.GetFirst(pli);
    while (part) {
        if (pending && *part != "%underline_line") {
            pending = 0;
            column = 0;
        }
        if (*part == "%s") {
            token = va_arg(args, MfString);
            column += token.Length();
            text << token;
        } else if (*part == "%d") {
            token = MfString(va_arg(args, int));
            column += token.Length();
            text << token;
        } else if (*part == "%f") {
            token = MfString(va_arg(args, float));
            column += token.Length();
            text << token;
        } else if (*part == "%underline_line") {
            token = NULL_STRING;
            float width = column * 1.75f;
            for (int i = 0; i < width; i++)
                token << MfString("-");
            token << MfString("\n");
            text << token;
            column = 0;
            pending = 0;
        } else if (*part == "%state_position") {
            token = MfString("state1:state2:state3");
            column += token.Length();
            text << token;
        } else if (!(*part == "%break")) {
            int isTable = part->LeftString(1) == "%";
            if (isTable) {
                int index = va_arg(args, int);
                token = MfErrorText::sNames[FindName(&MfErrorText::sTables, *part)][index];
                column += token.Length();
                text << token;
            } else {
                bool newline = part->MidString(part->Length() - 1, 1) == "n" &&
                               part->MidString(part->Length() - 2, 1) == "\\";
                if (newline) {
                    *part = part->LeftString(part->Length() - 2) + MfString("\n");
                    pending = 1;
                }
                column += part->Length();
                text << *part;
            }
        }
        part = (MfString *)MfErrorTable::sMessages[code].mTokens.GetNext(pli);
    }
    MfLogger::Log(MfErrorTable::sMessages[code].mCategory, actorName, MfErrorTable::sMessages[code].mTitle, text);
}

// 0x10020cb0 ?FindName@@YAHPAVMfStringList@@VMfString@@@Z
// The index of a name in the list (the count if it is not there).
int FindName(MfStringList *list, MfString name)
{
    int index = 0;
    MfDblListItem *pli;
    for (MfString *s = (MfString *)list->GetFirst(pli); s; s = (MfString *)list->GetNext(pli)) {
        if (*s == name)
            break;
        index++;
    }
    return index;
}
