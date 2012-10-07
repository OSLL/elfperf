#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <signal.h>

char ** transform_argv(int argc, char** argv)
{
	char ** new_argv = (char**)malloc((sizeof(char*))*(argc-1));
	int i;
	size_t string_length;
	for (i =0; i<(argc-1); i++){
		string_length = strlen(argv[i+1]);
		new_argv[i] = (char*)malloc(sizeof(char) * string_length );
		memcpy(new_argv[i], argv[i+1], string_length );
//		printf("%s, %s, %d\n",new_argv[i], argv[i+1], string_length);
	}
	return new_argv;
}

int main(int argc, char** argv)
{

	char** new_argv = transform_argv(argc, argv);
	pid_t pid, status;
	if (argc < 2){
		puts("Not enaugh arguments!");
	}else{

		pid = fork();

		if (pid < 0) {
			puts("Error! ");
                	exit(1);
        	}else if(!pid) {
			// Child process
			ptrace(PTRACE_TRACEME, 0, 0, 0);
			execvp(argv[1], new_argv );
		}
		printf("child pid = %d", pid);

	}
	// Parent process
	// waiting for child process state changing (PTRACE_TRACEME)
	wait(&status);
	ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);

	while (!WIFEXITED(status)) {

		struct user_regs_struct state;

		ptrace(PTRACE_SYSCALL, pid, 0, 0);
		wait( &status);

		// at syscall
		if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80) {
		    ptrace(PTRACE_GETREGS, pid, 0, &state);
		    printf("SYSCALL %ld at %08lx\n", state.orig_rax, state.rip);
		    
		    // skip after syscall
		    ptrace(PTRACE_SYSCALL, pid, 0, 0);
		    wait( &status);

		}
	}
	return 0;
}
