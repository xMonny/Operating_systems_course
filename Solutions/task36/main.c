#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

bool flag_num = false;
bool isNewLine = true;
int row = 1;

void read_from (int fd)
{
	char c;
	ssize_t read_size;
	while ((read_size=read(fd, &c, sizeof(c))) > 0)
	{
		if (flag_num && isNewLine)
		{
			char buf[sizeof(row)+5];
			sprintf(buf, "  %d ", row);
			write(1, buf, sizeof(buf));
			isNewLine = false;
		}
		if (write(1, &c, read_size) != read_size)
		{
			int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(2, "Error in reading file");
		}
		if (flag_num && c == '\n')
		{
			row++;
			isNewLine = true;
		}
	}
} 

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		read_from(0);
		exit(0);
	}
	
	int i = 1;
	if (strcmp(argv[1], "-n") == 0)
	{
		flag_num = true;
		i = 2;
	}
	if (argc == 2 && flag_num)
	{
		read_from(0);
		exit(0);
	}
	
	for (; i < argc; i++)
	{
		if (strcmp(argv[i], "-") == 0)
		{
			read_from(0);
			continue;
		}
		int f = open(argv[i], O_RDONLY);
		if (f < 0)
		{
			warn("Cannot open file %s", argv[i]);
			continue;
		}

		read_from(f);

		close(f);
	}

	exit(0);
}
