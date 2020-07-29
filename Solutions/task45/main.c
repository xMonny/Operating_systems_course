#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>

int main()
{
	int a[2];
	if (pipe(a) < 0)
	{
		err(1, "Cannot make a pipe");
	}

	pid_t one = fork();
	if (one < 0)
	{
		const int olderrno = errno;
		close(a[0]);
		close(a[1]);
		errno = olderrno;
		err(2, "Cannot fork");
	}

	if (one == 0)
	{
		close(a[0]);
		if (dup2(a[1], 1) < 0)
		{
			const int olderrno = errno;
			close(a[1]);
			errno = olderrno;
			err(3, "Cannot make a duplicate");
		}
		if (execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*)NULL) < 0)
		{
			const int olderrno = errno;
			close(a[1]);
			errno = olderrno;
			err(4, "Cannot execute command cut");
		}
	}
	close(a[1]);

	int b[2];
	if (pipe(b) < 0)
	{
		const int olderrno = errno;
		close(a[0]);
		errno = olderrno;
		err(5, "Cannot make a pipe");
	}

	pid_t two = fork();
	if (two < 0)
	{
		const int olderrno = errno;
		close(a[0]);
		close(b[0]);
		close(b[1]);
		errno = olderrno;
		err(6, "Cannot fork");
	}

	if (two == 0)
	{
		close(b[0]);
		if (dup2(a[0], 0) < 0)
		{
			const int olderrno = errno;
			close(a[0]);
			close(b[1]);
			errno = olderrno;
			err(7, "Cannot make a duplicate");
		}
		if (dup2(b[1], 1) < 0)
		{
			const int olderrno = errno;
			close(a[0]);
			close(b[1]);
			errno = olderrno;
			err(8, "Cannot make a duplicate");
		}
		if (execlp("sort", "sort", "-n", (char*)NULL) < 0)
		{
			const int olderrno = errno;
			close(a[0]);
			close(b[1]);
			errno = olderrno;
			err(9, "Cannot execute command sort");
		}
	}
	close(a[0]);
	close(b[1]);

	int c[2];
	if (pipe(c) < 0)
	{
		const int olderrno = errno;
		close(b[0]);
		errno = olderrno;
		err(10, "Cannot make a pipe");
	}

	pid_t three = fork();
	if (three < 0)
	{
		const int olderrno = errno;
		close(b[0]);
		close(c[0]);
		close(c[1]);
		errno = olderrno;
		err(11, "Cannot fork");
	}

	if (three == 0)
	{
		close(c[0]);
	
		if (dup2(b[0], 0) < 0)
		{
			const int olderrno = errno;
			close(b[0]);
			close(c[1]);
			errno = olderrno;
			err(12, "Cannot make a duplicate");
		}
		if (dup2(c[1], 1) < 0)
		{
			const int olderrno = errno;
                        close(b[0]);
                        close(c[1]);
                        errno = olderrno;
                        err(13, "Cannot make a duplicate");
		}
		if (execlp("uniq", "uniq", "-c", (char*)NULL) < 0)
		{
			const int olderrno = errno;
			close(b[0]);
			close(c[1]);
			errno = olderrno;
			err(14, "Cannot execute command uniq");
		}
	}
	close(b[0]);
	close(c[1]);

	if (dup2(c[0], 0) < 0)
	{
		const int olderrno = errno;
		close(c[0]);
		errno = olderrno;
		err(15, "Cannot make a duplicate");
	}
	if (execlp("sort", "sort", "-n", "-k1", (char*)NULL) < 0)
	{
		const int olderrno = errno;
                close(c[0]);
                errno = olderrno;
                err(16, "Cannot execute command sort");
	}	
}
