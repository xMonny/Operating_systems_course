#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		errx(1, "Invalid number of arguments");
	}
	const char* command_one = argv[1];
	const char* command_two = argv[2];
	const char* file_name = argv[3];
	
	pid_t p = fork();
	if (p < 0)
	{
		err(3, "Cannot fork");
	}
	if (p == 0)
	{
		if (execlp(command_one, "command_one", (char*)NULL) < 0)
		{
			err(4, "Cannot execute command %s", command_one);
		}
	}

	bool success = true;
	int status;
	pid_t wait_code = wait(&status);
	if (wait_code < 0)
	{
		err(5, "Error in waiting");
	}
	if (!WIFEXITED(status))
	{
		success = false;
		warnx("Command %s didn't execute normally", command_one);
	}
	else if (WEXITSTATUS(status) != 0)
	{
		success = false;
		warnx("Exit code of command %s is not 0", command_one);
	}	

	if (success)
	{
		int fd1 = open(file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (fd1 < 0)
		{
			err(6, "Cannot open file %s", file_name);
		}

		ssize_t write_size;
		write_size = write(fd1, command_one, strlen(command_one));
		if (write_size == -1)
		{
			const int olderrno = errno;
			close(fd1);
			errno = olderrno;
			err(7, "Error in writing in file %s", file_name);
		}
		if (write_size != (ssize_t)strlen(command_one))
		{
			const int olderrno = errno;
			close(fd1);
			errno = olderrno;
			errx(8, "Cannot write correctly in file %s", file_name);
		}
		close(fd1);
	}

	if (execlp(command_two, "command_two", (char*)NULL) < 0)
	{
		err(9, "Cannot execute command %s", file_name);
	}	

	exit(0);
}
