#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		errx(1, "Expect at least one command as arugment");
	}
	
	int success = 0;
	int wrong = 0;
	for (int i = 1; i < argc; i++)
	{
		pid_t p = fork();
		if (p < 0)
		{
			err(2, "Cannot fork");
		}
		if (p == 0)
		{
			if (execlp(argv[i], "command_", (char*)NULL) < 0)
			{
				err(3, "Cannot execute command %s", argv[i]);
			}
		}

		int status;
		pid_t wait_code = wait(&status);
		if (wait_code < 0)
		{
			err(4, "Error in waiting");
		}
		if (!WIFEXITED(status))
		{
			wrong++;
			warnx("Command %s didn't terminate normally", argv[i]);
		}
		else if (WEXITSTATUS(status))
		{
			wrong++;
			warnx("Exit status of command %s is not 0", argv[i]);
			continue;
		}
		success++;
	}

	printf("Succeeded: %d\nError: %d\n", success, wrong);
	
	exit(0);
}
