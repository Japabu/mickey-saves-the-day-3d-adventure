// The tokens and the semantic values shared by the scanner (MfStateLexer.cpp)
// and the parser (MfStateParser.cpp) of behaviour files.
#ifndef RUNTIMELIB_MFSTATEGRAMMAR_H
#define RUNTIMELIB_MFSTATEGRAMMAR_H
class MfString;
class MfStringList;
class MfFSMList;
class MfFiniteStateMachine;
class MfStateProperty;
class MfStatePropertyList;
class MfTransitionProperty;
class MfTransPropList;
class Mf2DVectorList;
struct Mf2DVector;

// the values of the tokens and the grammar symbols (yylval, yyval)
union YYSTYPE {
    int ival;
    double dval;
    MfString *sval;
    MfFSMList *machines;
    MfFiniteStateMachine *machine;
    MfStateProperty *prop;
    MfStatePropertyList *props;
    MfStringList *names;
    MfTransitionProperty *trans;
    MfTransPropList *transProps;
    Mf2DVector *point;
    Mf2DVectorList *points;
};
extern YYSTYPE yylval;

// the tokens
enum {
    TOK_IDENTIFIER = 257, TOK_STRING, TOK_INTEGER, TOK_FLOAT, TOK_CODE,
    TOK_LBRACE = 265, TOK_RBRACE, TOK_COMMA, TOK_HASH,
    TOK_STATE, TOK_VARS, TOK_ENTRY_ACTION, TOK_EXIT_ACTION, TOK_TRANSITION, TOK_DEST_STATE,
    TOK_TRIG_BY, TOK_TRIG_ACTION, TOK_TRIG_GUARD, TOK_DISPLAY_POS, TOK_VISUAL_CON, TOK_FUNCTIONS,
    TOK_FUNC_DEF, TOK_MF_HFSM, TOK_START_STATE, TOK_EXIT_PORT, TOK_ENTRY_PORT, TOK_INIT,
    TOK_EXITBY, TOK_ENTERBY, TOK_CAMERA_SETTING, TOK_INIT_VAR, TOK_BEGIN_HIST, TOK_PLAYBACK,
    TOK_NOTE, TOK_NOTE_CONTENTS, TOK_CHOICE_PT, TOK_DEST_CHOICE_PT, TOK_TRANS_TYPE
};

#endif
