#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char** argv)
{
    //char* a = (char*)malloc(sizeof(char)*5);
    int i = 0;
    for (i = 0; i < 10; i++) {
        char* ok = (char*) malloc(sizeof(char));
        strcmp("ok", "ok");
        strdup(ok);
        strlen(ok);
        printf("This is a test call of strlen with result %d\n", strlen("ok"));
        char* buf = "This is a test string";
        hello(buf);
	}

	printf("PID == %d\n", getpid());

	return 0;
}
