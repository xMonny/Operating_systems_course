#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		errx(1, "Invalid number of arguments");
	}

	const char* command_one = argv[1];
	const char* command_two = argv[2];

	pid_t one = fork();
	if (one < 0)
	{
		err(2, "Error in fork");
	}
	if (one == 0)
	{
		if (execlp(command_one, "c1", (char*)NULL) < 0)
		{
			err(3, "Cannot execute command %s", command_one);
		}
	}

	pid_t two = fork();
	if (two < 0)
	{
		err(4, "Error in fork");
	}
	if (two == 0)
	{
		if (execlp(command_two, "c2", (char*)NULL) < 0)
		{
			err(5, "Cannot execute command %s", command_two);
		}
	}

	bool first = false;

	int status_one;
	pid_t wait_one = wait(&status_one);
	if (wait_one < 0)
	{
		err(6, "Error in waiting");
	}
	if (WIFEXITED(status_one))
	{
		if (WEXITSTATUS(status_one) == 0)
		{
			first = true;
			printf("PID (first successful command): %d\n", wait_one);
		}
	}

	if (!first)
	{
		int status_two;
		pid_t wait_two = wait(&status_two);
		if (wait_two < 0)
		{
			err(7, "Error in waiting");
		}
		if (WIFEXITED(status_two))
		{
			if (WEXITSTATUS(status_two) == 0)
			{
				first = true;
				printf("PID (first successful command): %d\n", wait_two);
			}
		}
	}

	if (wait(NULL) < 0)
	{
		err(8, "Error in waiting children");
	}

	if (!first)
	{
		printf("-1\n");
	}

	exit(0);
}
