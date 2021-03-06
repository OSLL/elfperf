/* Definitions for thread-local data handling.  linuxthreads/x86-64 version.
   Copyright (C) 2002, 2005 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _TLS_H
#define _TLS_H

#ifndef __ASSEMBLER__

# include <pt-machine.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

/* Type for the dtv.  */
typedef union dtv
{
  size_t counter;
  struct
  {
    void *val;
    bool is_static;
  } pointer;
} dtv_t;


typedef struct
{
  void *tcb;		/* Pointer to the TCB.  Not necessary the
			   thread descriptor used by libpthread.  */
  dtv_t *dtv;
  void *self;		/* Pointer to the thread descriptor.  */
  int multiple_threads;
  uintptr_t sysinfo;
  uintptr_t stack_guard;
  uintptr_t pointer_guard;
# ifdef __FreeBSD_kernel__
  long gscope_flag;
# else
  int gscope_flag;
# endif
} tcbhead_t;

#else /* __ASSEMBLER__ */
# include <tcb-offsets.h>
#endif


#ifdef HAVE_TLS_SUPPORT

/* Signal that TLS support is available.  */
# define USE_TLS	1

# ifndef __ASSEMBLER__
/* Get system call information.  */
#  include <sysdep.h>

/* Get the thread descriptor definition.  */
#  include <linuxthreads/descr.h>

/* This is the size of the initial TCB.  */
#  define TLS_INIT_TCB_SIZE sizeof (tcbhead_t)

/* Alignment requirements for the initial TCB.  */
#  define TLS_INIT_TCB_ALIGN __alignof__ (tcbhead_t)

/* This is the size of the TCB.  */
#  define TLS_TCB_SIZE sizeof (struct _pthread_descr_struct)

/* Alignment requirements for the TCB.  */
#  define TLS_TCB_ALIGN __alignof__ (struct _pthread_descr_struct)

/* The TCB can have any size and the memory following the address the
   thread pointer points to is unspecified.  Allocate the TCB there.  */
#  define TLS_TCB_AT_TP	1


/* Install the dtv pointer.  The pointer passed is to the element with
   index -1 which contain the length.  */
#  define INSTALL_DTV(descr, dtvp) \
  ((tcbhead_t *) (descr))->dtv = (dtvp) + 1

/* Install new dtv for current thread.  */
#  define INSTALL_NEW_DTV(dtv) \
  ({ struct _pthread_descr_struct *__descr;				      \
     THREAD_SETMEM (__descr, p_header.data.dtvp, (dtv)); })

/* Return dtv of given thread descriptor.  */
#  define GET_DTV(descr) \
  (((tcbhead_t *) (descr))->dtv)

/* Code to initially initialize the thread pointer.  This might need
   special attention since 'errno' is not yet available and if the
   operation can cause a failure 'errno' must not be touched.  */
# define TLS_INIT_TP(descr, secondcall)					      \
  ({									      \
    void *_descr = (descr);						      \
    tcbhead_t *head = _descr;						      \
    long int _result;							      \
									      \
    head->tcb = _descr;							      \
    /* For now the thread descriptor is at the same address.  */	      \
    head->self = _descr;						      \
									      \
    asm volatile ("syscall"						      \
		  : "=a" (_result)					      \
		  : "0" ((unsigned long int) __NR_arch_prctl),		      \
		    "D" ((unsigned long int) ARCH_SET_FS),		      \
		    "S" (_descr)					      \
		  : "memory", "cc", "r11", "cx");			      \
									      \
    _result ? "cannot set %fs base address for thread-local storage" : 0;     \
  })

/* Indicate that dynamic linker shouldn't try to initialize TLS even
   when no PT_TLS segments are found in the program and libraries
   it is linked against.  */
#  define TLS_INIT_TP_EXPENSIVE 1

/* Return the address of the dtv for the current thread.  */
#  define THREAD_DTV() \
  ({ struct _pthread_descr_struct *__descr;				      \
     THREAD_GETMEM (__descr, p_header.data.dtvp); })

/* Set the stack guard field in TCB head.  */
# define THREAD_SET_STACK_GUARD(value) \
    THREAD_SETMEM (THREAD_SELF, p_header.data.stack_guard, value)
# define THREAD_COPY_STACK_GUARD(descr) \
    ((descr)->p_header.data.stack_guard				      \
     = THREAD_GETMEM (THREAD_SELF, p_header.data.stack_guard))


/* Set the pointer guard field in the TCB head.  */
#define THREAD_SET_POINTER_GUARD(value) \
  THREAD_SETMEM (THREAD_SELF, p_header.data.pointer_guard, value)
#define THREAD_COPY_POINTER_GUARD(descr) \
  ((descr)->p_header.data.pointer_guard				      \
   = THREAD_GETMEM (THREAD_SELF, p_header.data.pointer_guard))

/* Get and set the global scope generation counter in the TCB head.  */
#define THREAD_GSCOPE_FLAG_UNUSED 0
#define THREAD_GSCOPE_FLAG_USED   1
#define THREAD_GSCOPE_FLAG_WAIT   2
#ifdef __FreeBSD_kernel__
# define THREAD_GSCOPE_RESET_FLAG() \
  do										    \
    { int __res;								    \
      asm volatile ("xchgl %0, %%fs:%P1"					    \
		    : "=r" (__res)						    \
		    : "i" (offsetof (struct _pthread_descr_struct, p_gscope_flag)), \
		      "0" (THREAD_GSCOPE_FLAG_UNUSED));				    \
      if (__res == THREAD_GSCOPE_FLAG_WAIT)					    \
	lll_futex_wake (&THREAD_SELF->p_gscope_flag, 1);			    \
    }										    \
  while (0)
#else
/* As the FreeBSD kernel defines futex as long (compared to int with
   a Linux kernel), we need to use xchgq instead of xchgl to handle
   the gscope_flag variable. */
# define THREAD_GSCOPE_RESET_FLAG() \
  do										    \
    { long __res;								    \
      asm volatile ("xchgq %0, %%fs:%P1"					    \
		    : "=r" (__res)						    \
		    : "i" (offsetof (struct _pthread_descr_struct, p_gscope_flag)), \
		      "0" (THREAD_GSCOPE_FLAG_UNUSED));				    \
      if (__res == THREAD_GSCOPE_FLAG_WAIT)					    \
	lll_futex_wake (&THREAD_SELF->p_gscope_flag, 1);			    \
    }										    \
  while (0)
#endif		/* __FreeBSD_kernel__ */
#define THREAD_GSCOPE_SET_FLAG() \
  THREAD_SETMEM (THREAD_SELF, p_gscope_flag, THREAD_GSCOPE_FLAG_USED)
#define THREAD_GSCOPE_WAIT() \
  do { /* GL(dl_wait_lookup_done) () */ } while (0)

# endif	/* HAVE_TLS_SUPPORT */
#endif /* __ASSEMBLER__ */

#endif	/* tls.h */
