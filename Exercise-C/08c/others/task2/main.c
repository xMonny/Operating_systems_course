//  cat /etc/passwd | cut -d ':' -f 7 | sort | uniq
//  cut -d ':' -f 7 /etc/passwd | sort | uniq

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>

int main()
{
	int a[2];
	if (pipe(a) < 0)
	{
		err(1, "Cannot make a pipe");
	}

	pid_t p = fork();
	if (p < 0)
	{
		int olderrno = errno;
		close(a[0]);
		close(a[1]);
		errno = olderrno;
		err(2, "Cannot make fork");
	}

	if (p == 0)
	{
		close(a[0]);
		if (dup2(a[1], 1) < 0)
		{
			int olderrno = errno;
			close(a[1]);
			errno = olderrno;
			err(3, "Error in duplicate");
		}

		if (execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*) NULL) < 0)
		{
			int olderrno = errno;
			close(a[1]);
			errno = olderrno;
			err(4, "Cannot execute command cut");
		}
	}

	close(a[1]);
	int b[2];
	if (pipe(b) < 0)
	{
		int olderrno = errno;
		close(a[0]);
		errno = olderrno;
		err(5, "Cannot make a pipe");
	}

	pid_t f = fork();
	if (f < 0)
	{
		int olderrno = errno;
                close(a[0]);
		close(b[0]);
		close(b[1]);
                errno = olderrno;
                err(6, "Cannot make fork");
	}

	if (f == 0)
	{
		close(b[0]);
		if (dup2(a[0], 0) < 0)
		{
			int olderrno = errno;
			close(a[0]);
			close(b[1]);
			errno = olderrno;
			err(7, "Error in duplicate");
		}

		if (dup2(b[1], 1) < 0)
		{
			int olderrno = errno;
                        close(a[0]);
                        close(b[1]);
                        errno = olderrno;
                        err(8, "Error in duplicate");
		}

		if (execlp("sort", "sort", (char*)NULL) < 0)
		{
			int olderrno = errno;
                        close(a[0]);
                        close(b[1]);
                        errno = olderrno;
                        err(9, "Cannot execute command cut");
		}
	}

	close(a[0]);
	close(b[1]);

	if (dup2(b[0], 0) < 0)
	{
		int olderrno = errno;
		close(b[0]);
		errno = olderrno;
		err(10, "Error in duplicate");
	}

	if (execlp("uniq", "uniq", (char*)NULL) < 0)
	{
		int olderrno = errno;
                close(b[0]);
                errno = olderrno;
                err(11, "Cannot execute command uniq");
	}

	exit(0);
}
