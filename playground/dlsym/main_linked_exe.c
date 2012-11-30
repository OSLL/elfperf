#include <stdio.h>


int main(){
	int i = 0;

	for (i=0; i<10; i++){
		char buf[100];
		sprintf(buf, "caller %d\n", i);	
		hello(buf);
	}

	return 0;
}
