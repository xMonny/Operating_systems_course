#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Invalid number of arguments");
	}

	char* dir = argv[1];

	struct stat st;
	if (stat(dir, &st) < 0)
	{
		err(2, "Cannot stat %s", argv[1]);
	}
	if (!S_ISDIR(st.st_mode))
	{
		errx(3, "Input argument should be a directory");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "Directory %s cannot be read", dir);
	}

	int a[2];
	if (pipe(a) < 0)
	{
		err(5, "Cannot make a pipe");
	}

	pid_t one = fork();
	if (one < 0)
	{
		int olderrno = errno;
		close(a[0]);
		close(a[1]);
		errno = olderrno;
		err(6, "Cannot make fork");
	}

	if (one == 0)
	{
		close(a[0]);
		if (dup2(a[1], 1) < 0)
		{
			int olderrno = errno;
			close(a[1]);
			errno = olderrno;
			err(7, "Cannot make a duplicate");
		}

		if (execlp("find", "find", dir, "-type", "f", "-printf", "%T@ %f\n", (char*)NULL) < 0)
		{
			int olderrno = errno;
                        close(a[1]);
                        errno = olderrno;
                        err(8, "Cannot execute command find");
		}
	}
	close(a[1]);

	int b[2];
	if (pipe(b) < 0)
	{
		int olderrno = errno;
		close(a[0]);
		errno = olderrno;
		err(9, "Cannot make a pipe");
	}

	pid_t two = fork();
	if (two < 0)
	{
		int olderrno = errno;
                close(a[0]);
		close(b[0]);
		close(b[1]);
                errno = olderrno;
                err(10, "Cannot make fork");
	}

	if (two == 0)
	{
		close(b[0]);

		if (dup2(a[0], 0) < 0)
		{
			int olderrno = errno;
			close(a[0]);
			close(b[1]);
			errno = olderrno;
			err(11, "Cannot make a duplicate");
		}

		if (dup2(b[1], 1) < 0)
                {
                        int olderrno = errno;
                        close(a[0]);
                        close(b[1]);
                        errno = olderrno;
                        err(12, "Cannot make a duplicate");
                }

		if (execlp("sort", "sort", "-n", (char*)NULL) < 0)
		{
                        int olderrno = errno;
                        close(a[0]);
                        close(b[1]);
                        errno = olderrno;
                        err(13, "Cannot execute command sort");
		}
	}
	close(a[0]);
	close(b[1]);

	int c[2];
	if (pipe(c) < 0)
	{
		int olderrno = errno;
		close(b[0]);
		errno = olderrno;
		err(14, "Cannot make a pipe");
	}

	pid_t three = fork();
	if (three < 0)
	{
		int olderrno = errno;
                close(b[0]);
                close(c[0]);
                errno = olderrno;
                err(15, "Cannot make fork");
	}

	if (three == 0)
	{
		close(c[0]);
		
		if (dup2(b[0], 0) < 0)
                {
                        int olderrno = errno;
                        close(b[0]);
                        close(c[1]);
                        errno = olderrno;
                        err(16, "Cannot make a duplicate");
                }

                if (dup2(c[1], 1) < 0)
                {
                        int olderrno = errno;
                        close(b[0]);
                        close(c[1]);
                        errno = olderrno;
                        err(17, "Cannot make a duplicate");
                }

                if (execlp("tail", "tail", "-n1", (char*)NULL) < 0)
                {
                        int olderrno = errno;
                        close(b[0]);
                        close(c[1]);
                        errno = olderrno;
			err(18, "Cannot execute command tail");
		} 		
	}

	close(b[0]);
	close(c[1]);

	if (dup2(c[0], 0) < 0)
	{
		int olderrno = errno;
		close(c[0]);
		errno = olderrno;
		err(19, "Cannot make a duplicate");
	}

	if (execlp("cut", "cut", "-d ", "-f2", (char*)NULL) < 0)
	{
		int olderrno = errno;
		close(c[0]);
		errno = olderrno;
		err(20, "Cannot execute command cut");
	}
}
