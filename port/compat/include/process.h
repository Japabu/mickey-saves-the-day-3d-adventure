// process.h - threads and processes of the Microsoft C runtime (see port_crt.h for
// _beginthread and _endthread).
#ifndef PORT_PROCESS_H
#define PORT_PROCESS_H
#include <stdlib.h>
#include <unistd.h>
#define _getpid getpid
#endif
