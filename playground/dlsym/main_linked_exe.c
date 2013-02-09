#include <stdio.h>

void hello(char * a);

int main(int argc, char** argv){
	int i = 0;

	void * ptr = &hello;

	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Linked exe started <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n" );
	printf(" hello = %x\n", ptr);


//	for (i=0; i<10; i++){
		char buf[100];
		sprintf(buf, "caller %d\n", i);	
		printf("Calling hello(buf):\t");
		hello(buf);
        hello1();
        hello2();
        hello3();
        hello4();
        hello5();
		testFunc();
//	}

	printf("PID == %d\n", getpid());

	return 0;
}
