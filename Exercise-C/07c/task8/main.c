#include <stdlib.h>
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
	if (argc != 2)
	{
		errx(1, "Expect one name of a file");
	}

	const char* str = "foobar\n";
	
	int fd = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0644);
	if (fd < 0)
	{
		err(2, "Cannot open file %s", argv[1]);
	}

	pid_t p = fork();
	if (p < 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(2, "Cannot fork");
	}
	if (p == 0)
	{
		ssize_t write_size;
		write_size = write(fd, str, strlen(str));
		if (write_size == -1)
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(3, "Error in writing in file %s", argv[1]);
		}
		if (write_size != (ssize_t)strlen(str))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(4, "Cannot write correctly in file %s", argv[1]);
		}
		exit(0);
	}

	int status;
	pid_t wait_code = wait(&status);
	if (wait_code < 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(5, "Error in waiting");
	}
	if (!WIFEXITED(status))
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(6, "Child process didn't terminate normally");
	}
	if (WEXITSTATUS(status) != 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(7, "Exit status of child process is not 0");
	}

	off_t check = lseek(fd, 0, SEEK_SET);
	if (check < 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(8, "Error in positioning in file %s", argv[1]);
	}

	int cnt = 1;
	char c;
	ssize_t read_size = -1;
	while ((read_size=read(fd, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(9, "Cannot read correctly from file %s", argv[1]);
		}
	
		ssize_t write_size;
		write_size = write(1, &c, read_size);
		if (write_size == -1)
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(12, "Error in writing on stdout");
		}
		if (write_size != read_size)
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(13, "Cannot write correctly on stdout");
		}

		if (cnt%2 == 0)
		{
			char space = ' ';
			ssize_t write_space = write(1, &space, sizeof(space));
			if (write_space == -1)
			{
				const int olderrno = errno;
				close(fd);
				errno = olderrno;
				err(10, "Error in writing on stdout");
			}
			if (write_space != sizeof(c))
			{
				const int olderrno = errno;
				close(fd);
				errno = olderrno;
				errx(11, "Cannot write correctly on stdout");
			}
		}
		cnt++;
	}

	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(14, "Error in reading from file %s", argv[1]);
	}
	close(fd);

	exit(0);
}
