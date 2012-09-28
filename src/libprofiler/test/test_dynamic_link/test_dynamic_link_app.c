#include "../test_lib/test_lib.h"
#include <stdio.h>



int main(){

	printf("Calling testFunction1(): %d\n", testFunction1(1,1));
	printf("Calling testFunction2(): %d\n", testFunction2());

	printf("Calling testFunction3()\n");
	testFunction3(1,1);

	printf("Calling testFunction4()\n");
	testFunction4();


	return 0;

}
