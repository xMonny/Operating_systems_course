#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main()
{
	const char* f1 = "f1";
	const char* f2 = "f2";
	const char* f3 = "f3";

	struct stat st;
	if (stat(f1, &st) < 0)
	{
		err(2, "Cannot stat file '%s'", f1);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "First argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File '%s' cannot be read", f1);
	}
	off_t f1_size = st.st_size;

	struct pair_t
	{
		uint32_t start;
		uint32_t length;
	}__attribute__((packed)); //attribute packed is not necessary here

	if (f1_size % sizeof(struct pair_t) != 0)
	{
		errx(5, "Error in format of file '%s'", f1);
	}

	if (stat(f2, &st) < 0)
	{
		err(5, "Cannot stat file '%s'", f2);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(6, "Second argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(7, "File '%s' cannot be read", f2);
	}
	off_t f2_size = st.st_size;

	if (f2_size % sizeof(uint32_t) != 0)
	{
		errx(8, "Error in format of file '%s'", f2);
	}

	if (f2_size > 0xFFFFFFFF) //UINT32_MAX
	{
		warnx("Size of file '%s' is more than acceptable", f2);
	}

	int fd_1;
	int fd_2;
	int fd_3;

	fd_1 = open(f1, O_RDONLY);
	if (fd_1 < 0)
	{
		err(9, "Cannot open file '%s'", f1);
	}

	fd_2 = open(f2, O_RDONLY);
	if (fd_2 < 0)
	{
		const int olderrno = errno;
		close(fd_1);
		errno = olderrno;
		err(10, "Cannot open file '%s'", f2);
	}

	fd_3 = open(f3, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd_3 < 0)
	{
		const int olderrno = errno;
		close(fd_1);
		close(fd_2);
		errno = olderrno;
		err(11, "Cannot open file '%s'", f3);
	}

	struct pair_t pair;
	ssize_t read_f1 = -1;

	while((read_f1=read(fd_1, &pair, sizeof(pair))) > 0)
	{
		if (read_f1 != sizeof(pair))
		{
			const int olderrno = errno;
			close(fd_1);
			close(fd_2);
			close(fd_3);
			errno = olderrno;
			errx(12, "Cannot read correctly file '%s'", f1);
		}

		uint32_t position = pair.start - 1;

		off_t check = lseek(fd_2, position*sizeof(uint32_t), SEEK_SET);
		if (check < 0)
		{
			const int olderrno = errno;
			close(fd_1);
			close(fd_2);
			close(fd_3);
			errno = olderrno;
			err(14, "Error in positioning in file '%s'", f2);
		}

		uint32_t len = 0;
		uint32_t current;
		ssize_t read_f2;
		while((read_f2=read(fd_2, &current, sizeof(current))) > 0)
		{
			if (read_f2 != sizeof(current))
			{
				const int olderrno = errno;
				close(fd_1);
				close(fd_2);
				close(fd_3);
				errno = olderrno;
				errx(15, "Cannot read correctly file '%s'", f2);
			}

			ssize_t write_f3;
			write_f3 = write(fd_3, &current, read_f2);
			if (write_f3 == -1)
			{
				const int olderrno = errno;
				close(fd_1);
				close(fd_2);
				close(fd_3);
				errno = olderrno;
				err(16, "Error in writing in file '%s'", f3);
			}
			if (write_f3 != read_f2)
			{
				const int olderrno = errno;
				close(fd_1);
				close(fd_2);
				close(fd_3);
				errno = olderrno;
				errx(17, "Cannot write correctly in file '%s'", f3);
			}
			len++;
			if (len == pair.length)
			{
				break;
			}
		}
		if (read_f2 == -1)
		{
			const int olderrno = errno;
			close(fd_1);
			close(fd_2);
			close(fd_3);
			errno = olderrno;
			err(18, "Error in reading file '%s'", f2);
		}
	}

	if (read_f1 == -1)
	{
		const int olderrno = errno;
		close(fd_1);
		close(fd_2);
		close(fd_3);
		errno = olderrno;
		err(19, "Error in reading file '%s'", f1);
	}

	close(fd_1);
	close(fd_2);
	close(fd_3);

	exit(0);
}
