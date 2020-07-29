#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Invalid number of arguments");
	}

	int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd < 0)
	{
		err(2, "Cannot open file %s", argv[1]);
	}

	const char* str1 = "foo\n";
	const char* str2 = "bar\n";

	ssize_t write_str1;
	write_str1 = write(fd, str1, 2);
	if (write_str1 == -1)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(3, "Error in writing in file %s", argv[1]);
	}
	if (write_str1 != 2)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(4, "Cannot write correctly in file %s", argv[1]);
	}

	pid_t p = fork();
	if (p < 0)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(5, "Cannot make fork");
	}
	if (p == 0)
	{
		ssize_t write_str2;
		write_str2 = write(fd, str2, 4);
		if (write_str2 == -1)
		{
			int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(6, "Error in writing in file %s", argv[1]);
		}
		if (write_str2 != 4)
		{
			int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(7, "Cannot write correctly in file %s", argv[1]);
		}

		exit(0);
	}

	int status;
	pid_t wait_pid = wait(&status);
	if (wait_pid < 0)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(8, "Error in waiting");
	}

	if (!WIFEXITED(status))
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(9, "Child didn't terminate normally");
	}
	if (WEXITSTATUS(status) != 0)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(10, "Exit status of child process is not 0");
	}

	write_str1 = write(fd, str1+2, 2);
	if (write_str1 == -1)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(11, "Error in writing in file %s", argv[1]);
	}
	if (write_str1 != 2)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(12, "Cannot write correctly in file %s", argv[1]);
	}

	close(fd);

	exit(0);
}
