// MBScriptEvent.h - the data of a script event.
#ifndef MBENGINE_MBSCRIPTEVENT_H
#define MBENGINE_MBSCRIPTEVENT_H

#include "MBPointStruct.h"

class IMBScript;

struct MBScriptEvent {
    IMBScript *script;
    unsigned long scriptID;
    union MBScriptEventParam {
        struct MBScriptEventStartAnim {
            unsigned long id;
            unsigned long seq;
            MBPointStruct loc;
        } startAnim;
        struct MBScriptEventNotifyMsg {
            const char *msg;
        } notifyMsg;
        struct MBScriptEventNotifyLine {
            unsigned long line;
        } notifyLine;
    } param;
};

#endif
