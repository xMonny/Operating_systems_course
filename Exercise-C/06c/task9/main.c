#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main (int argc, char* argv[])
{
	if (argc != 3)
	{
		errx(1, "Invalid number of arguments");
	}

	int is_min = strcmp(argv[1], "--min");
	int is_max = strcmp(argv[1], "--max");
	int is_print = strcmp(argv[1], "--print");

	if (is_min != 0 && is_max != 0 && is_print != 0)
	{
		errx(2, "Incorrect first argument: It should be --min, --max or --print");
	}

	struct stat st;
	if (stat(argv[2], &st) < 0)
	{
		err(3, "Cannot stat file %s", argv[2]);
	}

	if (!S_ISREG(st.st_mode))
	{
		errx(4, "Input second argument should be a file");
	}
	
	ssize_t file_size = st.st_size;

	if (file_size % sizeof(uint16_t) != 0)
	{
		errx(5, "Error in file %s format", argv[2]);
	} 

	int fd;
	fd = open(argv[2], O_RDONLY);
	if (fd < 0)
	{
		err(6, "Cannot open file %s", argv[2]);
	}

	uint16_t minimal = 65535;
	uint16_t maximal = 0;
	uint16_t c;
	ssize_t read_size;

	while ((read_size = read(fd, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(7, "Error in reading file %s", argv[2]);
		}

		if (c < minimal)
		{
			minimal = c;
		}
		if (c > maximal)
		{
			maximal = c;
		}

		if (is_print == 0)
		{
			printf("%d\n", c);
		}
	}

	if (is_min == 0)
	{
		printf("Min: %d\n", minimal);
	}
	else if(is_max == 0)
	{
		printf("Max: %d\n", maximal);
	}

	close(fd);
	exit(0);
}
