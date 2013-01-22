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
#include <time.h>

#include <ldsodefs.h>


#define _dl_debug_printf printf
#define NO_CONSOLE_OUTPUT_LD_SO
#include "../../src/libelfperf/ld-routines.h"

#if !defined SHARED && defined IS_IN_libdl


void *
dlsym (void *handle, const char *name)
{
//  elfperf_log("dlsym");
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


void *
__dlsym (void *handle, const char *name DL_CALLER_DECL)
{


  

# ifdef SHARED
	//elfperf_log("__dlsym ehter");
  if (__builtin_expect (_dlfcn_hook != NULL, 0))
    return _dlfcn_hook->dlsym (handle, name, DL_CALLER);
# endif

  struct dlsym_args args;
  args.who = DL_CALLER;
  args.handle = handle;
  args.name = name;

  /* Protect against concurrent loads and unloads.  */
  __rtld_lock_lock_recursive (GL(dl_load_lock));

  void *result = (_dlerror_run (dlsym_doit, &args) ? NULL : args.sym);

  __rtld_lock_unlock_recursive (GL(dl_load_lock));

  // Do initialization
  static struct ElfperfContext* elfperfContext = NULL;
  static struct RedirectorContext context;
  static struct ElfperfFunctions elfperfFuncs;
  static bool errorDuringElfperfContextLoading = 0;
  printf("libdl.so: doing routines for %s\n", name);
  // If profiling is not enabled or function is not profiled or  
  // there are problems during ElfperfContext loading
  // do not do initialization and skip next code
  if (!isElfPerfEnabled() || !isFunctionProfiled(name) || errorDuringElfperfContextLoading){
    printf("libdl.so: profiling is not enabled, skipping.\n");
    goto skip_elfperf_libdl;
  }

  // Recieve elfperfContext value
  if (elfperfContext == NULL){
    elfperfContext = getElfperfContextStorage();
    // Skip other steps if something went wrong during 
    // ElfperfContext loading
    if (elfperfContext == NULL){
      errorDuringElfperfContextLoading = 1;
      printf("libdl.so: problems during getElfperfContextStorage(), skipping.\n");
      goto skip_elfperf_libdl;
    }
    
    context = elfperfContext->context;
    elfperfFuncs = elfperfContext->addresses;
  
  }

  printf("libdl.so: successful intialization\n");
  // Do redirector generation if needed

  if (! (*(elfperfFuncs.isFunctionRedirectorRegistered))(name, context)){

    printf("libdl.so: searching function info\n");
    struct FunctionInfo* tmp = getInfoByName(name, elfperfContext->infos, context.count);

    if (tmp != NULL){
      tmp->addr = result;
    } else {
      printf("libdl.so: error - not found function for name(%s) by getInfoByName\n", name);
    }

    printf("libdl.so: function is not registered, do addNewFunction()\n");
    //_dl_error_printf("Function %s (%u) not registered, adding\n", name, value);
    (*(elfperfFuncs.addNewFunction))(name, result, context);
    //_dl_error_printf("Registration of %s successful.\n", name);
  }else{
    printf("libdl.so: function registred\n");
  }	
  result  =  (*(elfperfFuncs.getRedirectorAddressForName))( name,context);



skip_elfperf_libdl:

  return result;
}
# ifdef SHARED
strong_alias (__dlsym, dlsym)
# endif
#endif
