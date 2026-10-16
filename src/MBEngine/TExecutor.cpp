// MBEngine.dll exec.obj: TExecutor, the virtual machine that runs a compiled script.
// The code block starts with the compiler version and the offset of the symbol
// table; the opcodes (vm*) run until the symbol table or a wait.
#include "CMBScript.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" __declspec(dllimport) int MBstricmp(const char *, const char *);

void (__cdecl *TExecutor::handlers[73])(TExecutor *) = {
    BadOpcodeHandler, VMNumber, VMAddress, VMSymStr, VMSymNum, VMPop, BadOpcodeHandler,
    BadOpcodeHandler, VMJumpIfZero, VMJump, VMPrintNum, VMPrintStr, BadOpcodeHandler,
    VMNumAssign, VMAdd, VMSub, VMMul, VMDiv, VMMod, VMNeg, VMExp, VMRand, VMSin, VMCos,
    VMSqrt, VMBoolEq, VMBoolNe, VMBoolOr, VMBoolAnd, VMBoolNot, VMBoolGt, VMBoolGe,
    VMBoolLt, VMBoolLe, BadOpcodeHandler, BadOpcodeHandler, VMStrAssign, VMStrAppend,
    VMNumToStr, VMStrCmp, VMStrICmp, BadOpcodeHandler, BadOpcodeHandler,
    VMStartActorOnPathAtPoint, VMWaitForActor, VMStopActor, VMCopyActor, VMMoveActor,
    VMPlaceActor, BadOpcodeHandler, BadOpcodeHandler, VMStartMovie, VMWaitForMovie,
    VMStopMovie, VMStartSound, VMWaitForSound, VMStopSound, VMStopSoundChannel,
    VMStopAllSound, BadOpcodeHandler, BadOpcodeHandler, VMStartScript, VMWaitForScript,
    VMStopScript, VMStopAllScripts, VMBackgroundScript, VMNotify, VMNotifyLine,
    VMWaitNMilliseconds, VMGotoScene, VMPauseEverything, BadOpcodeHandler,
    BadOpcodeHandler
};

// TODO 0x100361e0 ??0TExecutor@@QAE@PAVCMBScript@@PAD@Z: original re-reads cursor and code from the object after every member store (ours keeps codeBlock in a register); cursor = code and helper forms tried (78% similar)
TExecutor::TExecutor(CMBScript *obj, char *codeBlock)
{
    code = codeBlock;
    cursor = code;
    ok = 0;
    paused = 1;
    imp = obj;
    arr = 0;
    nodeCnt = 0;
    long version = *(long *)cursor;
    cursor += 4;
    if (version != vmCompilerVersion) {
        MBDebugStr("Script executor error: executor version %ld, script compiled with compiler version %ld\n",
                   vmCompilerVersion, version);
        return;
    }
    long symtabOffset = *(long *)cursor;
    cursor += 4;
    codeEnd = code + symtabOffset;
    cursor = codeEnd;
    nodeCnt = *(long *)cursor;
    cursor += 4;
    arr = new TSymtabNode *[nodeCnt];
    for (int i = 0; i < nodeCnt; i++) {
        TSymtabNode *node = new TSymtabNode;
        long size = node->StreamIn(cursor);
        arr[node->Index()] = node;
        if (size == -1) {
            MBDebugStr("Script executor error: error loading symbol table\n");
            return;
        }
        cursor += size;
    }
    ok = 1;
    cursor = code + 8;
}

// 0x100362f0 ??1TExecutor@@QAE@XZ
TExecutor::~TExecutor(void)
{
    for (int i = 0; i < nodeCnt; i++)
        delete arr[i];
    delete [] arr;
}

// EQUIV 0x10036340 ?Go@TExecutor@@QAEXXZ: reviewed: the code pointer is kept in a register instead of being reloaded; same dispatch
void TExecutor::Go(void)
{
    if (!ok)
        return;
    paused = 0;
    while (cursor < codeEnd && !paused) {
        short opcode = *(short *)cursor;
        cursor += 2;
        if (opcode >= vmFirstOp && opcode <= vmLastOp)
            handlers[opcode](this);
        else
            BadOpcodeHandler(this);
    }
    if (cursor >= codeEnd)
        ok = 0;
}

// 0x100363b0 ?Search@TExecutor@@QAEPAVTSymtabNode@@PBD@Z
TSymtabNode *TExecutor::Search(const char *varname)
{
    for (int i = 0; i < nodeCnt; i++) {
        if (strcmp(varname, arr[i]->Name()) == 0)
            return arr[i];
    }
    return 0;
}

// 0x10036420 ?SetScriptVariable@TExecutor@@QAE_NPBD0@Z
bool TExecutor::SetScriptVariable(const char *varname, const char *value)
{
    TSymtabNode *node = Search(varname);
    if (!node)
        return false;
    node->SetValue(value);
    return true;
}

// 0x10036450 ?SetScriptVariable@TExecutor@@QAE_NPBDN@Z
bool TExecutor::SetScriptVariable(const char *varname, double value)
{
    TSymtabNode *node = Search(varname);
    if (!node)
        return false;
    node->SetValue((float)value);
    return true;
}

// 0x10036480 ?GetScriptVariable@TExecutor@@QAE_NPBDPAPBD@Z
bool TExecutor::GetScriptVariable(const char *varname, const char **valueDest)
{
    TSymtabNode *node = Search(varname);
    if (!node)
        return false;
    *valueDest = node->String();
    return true;
}

// 0x100364c0 ?GetScriptVariable@TExecutor@@QAE_NPBDPAN@Z
bool TExecutor::GetScriptVariable(const char *varname, double *valueDest)
{
    TSymtabNode *node = Search(varname);
    if (!node)
        return false;
    *valueDest = node->value.num;
    return true;
}

// 0x100364f0 ?BadOpcodeHandler@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::BadOpcodeHandler(TExecutor *This)
{
    short opcode = *(short *)This->cursor;
    This->cursor += 2;
    MBDebugStr("Bad error: unknown opcode %ld in script %s.\n", (long)opcode);
    This->cursor = This->codeEnd;
}

// TODO 0x10036520 ?VMNumber@TExecutor@@SAXPAV1@@Z: a double temporary gives the fld/fstp load, but the fstp is scheduled after lea ecx,stack in ours (before it in the original); float/double/long double/pointer forms tried (67% similar)
void __cdecl TExecutor::VMNumber(TExecutor *This)
{
    float number = *(float *)This->cursor;
    This->cursor += 4;
    This->stack.Push(number * 1.0f);
}

// 0x10036540 ?VMAddress@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMAddress(TExecutor *This)
{
    short addr = *(short *)This->cursor;
    This->cursor += 2;
    This->stack.Push(addr);
}

// 0x10036560 ?VMSymNum@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMSymNum(TExecutor *This)
{
    short addr = *(short *)This->cursor;
    This->cursor += 2;
    This->stack.Push(This->arr[addr]->value.num);
}

// 0x10036590 ?VMSymStr@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMSymStr(TExecutor *This)
{
    short addr = *(short *)This->cursor;
    This->cursor += 2;
    This->stack.Push(addr);
}

// 0x100365b0 ?VMPop@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMPop(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
}

// 0x100365d0 ?VMPrintNum@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMPrintNum(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
}

// 0x100365f0 ?VMPrintStr@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMPrintStr(TExecutor *This)
{
    short symRef;
    This->stack.Pop(&symRef);
}

// 0x10036610 ?VMJumpIfZero@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMJumpIfZero(TExecutor *This)
{
    short addr;
    float d1;
    This->stack.Pop(&addr);
    This->stack.Pop(&d1);
    if (d1 == 0)
        This->cursor = This->code + addr;
}

// 0x10036660 ?VMJump@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMJump(TExecutor *This)
{
    short symRef;
    This->stack.Pop(&symRef);
    This->cursor = This->code + symRef;
}

// 0x10036680 ?VMNumAssign@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMNumAssign(TExecutor *This)
{
    short symRef;
    float d1;
    This->stack.Pop(&symRef);
    This->stack.Pop(&d1);
    This->arr[symRef]->SetValue(d1);
    This->stack.Push(d1);
}

// 0x100366d0 ?VMAdd@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMAdd(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push(d1 + d2);
}

// 0x10036710 ?VMSub@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMSub(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push(d1 - d2);
}

// 0x10036750 ?VMMul@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMMul(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push(d1 * d2);
}

// 0x10036790 ?VMDiv@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMDiv(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push(d1 / d2);
}

// 0x100367d0 ?VMMod@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMMod(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)((long)d1 % (long)d2));
}

// 0x10036830 ?VMNeg@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMNeg(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->stack.Push(-d1);
}

// 0x10036860 ?VMExp@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMExp(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)pow(d1, d2));
}

// 0x100368a0 ?VMRand@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMRand(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    if ((long)d1 == 0)
        This->stack.Push(0.0f);
    else
        This->stack.Push((float)(rand() % (long)d1) + 1);
}

// 0x10036910 ?VMSin@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMSin(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->stack.Push((float)sin(d1));
}

// 0x10036940 ?VMCos@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMCos(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->stack.Push((float)cos(d1));
}

// 0x10036970 ?VMSqrt@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMSqrt(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->stack.Push((float)sqrt(d1));
}

// 0x100369a0 ?VMBoolEq@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolEq(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 == d2 ? 1 : 0));
}

// 0x10036a00 ?VMBoolNe@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolNe(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 != d2 ? 1 : 0));
}

// 0x10036a60 ?VMBoolOr@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolOr(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 != 0 || d2 != 0 ? 1 : 0));
}

// 0x10036ad0 ?VMBoolAnd@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolAnd(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 != 0 && d2 != 0 ? 1 : 0));
}

// 0x10036b40 ?VMBoolNot@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolNot(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 == 0 ? 1 : 0));
}

// 0x10036b90 ?VMBoolGt@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolGt(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 > d2 ? 1 : 0));
}

// 0x10036bf0 ?VMBoolGe@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolGe(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 >= d2 ? 1 : 0));
}

// 0x10036c50 ?VMBoolLt@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolLt(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 < d2 ? 1 : 0));
}

// 0x10036cb0 ?VMBoolLe@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBoolLe(TExecutor *This)
{
    float d2;
    float d1;
    This->stack.Pop(&d2);
    This->stack.Pop(&d1);
    This->stack.Push((float)(d1 <= d2 ? 1 : 0));
}

// EQUIV 0x10036d10 ?VMStrAssign@TExecutor@@SAXPAV1@@Z: reviewed: the first popped operand is kept in a register instead of re-read from its slot; same calls and arguments
void __cdecl TExecutor::VMStrAssign(TExecutor *This)
{
    short s2;
    short s1;
    This->stack.Pop(&s1);
    This->stack.Pop(&s2);
    This->arr[s1]->SetValue(This->arr[s2]->String());
    This->stack.Push(s1);
}

// EQUIV 0x10036d70 ?VMStrAppend@TExecutor@@SAXPAV1@@Z: reviewed: stack slots and the save of edi only; same pops, copies and calls
void __cdecl TExecutor::VMStrAppend(TExecutor *This)
{
    short s3;
    short s2;
    short s1;
    char buffer[32];
    This->stack.Pop(&s3);
    This->stack.Pop(&s2);
    This->stack.Pop(&s1);
    buffer[31] = 0;
    strncpy(buffer, This->arr[s1]->String(), 31);
    int len = strlen(buffer);
    if (len < 31)
        strncpy(buffer + len, This->arr[s2]->String(), 31 - len);
    This->arr[s3]->SetValue(buffer);
    This->stack.Push(s3);
}

// EQUIV 0x10036e40 ?VMNumToStr@TExecutor@@SAXPAV1@@Z: reviewed: stack slots and registers only (which pop goes to the frame and which to the dead parameter slot)
void __cdecl TExecutor::VMNumToStr(TExecutor *This)
{
    short s1;
    float d1;
    char buffer[32];
    This->stack.Pop(&s1);
    This->stack.Pop(&d1);
    buffer[31] = 0;
    sprintf(buffer, "%-*f", 31, d1);
    char *p = buffer + 30;
    while (*p == '0' || *p == ' ') {
        *p = 0;
        p--;
    }
    if (*p == '.')
        *p = 0;
    This->arr[s1]->SetValue(buffer);
    This->stack.Push(s1);
}

// EQUIV 0x10036ed0 ?VMStrCmp@TExecutor@@SAXPAV1@@Z: reviewed: stack slots only (the original pops into its parameter slot, ours into a local)
void __cdecl TExecutor::VMStrCmp(TExecutor *This)
{
    short s2;
    short s1;
    This->stack.Pop(&s2);
    This->stack.Pop(&s1);
    This->stack.Push((float)strcmp(This->arr[s1]->String(), This->arr[s2]->String()));
}

// EQUIV 0x10036f70 ?VMStrICmp@TExecutor@@SAXPAV1@@Z: reviewed: stack slots only (the original pops into its parameter slot, ours into a local)
void __cdecl TExecutor::VMStrICmp(TExecutor *This)
{
    short s1;
    short s2;
    This->stack.Pop(&s2);
    This->stack.Pop(&s1);
    This->stack.Push((float)MBstricmp(This->arr[s1]->String(), This->arr[s2]->String()));
}

// 0x10036ff0 ?VMStartActorOnPathAtPoint@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStartActorOnPathAtPoint(TExecutor *This)
{
    float y;
    float x;
    float path;
    short actor;
    float wait;
    This->stack.Pop(&y);
    This->stack.Pop(&x);
    This->stack.Pop(&path);
    This->stack.Pop(&actor);
    This->stack.Pop(&wait);
    This->imp->StartActor(This->arr[actor], (long)wait, (long)path, (long)x, (long)y);
}

// 0x10037080 ?VMWaitForActor@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMWaitForActor(TExecutor *This)
{
    float path;
    short actor;
    This->stack.Pop(&path);
    This->stack.Pop(&actor);
    This->imp->WaitForActor(This->arr[actor], (long)path);
}

// 0x100370d0 ?VMStopActor@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStopActor(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->StopActor(This->arr[s1]);
}

// 0x10037100 ?VMCopyActor@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMCopyActor(TExecutor *This)
{
    float zplane;
    float y;
    float x;
    short newName;
    short actor;
    This->stack.Pop(&zplane);
    This->stack.Pop(&y);
    This->stack.Pop(&x);
    This->stack.Pop(&newName);
    This->stack.Pop(&actor);
    This->imp->CopyActor(This->arr[actor], This->arr[newName], (long)x, (long)y, (long)zplane);
}

// 0x10037190 ?VMMoveActor@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMMoveActor(TExecutor *This)
{
    float z;
    float y;
    float x;
    short actor;
    This->stack.Pop(&z);
    This->stack.Pop(&y);
    This->stack.Pop(&x);
    This->stack.Pop(&actor);
    This->imp->MoveActor(This->arr[actor], x, y, z);
}

// 0x10037200 ?VMPlaceActor@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMPlaceActor(TExecutor *This)
{
    float z;
    float y;
    float x;
    short actor;
    This->stack.Pop(&z);
    This->stack.Pop(&y);
    This->stack.Pop(&x);
    This->stack.Pop(&actor);
    This->imp->PlaceActor(This->arr[actor], x, y, z);
}

// 0x10037270 ?VMStartMovie@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStartMovie(TExecutor *This)
{
    float z;
    float y;
    float x;
    float flags;
    short movie;
    float wait;
    This->stack.Pop(&z);
    This->stack.Pop(&y);
    This->stack.Pop(&x);
    This->stack.Pop(&flags);
    This->stack.Pop(&movie);
    This->stack.Pop(&wait);
    This->imp->StartMovie(This->arr[movie], (long)wait, (long)flags, (long)x, (long)y, (long)z);
}

// 0x10037320 ?VMWaitForMovie@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMWaitForMovie(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->WaitForMovie(This->arr[s1]);
}

// 0x10037350 ?VMStopMovie@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStopMovie(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->StopMovie(This->arr[s1]);
}

// 0x10037380 ?VMStartSound@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStartSound(TExecutor *This)
{
    float force;
    float queue;
    float volume;
    float channel;
    short sound;
    float loop;
    float wait;
    This->stack.Pop(&force);
    This->stack.Pop(&queue);
    This->stack.Pop(&volume);
    This->stack.Pop(&channel);
    This->stack.Pop(&sound);
    This->stack.Pop(&loop);
    This->stack.Pop(&wait);
    This->imp->StartSound(This->arr[sound], (long)wait, (long)channel, (long)volume, (long)loop, (long)queue,
                          (long)force);
}

// 0x10037440 ?VMWaitForSound@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMWaitForSound(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->WaitForSound(This->arr[s1]);
}

// 0x10037470 ?VMStopSound@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStopSound(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->StopSound(This->arr[s1]);
}

// 0x100374a0 ?VMStopSoundChannel@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStopSoundChannel(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->imp->StopSound((long)d1);
}

// 0x100374d0 ?VMStopAllSound@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStopAllSound(TExecutor *This)
{
    This->imp->StopAllSound();
}

// 0x100374e0 ?VMStartScript@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStartScript(TExecutor *This)
{
    short script;
    float wait;
    This->stack.Pop(&script);
    This->stack.Pop(&wait);
    This->imp->StartScript(This->arr[script], (long)wait);
}

// 0x10037530 ?VMWaitForScript@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMWaitForScript(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->WaitForScript(This->arr[s1]);
}

// 0x10037560 ?VMStopScript@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStopScript(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->StopScript(This->arr[s1]);
}

// 0x10037590 ?VMStopAllScripts@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMStopAllScripts(TExecutor *This)
{
    This->imp->StopAllScripts();
}

// 0x100375a0 ?VMBackgroundScript@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMBackgroundScript(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->imp->BackgroundScript((long)d1);
}

// 0x100375d0 ?VMNotify@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMNotify(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->Notify(This->arr[s1]);
}

// 0x10037600 ?VMNotifyLine@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMNotifyLine(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->imp->Notify((long)d1);
}

// 0x10037630 ?VMWaitNMilliseconds@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMWaitNMilliseconds(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->imp->WaitNMilliseconds((long)d1);
}

// 0x10037660 ?VMGotoScene@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMGotoScene(TExecutor *This)
{
    short s1;
    This->stack.Pop(&s1);
    This->imp->GotoScene(This->arr[s1]);
}

// 0x10037690 ?VMPauseEverything@TExecutor@@SAXPAV1@@Z
void __cdecl TExecutor::VMPauseEverything(TExecutor *This)
{
    float d1;
    This->stack.Pop(&d1);
    This->imp->PauseEverything((long)d1);
}

// Matched by this code (static initializers):
// 0x100376c0 _$E77
// 0x100376e0 _$E76
// 0x100376f0 _$E22
// 0x10037700 _$E21
// 0x100377d0 _$E25
// 0x100377e0 _$E24
// 0x100378b0 _$E28
// 0x100378c0 _$E27
// 0x10037990 _$E31
// 0x100379a0 _$E30
// 0x10037a60 _$E34
// 0x10037a70 _$E33
// 0x10037b30 _$E37
// 0x10037b40 _$E36
// 0x10037bf0 _$E40
// 0x10037c00 _$E39
// 0x10037cb0 _$E43
// 0x10037cc0 _$E42
// 0x10037d70 _$E46
// 0x10037d80 _$E45
// 0x10037e40 _$E49
// 0x10037e50 _$E48
// 0x10037f10 _$E52
// 0x10037f20 _$E51
// 0x10037fd0 _$E55
// 0x10037fe0 _$E54
// 0x10038090 _$E58
// 0x100380a0 _$E57
// 0x10038150 _$E61
// 0x10038160 _$E60
// 0x10038210 _$E64
// 0x10038220 _$E63
// 0x100382d0 _$E67
// 0x100382e0 _$E66
