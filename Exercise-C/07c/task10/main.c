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
		err(2, "Cannot fork");
	}
	if (one == 0)
	{
		if (execlp(command_one, "command_one", (char*)NULL) < 0)
		{
			err(3, "Cannot execute command %s", command_one);
		}
	}

	bool success = false;
	int status;
	pid_t wait_pid = wait(&status);
	if (wait_pid < 0)
	{
		err(4, "Error in waiting");
	}
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == 0)
		{
			success = true;
		}
	}

	if (!success)
	{
		exit(42);
	}

	if (execlp(command_two, "command_two", (char*)NULL) < 0)
	{
		err(5, "Cannot execute command %s", command_two);
	}

	exit(0);
}
