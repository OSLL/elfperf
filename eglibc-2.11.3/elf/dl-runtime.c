/* On-demand PLT fixup for shared objects.
   Copyright (C) 1995-2009, 2010, 2011 Free Software Foundation, Inc.
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

#define IN_DL_RUNTIME 1		/* This can be tested in dl-machine.h.  */

#include <alloca.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <ldsodefs.h>
#include <sysdep-cancel.h>
#include "dynamic-link.h"
#include <tls.h>

//////////////////////////////////////////////////////////////////////////////////////// 
#include <sched.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>    /* for PAGESIZE */

#include "../../src/libelfperf/ld-routines.h"
//////////////////////////////////////////////////////////////////////////////////

#if (!defined ELF_MACHINE_NO_RELA && !defined ELF_MACHINE_PLT_REL) \
    || ELF_MACHINE_NO_REL
# define PLTREL  ElfW(Rela)
#else
# define PLTREL  ElfW(Rel)
#endif

#ifndef VERSYMIDX
# define VERSYMIDX(sym)	(DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGIDX (sym))
#endif

/* The fixup functions might have need special attributes.  If none
   are provided define the macro as empty.  */
#ifndef ARCH_FIXUP_ATTRIBUTE
# define ARCH_FIXUP_ATTRIBUTE
#endif

#ifndef reloc_offset
# define reloc_offset reloc_arg
# define reloc_index  reloc_arg / sizeof (PLTREL)
#endif

#define ELFPERF_LIB_NAME "libelfperf.so"

/*
  Search for library @libname@'s link_map, and if it exists return pointer to it,
  otherwise return NULL.
*/

static struct link_map* getLibMap(char * libname, struct link_map *l)
{


	struct link_map* lib_map = NULL;
	struct link_map* tmp_map = NULL;

	_dl_error_printf("Doing search for %s linkmap\n", libname);
	// Finding libhello link_map by string name
	if (strstr(l->l_name,libname) == NULL){
		// If l is not the needed lib search in the previous and next libs 

		// Analyze previous libs
		if (l->l_prev != NULL){
			_dl_error_printf("Doing search for %s linkmap in PREVIOUS libs\n", libname);

			tmp_map = l;

			while (tmp_map->l_prev != NULL) {
				
				if (strstr(tmp_map->l_name,libname) != NULL){
					_dl_error_printf("Map for %s found: %s\n", libname, tmp_map->l_name);
					lib_map = tmp_map;
					break;
				}

				tmp_map = tmp_map->l_prev;
			}

		// Analyze next libs if they exists and we didnt found anything else
		}else if (l->l_next != NULL && lib_map == NULL){
			_dl_error_printf("Doing search for %s linkmap in NEXT libs\n", libname);

			tmp_map = l;

			while (tmp_map->l_next != NULL) {
				
				if (strstr(tmp_map->l_name,libname) != NULL){
					_dl_error_printf("Map for %s found: %s\n", libname, tmp_map->l_name);
					lib_map = tmp_map;
					break;
				}

				tmp_map = tmp_map->l_next;
			}

		}
		

	}else {
		// l is needed lib
		_dl_error_printf("%s found. l is %s\n", libname);
		lib_map = l;
	}
	return lib_map;
}


/*
  Search for library @libname@'s link_map, and if its exists try to get from it
  symbol with name @symbol_name@.
  @flags@ are domestic var resieved from dl_fixup
  @l@ is link_map from which we start search.

  If nothing found or something went wrong 0 is returned. 
*/
static DL_FIXUP_VALUE_TYPE getSymbolAddrFromLibrary(char * libname, char * symbol_name, 
	struct link_map *l, int flags){

	ElfW(Sym) * sym1 = (ElfW(Sym) *)malloc(sizeof(ElfW(Sym) ));
	lookup_t result1;
	DL_FIXUP_VALUE_TYPE value1 = 0;
//	char* name1 = "hello";
//	char* libname = "libhello.so";
	struct link_map* lib_map = getLibMap(libname, l);

	// If we found something
	if (lib_map != NULL){
	

		_dl_error_printf("Doing _dl_lookup_symbol_x for %s symbol\n", symbol_name);
		result1 = _dl_lookup_symbol_x (symbol_name, lib_map, &sym1, lib_map->l_scope,
					    NULL, ELF_RTYPE_CLASS_PLT, flags, NULL);
		
		if (sym1 !=NULL){
		
			_dl_error_printf("Found not NULL symbol for \"%s\"!\n", symbol_name);
			value1 = DL_FIXUP_MAKE_VALUE (result1,
						   sym1 ? (LOOKUP_VALUE_ADDRESS (result1)
							  + sym1->st_value) : 0);

			if (sym1 != NULL
			    && __builtin_expect (ELFW(ST_TYPE) (sym1->st_info) == STT_GNU_IFUNC, 0))
			  value1 = ((DL_FIXUP_VALUE_TYPE (*) (void)) DL_FIXUP_VALUE_ADDR (value1)) ();
				
		}else{
			
			_dl_error_printf("Error! Found NULL symbol for \"%s\"!\n", symbol_name);
		}
	}else {
		_dl_error_printf("Library not found!\n");
	}

	//free(sym1);
	return value1;
}



struct ElfperfFunctions {

/*	void (* wrapper)();
	void (* initWrapperRedirectors)(char**, unsigned int, void *);
	void (* addNewFunction)(char* , void *);
	bool (* isFunctionRedirectorRegistered)(char*);
	bool (* isFunctionInFunctionList)(char*);
	void * (* getRedirectorAddressForName)(char*);*/

	void (* wrapper)();
	void (* initWrapperRedirectors)(struct RedirectorContext*);
	void (* addNewFunction)(char* , void *, struct RedirectorContext);
	bool (* isFunctionRedirectorRegistered)(char*, struct RedirectorContext);
	bool (* isFunctionInFunctionList)(char*, struct RedirectorContext);
	void * (* getRedirectorAddressForName)(char*, struct RedirectorContext);
	void * storage;
};

#define ELFPERF_WRAPPER_SYMBOL "wrapper"
#define ELFPERF_GET_REDIRECTOR_ADDRESS_FOR_NAME_SYMBOL "getRedirectorAddressForName"
#define ELFPERF_IS_FUNCTION_REDIRECTOR_REGISTERED_SYMBOL "isFunctionRedirectorRegistered"
#define ELFPERF_IS_FUNCTION_IN_FUNCTION_LIST_SYMBOL "isFunctionInFunctionList"
#define ELFPERF_ADD_NEW_FUNCTION_SYMBOL "addNewFunction"
#define ELFPERF_INIT_WRAPPER_REDIRECTORS_SYMBOL "initWrapperRedirectors"
#define ELFPERF_STORAGE "storage"

/*
  Return pointer to struct ElfperfFunctions with pointers to the all needed routines.
  If something went wrong NULL returned.
*/
static struct ElfperfFunctions * getElfperfFunctions(struct link_map* l, int flags){

	struct ElfperfFunctions * result = 
		(struct ElfperfFunctions *)malloc(sizeof(struct ElfperfFunctions));
	

	result->wrapper =  getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_WRAPPER_SYMBOL, l, flags);
	result->initWrapperRedirectors =  getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_INIT_WRAPPER_REDIRECTORS_SYMBOL, l, flags);
	result->addNewFunction =  getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_ADD_NEW_FUNCTION_SYMBOL, l, flags);
	result->isFunctionInFunctionList = getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_IS_FUNCTION_IN_FUNCTION_LIST_SYMBOL, l, flags);
	result->isFunctionRedirectorRegistered =  getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_IS_FUNCTION_REDIRECTOR_REGISTERED_SYMBOL, l, flags);
	result->getRedirectorAddressForName =  getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_GET_REDIRECTOR_ADDRESS_FOR_NAME_SYMBOL, l, flags);

	result->storage = getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_STORAGE, l, flags);

	// Something went wrong - symbol not found
	if ( result->wrapper == NULL || result->initWrapperRedirectors == NULL 
		|| result->addNewFunction == NULL || result->isFunctionRedirectorRegistered == NULL 
		|| result->getRedirectorAddressForName == NULL || result->isFunctionInFunctionList == NULL)
	return NULL;

	return result;

}

/* This function is called through a special trampoline from the PLT the
   first time each PLT entry is called.  We must perform the relocation
   specified in the PLT of the given shared object, and return the resolved
   function address to the trampoline, which will restart the original call
   to that address.  Future calls will bounce directly from the PLT to the
   function.  */

#ifndef ELF_MACHINE_NO_PLT
DL_FIXUP_VALUE_TYPE
__attribute ((noinline)) ARCH_FIXUP_ATTRIBUTE
_dl_fixup (
# ifdef ELF_MACHINE_RUNTIME_FIXUP_ARGS
	   ELF_MACHINE_RUNTIME_FIXUP_ARGS,
# endif
	   /* GKM FIXME: Fix trampoline to pass bounds so we can do
	      without the `__unbounded' qualifier.  */
	   struct link_map *__unbounded l, ElfW(Word) reloc_arg)
{
  const ElfW(Sym) *const symtab
    = (const void *) D_PTR (l, l_info[DT_SYMTAB]);
  const char *strtab = (const void *) D_PTR (l, l_info[DT_STRTAB]);

  const PLTREL *const reloc
    = (const void *) (D_PTR (l, l_info[DT_JMPREL]) + reloc_offset);
  const ElfW(Sym) *sym = &symtab[ELFW(R_SYM) (reloc->r_info)];
  void *const rel_addr = (void *)(l->l_addr + reloc->r_offset);
  lookup_t result;
  DL_FIXUP_VALUE_TYPE value;

  char* name = 0;
  int flags;

  /* Sanity check that we're really looking at a PLT relocation.  */
  assert (ELFW(R_TYPE)(reloc->r_info) == ELF_MACHINE_JMP_SLOT);

   /* Look up the target symbol.  If the normal lookup rules are not
      used don't look in the global scope.  */
  if (__builtin_expect (ELFW(ST_VISIBILITY) (sym->st_other), 0) == 0)
    {
      const struct r_found_version *version = NULL;

      if (l->l_info[VERSYMIDX (DT_VERSYM)] != NULL)
	{
	  const ElfW(Half) *vernum =
	    (const void *) D_PTR (l, l_info[VERSYMIDX (DT_VERSYM)]);
	  ElfW(Half) ndx = vernum[ELFW(R_SYM) (reloc->r_info)] & 0x7fff;
	  version = &l->l_versions[ndx];
	  if (version->hash == 0)
	    version = NULL;
	}

      /* We need to keep the scope around so do some locking.  This is
	 not necessary for objects which cannot be unloaded or when
	 we are not using any threads (yet).  */
      flags = DL_LOOKUP_ADD_DEPENDENCY;
      if (!RTLD_SINGLE_THREAD_P)
	{
	  THREAD_GSCOPE_SET_FLAG ();
	  flags |= DL_LOOKUP_GSCOPE_LOCK;
	}

#ifdef RTLD_ENABLE_FOREIGN_CALL
      RTLD_ENABLE_FOREIGN_CALL;
#endif
//	_dl_error_printf("dl-runtime1\n");
      name = strtab + sym->st_name;

//      _dl_error_printf("Doing _dl_lookup_symbol_x for %s\n",name);

      result = _dl_lookup_symbol_x (strtab + sym->st_name, l, &sym, l->l_scope,
				    version, ELF_RTYPE_CLASS_PLT, flags, NULL);

      /* We are done with the global scope.  */
      if (!RTLD_SINGLE_THREAD_P)
	THREAD_GSCOPE_RESET_FLAG ();

#ifdef RTLD_FINALIZE_FOREIGN_CALL
      RTLD_FINALIZE_FOREIGN_CALL;
#endif

      /* Currently result contains the base load address (or link map)
	 of the object that defines sym.  Now add in the symbol
	 offset.  */
      value = DL_FIXUP_MAKE_VALUE (result,
				   sym ? (LOOKUP_VALUE_ADDRESS (result)
					  + sym->st_value) : 0);
//	_dl_error_printf("dl-runtime3\n");
    }
  else
    {
      /* We already found the symbol.  The module (and therefore its load
	 address) is also known.  */
//	_dl_error_printf("dl-runtime4\n");
      value = DL_FIXUP_MAKE_VALUE (l, l->l_addr + sym->st_value);
	_dl_error_printf("dl-runtime5\n");
      result = l;
    }

//	_dl_error_printf("dl-runtime6\n");
  /* And now perhaps the relocation addend.  */
  value = elf_machine_plt_value (l, reloc, value);

//	_dl_error_printf("dl-runtime7\n");
  if (sym != NULL
      && __builtin_expect (ELFW(ST_TYPE) (sym->st_info) == STT_GNU_IFUNC, 0))
    value = ((DL_FIXUP_VALUE_TYPE (*) (void)) DL_FIXUP_VALUE_ADDR (value)) ();

//	_dl_error_printf("dl-runtime8\n");
  /* Finally, fix up the plt itself.  */
  if (__builtin_expect (GLRO(dl_bind_not), 0))
    return value;


/// Check that profiling by ELFPERF is enabled and ELFPERF_LIB was found among LD_PRELOAD libs

	static struct ElfperfFunctions * elfperfFuncs = NULL;
	static bool errorDuringElfperfFunctionLoad = 0;
	static struct RedirectorContext context;
	static struct FunctionInfo * infos;
	static int initialized = 0;

	if (isElfPerfEnabled() && !initialized){
		
		initFunctionStatisticsStorage();
	}

	if (isElfPerfEnabled() && isFunctionProfiled(name) && getLibMap(ELFPERF_LIB_NAME, l) != NULL 
		&& !errorDuringElfperfFunctionLoad) {

		_dl_error_printf("All conditions for %s profiling is fine\n", name);
	}else{
		goto skip_elfperf;
	}

	// Try to get structure with functions
	// if unsuccess - elfperf routines will be skipped
	if (elfperfFuncs == NULL){
		// Getting pointers to all needed functions
		_dl_error_printf("Recieving functoins pointers from libelfperf.so\n");
		elfperfFuncs = getElfperfFunctions(l, flags);
		// skip elfperf part 
		if (elfperfFuncs == NULL){
			_dl_error_printf("Errors during getElfperfFunctions!\n");
			errorDuringElfperfFunctionLoad = 1;
			goto skip_elfperf;
		}
	}
	_dl_error_printf("Recieved all pointers to functions from libelfperf.so\n");

	//            // ELFPERF


	if(0 == initialized)
	{
		_dl_error_printf("Redirectors are not initialized.\n");
//		char **names = 0;//={"hello"};
//		unsigned int count = 0;


		context.names = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &(context.count));
		context.redirectors = elfperfFuncs->storage;		

		_dl_error_printf("Initializing redirectors for:\n");
		unsigned int i = 0;
		for ( i =0 ; i < context.count; i++){
			_dl_error_printf("\t%s\n", context.names[i]);
		}
		void *wr = elfperfFuncs->wrapper;

		

		_dl_error_printf("Going to initWrapperRedirectors\n");
		( * (elfperfFuncs->initWrapperRedirectors))(&context);
		__sync_fetch_and_add(&initialized, 1);
		_dl_error_printf("After setting initialized , curr val = %u\n", initialized);


		infos = getFunctionInfoStorage();
		_dl_error_printf("After FunctionInfo storage initialization %x\n", infos);

	}

	_dl_error_printf("Doing routines for ELFPERF\n");
	if (! (*(elfperfFuncs->isFunctionRedirectorRegistered))(name, context)){
		struct FunctionInfo* tmp = getInfoByName(name, infos, context.count);
		
		if (tmp != NULL){
			tmp->addr = value;
		} else {
			_dl_error_printf("Error - not found function for name by getInfoByName\n");
		}

		_dl_error_printf("Function %s (%u) not registered, adding\n", name, value);
		(*(elfperfFuncs->addNewFunction))(name,(void*) value, context);
		_dl_error_printf("Registration of %s successful.\n", name);
	}	
	DL_FIXUP_VALUE_TYPE testFuncAddr= getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, "testFunc", l, flags);

	_dl_error_printf("Getting redirector address for  %s \n", name);
	DL_FIXUP_VALUE_TYPE value1 = (DL_FIXUP_VALUE_TYPE) (*(elfperfFuncs->getRedirectorAddressForName))( name,context);
	_dl_error_printf("Got redirector address for %s, real_addr+3 = %x, redir_addr = %x, storage = %x, testFunc = %x\n", 
		name, value+3, value1, elfperfFuncs->storage, testFuncAddr);
     	value = value1;

	void ** ptr =  ((void*) value1);
	int j;

	_dl_error_printf("Bytes of redirector:\n");
	
	for (j=0 ;j<4; j++){
	_dl_error_printf("\t%x %x %x %x \n", 
		(((unsigned int)ptr[j]) ) & 0xFF, (((unsigned int)ptr[j]) >> 8) & 0xFF,
		(((unsigned int)ptr[j]) >> 16) & 0xFF, (((unsigned int)ptr[j]) >> 24) & 0xFF);
	}

	//value = getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, "testFunc", l, flags); 

//getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, ELFPERF_ADD_NEW_FUNCTION_SYMBOL, l, flags);

skip_elfperf:

  _dl_error_printf("Before returning from dl-fixup_plt\n");

  return elf_machine_fixup_plt (l, result, reloc, rel_addr, value);
}
#endif

#if !defined PROF && !defined ELF_MACHINE_NO_PLT && !__BOUNDED_POINTERS__
DL_FIXUP_VALUE_TYPE
__attribute ((noinline)) ARCH_FIXUP_ATTRIBUTE
_dl_profile_fixup (
#ifdef ELF_MACHINE_RUNTIME_FIXUP_ARGS
		   ELF_MACHINE_RUNTIME_FIXUP_ARGS,
#endif
		   struct link_map *l, ElfW(Word) reloc_arg,
		   ElfW(Addr) retaddr, void *regs, long int *framesizep)
{
  void (*mcount_fct) (ElfW(Addr), ElfW(Addr)) = INTUSE(_dl_mcount);

  /* This is the address in the array where we store the result of previous
     relocations.  */
  struct reloc_result *reloc_result = &l->l_reloc_result[reloc_index];
  DL_FIXUP_VALUE_TYPE *resultp = &reloc_result->addr;

  DL_FIXUP_VALUE_TYPE value = *resultp;
  if (DL_FIXUP_VALUE_CODE_ADDR (value) == 0)
    {
      /* This is the first time we have to relocate this object.  */
      const ElfW(Sym) *const symtab
	= (const void *) D_PTR (l, l_info[DT_SYMTAB]);
      const char *strtab = (const char *) D_PTR (l, l_info[DT_STRTAB]);

      const PLTREL *const reloc
	= (const void *) (D_PTR (l, l_info[DT_JMPREL]) + reloc_offset);
      const ElfW(Sym) *refsym = &symtab[ELFW(R_SYM) (reloc->r_info)];
      const ElfW(Sym) *defsym = refsym;
      lookup_t result;

      /* Sanity check that we're really looking at a PLT relocation.  */
      assert (ELFW(R_TYPE)(reloc->r_info) == ELF_MACHINE_JMP_SLOT);

      /* Look up the target symbol.  If the symbol is marked STV_PROTECTED
	 don't look in the global scope.  */
      if (__builtin_expect (ELFW(ST_VISIBILITY) (refsym->st_other), 0) == 0)
	{
	  const struct r_found_version *version = NULL;

	  if (l->l_info[VERSYMIDX (DT_VERSYM)] != NULL)
	    {
	      const ElfW(Half) *vernum =
		(const void *) D_PTR (l, l_info[VERSYMIDX (DT_VERSYM)]);
	      ElfW(Half) ndx = vernum[ELFW(R_SYM) (reloc->r_info)] & 0x7fff;
	      version = &l->l_versions[ndx];
	      if (version->hash == 0)
		version = NULL;
	    }

	  /* We need to keep the scope around so do some locking.  This is
	     not necessary for objects which cannot be unloaded or when
	     we are not using any threads (yet).  */
	  int flags = DL_LOOKUP_ADD_DEPENDENCY;
	  if (!RTLD_SINGLE_THREAD_P)
	    {
	      THREAD_GSCOPE_SET_FLAG ();
	      flags |= DL_LOOKUP_GSCOPE_LOCK;
	    }
	  _dl_error_printf("dl-runtime2\n");
	  result = _dl_lookup_symbol_x (strtab + refsym->st_name, l,
					&defsym, l->l_scope, version,
					ELF_RTYPE_CLASS_PLT, flags, NULL);

	  /* We are done with the global scope.  */
	  if (!RTLD_SINGLE_THREAD_P)
	    THREAD_GSCOPE_RESET_FLAG ();

	  /* Currently result contains the base load address (or link map)
	     of the object that defines sym.  Now add in the symbol
	     offset.  */
	  value = DL_FIXUP_MAKE_VALUE (result,
				       defsym != NULL
				       ? LOOKUP_VALUE_ADDRESS (result)
					 + defsym->st_value : 0);

	  if (defsym != NULL
	      && __builtin_expect (ELFW(ST_TYPE) (defsym->st_info)
				   == STT_GNU_IFUNC, 0))
	    value = ((DL_FIXUP_VALUE_TYPE (*) (void))
		     DL_FIXUP_VALUE_ADDR (value)) ();
	}
      else
	{
	  /* We already found the symbol.  The module (and therefore its load
	     address) is also known.  */
	  value = DL_FIXUP_MAKE_VALUE (l, l->l_addr + refsym->st_value);

	  if (__builtin_expect (ELFW(ST_TYPE) (refsym->st_info)
				== STT_GNU_IFUNC, 0))
	    value = ((DL_FIXUP_VALUE_TYPE (*) (void))
		     DL_FIXUP_VALUE_ADDR (value)) ();

	  result = l;
	}
      /* And now perhaps the relocation addend.  */
      value = elf_machine_plt_value (l, reloc, value);

#ifdef SHARED
      /* Auditing checkpoint: we have a new binding.  Provide the
	 auditing libraries the possibility to change the value and
	 tell us whether further auditing is wanted.  */
      if (defsym != NULL && GLRO(dl_naudit) > 0)
	{
	  reloc_result->bound = result;
	  /* Compute index of the symbol entry in the symbol table of
	     the DSO with the definition.  */
	  reloc_result->boundndx = (defsym
				    - (ElfW(Sym) *) D_PTR (result,
							   l_info[DT_SYMTAB]));

	  /* Determine whether any of the two participating DSOs is
	     interested in auditing.  */
	  if ((l->l_audit_any_plt | result->l_audit_any_plt) != 0)
	    {
	      unsigned int altvalue = 0;
	      struct audit_ifaces *afct = GLRO(dl_audit);
	      /* Synthesize a symbol record where the st_value field is
		 the result.  */
	      ElfW(Sym) sym = *defsym;
	      sym.st_value = DL_FIXUP_VALUE_ADDR (value);

	      /* Keep track whether there is any interest in tracing
		 the call in the lower two bits.  */
	      assert (DL_NNS * 2 <= sizeof (reloc_result->flags) * 8);
	      assert ((LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT) == 3);
	      reloc_result->enterexit = LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT;

	      const char *strtab2 = (const void *) D_PTR (result,
							  l_info[DT_STRTAB]);

	      for (unsigned int cnt = 0; cnt < GLRO(dl_naudit); ++cnt)
		{
		  /* XXX Check whether both DSOs must request action or
		     only one */
		  if ((l->l_audit[cnt].bindflags & LA_FLG_BINDFROM) != 0
		      && (result->l_audit[cnt].bindflags & LA_FLG_BINDTO) != 0)
		    {
		      unsigned int flags = altvalue;
		      if (afct->symbind != NULL)
			{
			  uintptr_t new_value
			    = afct->symbind (&sym, reloc_result->boundndx,
					     &l->l_audit[cnt].cookie,
					     &result->l_audit[cnt].cookie,
					     &flags,
					     strtab2 + defsym->st_name);
			  if (new_value != (uintptr_t) sym.st_value)
			    {
			      altvalue = LA_SYMB_ALTVALUE;
			      sym.st_value = new_value;
			    }
			}

		      /* Remember the results for every audit library and
			 store a summary in the first two bits.  */
		      reloc_result->enterexit
			&= flags & (LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT);
		      reloc_result->enterexit
			|= ((flags & (LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT))
			    << ((cnt + 1) * 2));
		    }
		  else
		    /* If the bind flags say this auditor is not interested,
		       set the bits manually.  */
		    reloc_result->enterexit
		      |= ((LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT)
			  << ((cnt + 1) * 2));

		  afct = afct->next;
		}

	      reloc_result->flags = altvalue;
	      value = DL_FIXUP_ADDR_VALUE (sym.st_value);
	    }
	  else
	    /* Set all bits since this symbol binding is not interesting.  */
	    reloc_result->enterexit = (1u << DL_NNS) - 1;
	}
#endif

      /* Store the result for later runs.  */
      if (__builtin_expect (! GLRO(dl_bind_not), 1))
	*resultp = value;
    }

  /* By default we do not call the pltexit function.  */
  long int framesize = -1;

#ifdef SHARED
  /* Auditing checkpoint: report the PLT entering and allow the
     auditors to change the value.  */
  if (DL_FIXUP_VALUE_CODE_ADDR (value) != 0 && GLRO(dl_naudit) > 0
      /* Don't do anything if no auditor wants to intercept this call.  */
      && (reloc_result->enterexit & LA_SYMB_NOPLTENTER) == 0)
    {
      ElfW(Sym) *defsym = ((ElfW(Sym) *) D_PTR (reloc_result->bound,
						l_info[DT_SYMTAB])
			   + reloc_result->boundndx);

      /* Set up the sym parameter.  */
      ElfW(Sym) sym = *defsym;
      sym.st_value = DL_FIXUP_VALUE_ADDR (value);

      /* Get the symbol name.  */
      const char *strtab = (const void *) D_PTR (reloc_result->bound,
						 l_info[DT_STRTAB]);
      const char *symname = strtab + sym.st_name;

      /* Keep track of overwritten addresses.  */
      unsigned int altvalue = reloc_result->flags;

      struct audit_ifaces *afct = GLRO(dl_audit);
      for (unsigned int cnt = 0; cnt < GLRO(dl_naudit); ++cnt)
	{
	  if (afct->ARCH_LA_PLTENTER != NULL
	      && (reloc_result->enterexit
		  & (LA_SYMB_NOPLTENTER << (2 * (cnt + 1)))) == 0)
	    {
	      unsigned int flags = altvalue;
	      long int new_framesize = -1;
	      uintptr_t new_value
		= afct->ARCH_LA_PLTENTER (&sym, reloc_result->boundndx,
					  &l->l_audit[cnt].cookie,
					  &reloc_result->bound->l_audit[cnt].cookie,
					  regs, &flags, symname,
					  &new_framesize);
	      if (new_value != (uintptr_t) sym.st_value)
		{
		  altvalue = LA_SYMB_ALTVALUE;
		  sym.st_value = new_value;
		}

	      /* Remember the results for every audit library and
		 store a summary in the first two bits.  */
	      reloc_result->enterexit
		|= ((flags & (LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT))
		    << (2 * (cnt + 1)));

	      if ((reloc_result->enterexit & (LA_SYMB_NOPLTEXIT
					      << (2 * (cnt + 1))))
		  == 0 && new_framesize != -1 && framesize != -2)
		{
		  /* If this is the first call providing information,
		     use it.  */
		  if (framesize == -1)
		    framesize = new_framesize;
		  /* If two pltenter calls provide conflicting information,
		     use the larger value.  */
		  else if (new_framesize != framesize)
		    framesize = MAX (new_framesize, framesize);
		}
	    }

	  afct = afct->next;
	}

      value = DL_FIXUP_ADDR_VALUE (sym.st_value);
    }
#endif

  /* Store the frame size information.  */
  *framesizep = framesize;

  (*mcount_fct) (retaddr, DL_FIXUP_VALUE_CODE_ADDR (value));

  return value;
}

#endif /* PROF && ELF_MACHINE_NO_PLT */


#include <stdio.h>
void
ARCH_FIXUP_ATTRIBUTE
_dl_call_pltexit (struct link_map *l, ElfW(Word) reloc_arg,
		  const void *inregs, void *outregs)
{
#ifdef SHARED
  /* This is the address in the array where we store the result of previous
     relocations.  */
  // XXX Maybe the bound information must be stored on the stack since
  // XXX with bind_not a new value could have been stored in the meantime.
  struct reloc_result *reloc_result = &l->l_reloc_result[reloc_index];
  ElfW(Sym) *defsym = ((ElfW(Sym) *) D_PTR (reloc_result->bound,
					    l_info[DT_SYMTAB])
		       + reloc_result->boundndx);

  /* Set up the sym parameter.  */
  ElfW(Sym) sym = *defsym;
  sym.st_value = DL_FIXUP_VALUE_ADDR (reloc_result->addr);

  /* Get the symbol name.  */
  const char *strtab = (const void *) D_PTR (reloc_result->bound,
					     l_info[DT_STRTAB]);
  const char *symname = strtab + sym.st_name;

  struct audit_ifaces *afct = GLRO(dl_audit);
  for (unsigned int cnt = 0; cnt < GLRO(dl_naudit); ++cnt)
    {
      if (afct->ARCH_LA_PLTEXIT != NULL
	  && (reloc_result->enterexit
	      & (LA_SYMB_NOPLTEXIT >> (2 * cnt))) == 0)
	{
	  afct->ARCH_LA_PLTEXIT (&sym, reloc_result->boundndx,
				 &l->l_audit[cnt].cookie,
				 &reloc_result->bound->l_audit[cnt].cookie,
				 inregs, outregs, symname);
	}

      afct = afct->next;
    }
#endif
}
