#pragma once
#include_next <assert.h>

#undef assert

#ifdef NDEBUG
# define assert(e) ((void)0)
#else
#ifndef __FILENAME__
# define __FILENAME__ __FILE__
#endif

extern void __assert_func(const char* file, int line, const char* func, const char* failedexpr);

# define assert(e) ((e) ? (void)0 : __assert_func(__FILENAME__, __LINE__, __func__, #e))
#endif
