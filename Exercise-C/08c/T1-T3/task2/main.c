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
		errx(1, "Expect one string as argument");
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

		char c;
		ssize_t read_size;
		while((read_size=read(a[0], &c, sizeof(c))) > 0)
		{
			if (read_size != sizeof(c))
			{
				const int olderrno = errno;
                        	close(a[0]);
                        	errno = olderrno;
                        	errx(5, "Cannot read correctly from pipe");
			}
			ssize_t write_size;
			write_size = write(1, &c, read_size);
			if (write_size == -1)
			{
				const int olderrno = errno;
                                close(a[0]);
                                errno = olderrno;
                                err(6, "Error in writing to stdout");
			}
			if (write_size != read_size)
			{
				const int olderrno = errno;
                                close(a[0]);
                                errno = olderrno;
                                errx(7, "Cannot write correctly to stdout");
			}
		}
	
		if (read_size == -1)
		{
			const int olderrno = errno;
                        close(a[0]);
                        errno = olderrno;
                        err(8, "Error in reading from pipe");
		}

		close(a[0]);
		exit(0);
	}
	close(a[0]);
	
	ssize_t write_size;
	write_size = write(a[1], argv[1], strlen(argv[1]));
	if (write_size == -1)
	{
		const int olderrno = errno;
                close(a[1]);
                errno = olderrno;
                err(10, "Error in writing in pipe");
	}
	if (write_size != (ssize_t)strlen(argv[1]))
	{
		const int olderrno = errno;
                close(a[1]);
                errno = olderrno;
                errx(11, "Cannot write correctly in pipe");
	}	

	close(a[1]);

	if (wait(NULL) < 0)
	{
		err(12, "Error in waiting");
	}

	exit(0);
}
