// PORT: the member is pointer-sized like the real one (MfScConsoleManager_unit.h's m_outputs), so operator= copies the whole pointer on 64-bit.
#pragma once
class MfAiConsoleOutput;
class MfScDataHandle;
class MfString;

class MfScConsoleManager {
public:
    // tag of the constructor that installs the manager
    class Exemplar {
    public:
        ~Exemplar() {}
    };
    virtual ~MfScConsoleManager();
    virtual void _slot1();
    MfScConsoleManager & operator=(MfScConsoleManager const &);
    MfScConsoleManager();
    MfScConsoleManager(MfScConsoleManager const &);
    MfScConsoleManager(Exemplar);
    static MfScConsoleManager * __cdecl Manager();
    void AddOutput(MfAiConsoleOutput *);
    void ClearConsole();
    void PrintString(MfString const &);
    void PrintStringNl(MfString const &);
    void RemoveAllOutputs();
    void RemoveOutput(MfAiConsoleOutput *);
private:
    friend class MfScPiccolo;
    static MfScDataHandle * __cdecl Console_Clear(MfScDataHandle *, int, MfScDataHandle * *);
    static MfScDataHandle * __cdecl Console_Print(MfScDataHandle *, int, MfScDataHandle * *);
    static MfScDataHandle * __cdecl Console_PrintNl(MfScDataHandle *, int, MfScDataHandle * *);
    static MfScConsoleManager * pcmManager;
public:
    void *field_4; // PORT: a pointer like the real member (the output list, MfDblList *); operator= copied an int
};
