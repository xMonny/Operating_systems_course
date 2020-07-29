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
		errx(1, "Expect one name of file");
	}

	const char* input = argv[1];
	struct stat st;
	if (stat(input, &st) < 0)
	{
		err(2, "Cannot stat file '%s'", input);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "Input argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File '%s' cannot be read", input);
	}

	const char* pipe_name = "named_pipe";
	if (stat(pipe_name, &st) < 0)
	{
		if (mkfifo(pipe_name, 0644) < 0)
		{
			err(5, "Cannot create pipe '%s'", pipe_name);
		}
	}

	int fd_pipe = open(pipe_name, O_WRONLY);
	if (fd_pipe < 0)
	{
		err(6, "Cannot open pipe '%s'", pipe_name);
	}

	if (dup2(fd_pipe, 1) < 0)
	{
		const int olderrno = errno;
		close(fd_pipe);
		errno = olderrno;
		err(7, "Cannot make a duplicate");
	}

	if (execlp("cat", "cat", input, (char*)NULL) < 0)
	{
		const int olderrno = errno;
		close(fd_pipe);
		errno = olderrno;
		err(8, "Cannot execute command cat");
	}
}
