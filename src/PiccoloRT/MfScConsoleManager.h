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
    int field_4;
};
