#ifndef _CORE_COMMON_H
#define _CORE_COMMON_H

#define HEAP_ALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define HEAP_FREE(x)  HeapFree(GetProcessHeap(), 0, (x))

#endif 
