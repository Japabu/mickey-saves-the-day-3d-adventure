// malloc.h - the Microsoft heap header: malloc and friends, _alloca, _msize.
#ifndef PORT_MALLOC_H
#define PORT_MALLOC_H
#include <stdlib.h>
#include <alloca.h>
#include <malloc/malloc.h>
#define _msize(p) malloc_size(p)
#endif
