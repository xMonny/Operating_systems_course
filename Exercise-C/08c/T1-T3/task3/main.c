#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
		const int olderrno = errno;
		close(a[0]);
		close(a[1]);
		errno = olderrno;
		err(3, "Cannot fork");
	}
	if (p == 0)
	{
		close(a[1]);

		if (dup2(a[0], 0) < 0)
		{
			const int olderrno = errno;
                	close(a[0]);
                	errno = olderrno;
                	err(4, "Cannot make a duplicate");
		}
		if (execlp("wc", "wc", "-c", (char*)NULL) < 0)
		{
			const int olderrno = errno;
                        close(a[0]);
                        errno = olderrno;
                        err(5, "Cannot execute command wc");
		}
	}
	close(a[0]);

	ssize_t write_size;
	write_size = write(a[1], argv[1], strlen(argv[1]));
	if (write_size == -1)
	{
		const int olderrno = errno;
                close(a[1]);
                errno = olderrno;
                err(6, "Error in writing in pipe");
	}
	if (write_size != (ssize_t)strlen(argv[1]))
	{
		const int olderrno = errno;
                close(a[1]);
                errno = olderrno;
                errx(7, "Cannot write correctly in pipe");
	}

	close(a[1]);

	if (wait(NULL) < 0)
	{
		const int olderrno = errno;
                close(a[1]);
                errno = olderrno;
                err(8, "Error in waiting");
	}
	exit(0);
}
