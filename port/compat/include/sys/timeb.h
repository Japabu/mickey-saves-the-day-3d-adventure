// sys/timeb.h - the system header plus Microsoft's underscore names (_timeb, _ftime).
#ifndef PORT_SYS_TIMEB_H
#define PORT_SYS_TIMEB_H
#include_next <sys/timeb.h>
#define _timeb timeb
static inline void _ftime(struct timeb *t) { ftime(t); }
#endif
