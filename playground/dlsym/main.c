#include <stdio.h>
#include <dlfcn.h>

int main()
{
int i=0;
	void *p = dlopen("./libhello.so",RTLD_LAZY);
	printf("handle=%p\n",p);
	
	void *f = dlsym(p,"hello");
	printf("f=%p\n",f);

for(i=0;i<3;i++)
{
   char buf[100];
   sprintf(buf, "caller %d", i );	
	((void (*)(const char*))f)(buf);

}
	return 0;
}
