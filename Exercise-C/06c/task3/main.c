#include <stdlib.h>
#include <stdio.h>
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
		errx(1, "Expect one name of a file");
	}

	struct stat st;
	if (stat(argv[1], &st) < 0)
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

	int rows = 0;
	int words = 0;
	int characters = 0;

	char c;
	ssize_t read_size = -1;

	while((read_size=read(fd, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(6, "Cannot read correctly file '%s'", argv[1]);
		}

		if (c == '\n')
		{
			rows++;
			words++;
		}
		if (c == ' ')
		{
			words++;
		}
		characters++;
	}
	
	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(7, "Error in reading file '%s'", argv[1]);
	}

	printf(" %d  %d %d %s\n", rows, words, characters, argv[1]);
	
	exit(0);
}
