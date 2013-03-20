/* s_finitel.c -- long double version of s_finite.c.
 * Conversion to IEEE quad long double by Jakub Jelinek, jj@ultra.linux.cz.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#if defined(LIBM_SCCS) && !defined(lint)
static char rcsid[] = "$NetBSD: $";
#endif

/*
 * finitel(x) returns 1 is x is finite, else 0;
 * no branching!
 */

#include <math.h>
#include <math_private.h>
#include <math_ldbl_opt.h>

int
___finitel (long double x)
{
	int64_t hx;
	GET_LDOUBLE_MSW64(hx,x);
	return (int)((u_int64_t)((hx&0x7fffffffffffffffLL)
				 -0x7ff0000000000000LL)>>63);
}
hidden_ver (___finitel, __finitel)
weak_alias (___finitel, ____finitel)
#ifdef IS_IN_libm
long_double_symbol (libm, ____finitel, finitel);
long_double_symbol (libm, ___finitel, __finitel);
#else
long_double_symbol (libc, ____finitel, finitel);
long_double_symbol (libc, ___finitel, __finitel);
#endif
