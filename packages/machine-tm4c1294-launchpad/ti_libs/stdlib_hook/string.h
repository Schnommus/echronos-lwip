#ifndef STRING_H
#define STRING_H

#include "utils/ustdlib.h"
#include <stddef.h>

inline static size_t strlen(const char *s) { return ustrlen( s ); }

inline static int strncmp(const char *s1, const char *s2, size_t n) { return ustrncmp( s1, s2, n ); }

inline static int strcmp(const char *s1, const char *s2) { return ustrcmp( s1, s2 ); } 

inline static char *strstr(const char *s1, const char *s2) { return ustrstr( s1, s2 ); }

inline static void *memset ( void * ptr, int value, size_t num ) { return umemset( ptr, value, num ); }

inline static void *memcpy ( void * destination, const void * source, size_t num ) { return umemcpy( destination, source, num ); }

inline static int memcmp(const void * s1, const void * s2,size_t n) { return umemcmp( s1, s2, n ); }

inline static char *strchr ( const char *s, int c ) { return ustrchr( s, c ); }

inline static char *strcat (char *dest, const char *src) { return ustrcat( dest, src ); }

#endif
