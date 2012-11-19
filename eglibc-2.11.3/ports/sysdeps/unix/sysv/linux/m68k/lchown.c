#include <sysdeps/unix/sysv/linux/i386/lchown.c>

#include <shlib-compat.h>
#if SHLIB_COMPAT (libc, GLIBC_2_1, GLIBC_2_2)
compat_symbol (libc, __lchown, lchown, GLIBC_2_1);
#endif
