/* Look up a symbol in a shared object loaded by `dlopen'.
   Copyright (C) 1995-2000, 2004 Free Software Foundation, Inc.
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

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <ldsodefs.h>

static void test(int t)
{
  printf("Hello from loader %d\n",t);
  asm("int3");
}


#if !defined SHARED && defined IS_IN_libdl



void *
dlsym (void *handle, const char *name)
{
  test(1);
  return __dlsym (handle, name, RETURN_ADDRESS (0));
}

#else

struct dlsym_args
{
  /* The arguments to dlsym_doit.  */
  void *handle;
  const char *name;
  void *who;

  /* The return value of dlsym_doit.  */
  void *sym;
};

static void
dlsym_doit (void *a)
{
  struct dlsym_args *args = (struct dlsym_args *) a;

  args->sym = _dl_sym (args->handle, args->name, args->who);
}

static void debug(const char *m)
{
	printf("profiler.so: %s\n",m);
}

static void *redirected=NULL;

void redirector()
{
  static int c=0;
  printf("redirected %p:%d\n",redirected,c++);
  ((void (*)())redirected)(); 
}

int register_hook(void *function)
{
  static int idx=0;

  if(function == NULL)
  {
    debug("-");
    return -1;
  }
  
  if(idx>0)
  {
    debug("+");
    return -1;
  }


  idx++;
  redirected = function;
  return 0;
}

void *
__dlsym (void *handle, const char *name DL_CALLER_DECL)
{
  void *result =NULL;	
  
# ifdef SHARED
  debug("SHARED");
  if (__builtin_expect (_dlfcn_hook != NULL, 0))
  { 
    register_hook(_dlfcn_hook->dlsym (handle, name, DL_CALLER));
    return redirector;
   }
# endif

  debug("non-SHARED");
  struct dlsym_args args;
  args.who = DL_CALLER;
  args.handle = handle;
  args.name = name;

  /* Protect against concurrent loads and unloads.  */
  __rtld_lock_lock_recursive (GL(dl_load_lock));

  result = (_dlerror_run (dlsym_doit, &args) ? NULL : args.sym);

  __rtld_lock_unlock_recursive (GL(dl_load_lock));

  register_hook(result);  
  result=(void*)redirector;
  return result;
}

# ifdef SHARED
strong_alias (__dlsym, dlsym)
# endif
#endif
