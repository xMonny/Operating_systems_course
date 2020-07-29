#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		errx(1, "Invalid number of arguments");
	}
	
	for (int i = 1; i < argc; i++)
	{
		pid_t p = fork();
		if (p < 0)
		{
			err(2, "Cannot fork");
		}
		if (p == 0)
		{
			if (execlp(argv[i], "command", (char*)NULL) < 0)
			{
				err(3, "Cannot execute command %s", argv[i]);
			}
		}

		int status;
		pid_t wait_code = wait(&status);
		if (wait_code < 0)
		{
			err(4, "Error in waiting fommand %s", argv[i]);
		}
		if (!WIFEXITED(status))
		{
			warn("Command %s didn't terminate normally", argv[i]);
			continue;
		}
		int exit_status = WEXITSTATUS(status);

		printf("COMMAND: %s\nPID: %d EXIT_STATUS: %d\n", argv[i], wait_code, exit_status); 
	}
	exit(0);
}
