#include <unistd.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Invalid number of arguments");
	}

	int a[2];
	if (pipe(a) < 0)
	{
		err(2, "Cannot make a pipe");
	}

	pid_t p = fork();
	if (p < 0)
	{
		int olderrno = errno;
		close(a[0]);
		close(a[1]);
		errno = olderrno;
		err(3, "Cannot fork");
	}

	if (p == 0)
	{
		close(a[0]);
		if (dup2(a[1], 1) < 0)
		{
			int olderrno = errno;
			close(a[1]);
			errno = olderrno;
			err(4, "Cannot make a duplicate");
		}
		if (execlp("cat", "cat", argv[1], (char*)NULL) < 0)
		{
			int olderrno = errno;
			close(a[1]);
			errno = olderrno;
			err(5, "Cannot execute command cat");
		}
	}
	close(a[1]);

	if (dup2(a[0], 0) < 0)
	{
		int olderrno = errno;
		close(a[0]);
		errno = olderrno;
		err(6, "Cannot make a duplicate");
	}
	if (execlp("sort", "sort", "-n", (char*)NULL) < 0)
	{
		int olderrno = errno;
		close(a[0]);
		errno = olderrno;
		err(7, "Cannot execute command sort");
	}
}
