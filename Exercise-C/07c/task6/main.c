#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Expect only one name of command");
	}

	pid_t p = fork();
	if (p < 0)
	{
		err(2, "Cannot fork");
	}
	if (p == 0)
	{
		if (execlp(argv[1], "command", (char*)NULL) < 0)
		{
			err(3, "Cannot execute command %s", argv[1]);
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
		err(5, "Command %s didn't execute normally", argv[1]);
	}
	if (WEXITSTATUS(status) != 0)
	{
		errx(6, "Exit status of command %s is not 0", argv[1]);
	}

	printf("%s\n", argv[1]);

	exit(0);
}
