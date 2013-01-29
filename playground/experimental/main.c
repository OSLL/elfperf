#include <stdio.h>

int main (int argc, char** argv)
{
    int i = 0;

    for (i = 0; i < 10; i++) {
        char buf[100];
        hello(buf);
        testFunc();
	}

	printf("PID == %d\n", getpid());

	return 0;
}
