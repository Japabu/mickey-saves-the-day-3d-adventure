# mfutil edits for the port (see port/rewrite.py, EDITS).
#
# The logger shows its messages in a list view window, which the port has no control
# for, and writes them to steamboat.log only from its window thread. Each message is
# also passed to OutputDebugString when it is logged (on the line that queues it), so
# script errors and Logger.Log lines reach the port's log (PORT_LOG=1) in order with
# everything else.
_ECHO = ('    csQueueLock.Enter();\n    plLogQueue->Add(pMessage);',
         '    { MfString fsLine = MfString("[MfLogger] ") + fsSource + MfString(": ") + fsMessage + MfString("\\n");'
         ' OutputDebugString(fsLine); MfDblListItem *pliExtra;'
         ' for (MfString *pfs = pMessage->lstExtra.First(pliExtra); pfs; pfs = pMessage->lstExtra.Next(pliExtra))'
         ' { OutputDebugString("    "); OutputDebugString(*pfs); OutputDebugString("\\n"); } }'
         ' csQueueLock.Enter();\n    plLogQueue->Add(pMessage);', 3)

# "%#m" (no leading zero) is Visual C++'s strftime; the C library prints it literally.
_TIME = [('"%#m/%#d/%y %#I:%M:%S %p"', '"%m/%d/%y %I:%M:%S %p"', 1),
         ('"%#m/%#d/%y"', '"%m/%d/%y"', 2),
         ('"%#I:%M:%S %p"', '"%I:%M:%S %p"', 2)]

EDITS = {
    'mfutil/MfLogger.cpp': [_ECHO] + _TIME,
}
