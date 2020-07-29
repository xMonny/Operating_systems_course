#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		errx(1, "Expect at least one argument");
	}

	struct stat st;
	for (int i = 1; i < argc; i++)
	{
		const char* current = argv[i];

		if (stat(current, &st) < 0)
		{
			warn("Cannot stat file '%s'", current);
			continue;
		}
		if (!S_ISREG(st.st_mode))
		{
			warnx("Argument '%s' should be a regular file", current);
			continue;
		}
		if (!(st.st_mode & S_IRUSR))
		{
			//warnx("File '%s' cannot be read", current);
			//continue;
			errx(10, "File '%s' cannot be read", current);
		}
		
		int fd = open(current, O_RDONLY);
		if (fd < 0)
		{
			warn("Cannot open file '%s'", current);
			continue;
		}

		char c[4096];
		ssize_t read_size;
		while((read_size=read(fd, &c, sizeof(c))) > 0)
		{
			ssize_t write_size;
			write_size = write(1, &c, read_size);
			if (write_size == -1)
			{
				const int olderrno = errno;
				close(fd);
				errno = olderrno;
				err(4, "Error in writing to stdout");
			}
			if (write_size != read_size)
			{
				const int olderrno = errno;
                                close(fd);
                                errno = olderrno;
                                errx(5, "Cannot write correctly to stdout");
			}
		}

		if (read_size == -1)
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(3, "Error in reading file '%s'", current);
		}
		close(fd);
	}

	exit(0);
}
