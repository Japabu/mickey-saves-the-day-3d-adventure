// eh.h - Microsoft's exception handling hooks. There are no structured (SEH)
// exceptions on macOS, so a translator is stored and never called.
#ifndef PORT_EH_H
#define PORT_EH_H
#include <exception>
struct _EXCEPTION_POINTERS;
typedef void (*_se_translator_function)(unsigned int, struct _EXCEPTION_POINTERS *);
_se_translator_function _set_se_translator(_se_translator_function f);
using std::terminate;
using std::set_terminate;
typedef void (*terminate_function)();
#endif
