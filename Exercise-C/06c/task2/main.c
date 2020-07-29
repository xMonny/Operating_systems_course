#include <stdlib.h>
#include <stdint.h>
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

	struct stat st;
	if (stat(argv[1], &st) == -1)
	{
		err(2, "Cannot stat file %s", argv[1]);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "Input argument should be a regular file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File '%s' cannot be read", argv[1]);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		err(5, "Cannot open file '%s'", argv[1]);
	}

	char c;
	ssize_t read_size = -1;

	uint8_t rows = 0;

	while((read_size=read(fd, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(7, "Cannot read correctly file '%s'", argv[1]);
		}

		ssize_t write_size;
		write_size = write(1, &c, read_size);
		if (write_size == -1)
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(8, "Error in writing to stdout");
		}
		if (write_size != read_size)
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(9, "Cannot write correctly to stdout");
		}

		if (c == '\n')
		{
			rows++;
		}
		if (rows == 10)
		{
			break;
		}
	}

	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(6, "Error in reading file '%s'", argv[1]);
	}
	
	exit(0);
}
