#define __x86_64_data_cache_size_half __x86_data_cache_size_half
#define __x86_64_shared_cache_size __x86_shared_cache_size
#define __x86_64_shared_cache_size_half __x86_shared_cache_size_half

#define DISABLE_PREFETCHW
#define DISABLE_PREFERRED_MEMORY_INSTRUCTION

#include <sysdeps/x86_64/cacheinfo.c>
