#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Expect one command");
	}

	const char* command = argv[1];
	struct stat st;
	const char* pipe_name = "named_pipe";
	if (stat(pipe_name, &st) < 0)
	{
		if (mkfifo(pipe_name, 0644) < 0)
		{
			err(2, "Cannot create a pipe '%s'", pipe_name);
		}
	}

	int fd_pipe = open(pipe_name, O_RDONLY);
	if (fd_pipe < 0)
	{
		err(3, "Cannot open pipe '%s'", pipe_name);
	}

	if (dup2(fd_pipe, 0) < 0)
	{
		const int olderrno = errno;
		close(fd_pipe);
		errno = olderrno;
		err(4, "Cannot make a duplicate");
	}

	if (execlp(command, "command", (char*)NULL) < 0)
	{
		const int olderrno = errno;
		close(fd_pipe);
		errno = olderrno;
		err(5, "Cannot execute command %s", command);
	}
}
