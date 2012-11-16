#include "stdio.h"

#define SIZE 10

// Testing shifting functions of asm 

int main(){

	int *testArray = (int*)malloc(sizeof(int)*SIZE);
	int i;
	for (i=0; i<SIZE; i++){
		*(testArray+i)=i;
		printf("%d ", testArray[i]);
	}
	printf("\n");

	int end = testArray+(SIZE);
	int start = testArray+1;
	printf("\nChecking shift below\n");
	// Move array for one word (4bytes) below ( high adresses -> low addreses)
	asm volatile(	"movl %0, %%ecx\n" 		// End of the moved area - bottom addr
		"movl %1, %%ebx\n"			// Start of the moved area - highest addr
		"shift_act: movl (%%ecx), %%eax\n" 	// Loop:
			"movl %%eax, -4(%%ecx)\n" 		// -4(%%ecx) = (%%ecx)
			"addl $0x4, %%ecx\n" 			// %%ecx += 4
			"cmp %%ecx, %%ebx\n" 			// if (%%ecx == %%ebx)
			"jne shift_act\n"			// goto loop_act
		 : :"m"(start),"m"(end) :"%ecx","%ebx","%eax"	// -4(%%ebx) = %%eax
		 
	);

	// Check 
	// Valid sequence is 1 2 3 4 5 6 7 8 9 9
        for (i=0; i<SIZE-1; i++){
                if ( testArray[i]!= i+1 ) {
			printf("Test of shift_below failed\n");
			break;
		}
        }
	if (testArray[ SIZE-1] !=  SIZE-1){
		printf("Test of shift_below failed\n");
	}
	////////

	for (i=0; i<SIZE; i++){
                printf("%d ", testArray[i]);            
        }
	printf("\nChecking shift_above\n");



	end = testArray+SIZE-1;
	start = testArray;
	// Move array for one word (4bytes) above ( high adresses <- low addreses)
	asm(	
		"movl %0, %%ebx\n"				// End of the moved area
		"movl %1, %%ecx\n"				// Start of the moved area
		"backshift_act: movl (%%ecx), %%eax\n"
			"movl %%eax, 4(%%ecx)\n"
			"subl $0x4, %%ecx\n"
			"cmp %%ecx, %%ebx\n"
			"jng backshift_act\n" 
		::"m"(start),"m"(end):"%ecx","%ebx","%eax");

	for (i=0; i<SIZE; i++){
                printf("%d ", testArray[i]);            
        }

	// Check 
	// Valid sequence is 1 1 2 3 4 5 6 7 8 9
        for (i=1; i<SIZE; i++){
                if ( testArray[i]!= i  ) {
			printf("Test of shift_above failed");
			break;
		}
        }
	if (testArray[0] != 1){
		printf("Test of shift_above failed\n");
	}
	///////
	


	return 0;
}
