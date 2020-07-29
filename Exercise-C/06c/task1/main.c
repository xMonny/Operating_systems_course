#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		errx(1, "Invalid number of arguments");
	}

	const char* original = argv[1];
	const char* copy = argv[2];

	struct stat st;
	if (stat(original, &st) < 0)
	{
		err(2, "Cannot stat file %s", original);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "First argument should be a regular file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File %s cannot be read", original);
	}
	off_t original_size = st.st_size;

	void* buff = malloc(original_size);
	if (!buff)
	{
		err(5, "No memory");
	}

	int fd_original;
	int fd_copy;

	fd_original = open(original, O_RDONLY);
	if (fd_original < 0)
	{
		free(buff);
		err(6, "Cannot open file %s", original);
	}

	fd_copy = open(copy, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd_copy < 0)
	{
		const int olderrno = errno;
		close(fd_original);
		free(buff);
		errno = olderrno;
		err(7, "Cannot open file %s", copy);
	}

	ssize_t read_size = read(fd_original, buff, original_size);
	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd_original);
		close(fd_copy);
		free(buff);
		errno = olderrno;
		err(8, "Error in reading file %s", original);
	}
	if (read_size != original_size)
	{
		const int olderrno = errno;
		close(fd_original);
		close(fd_copy);
		free(buff);
		errno = olderrno;
		errx(9, "Cannot read correctly file %s", original);
	}

	close(fd_original);

	ssize_t write_size;
	write_size = write(fd_copy, buff, read_size);
	if (write_size == -1)
	{
		const int olderrno = errno;
		close(fd_copy);
		free(buff);
		errno = olderrno;
		err(10, "Error in writing in file %s", copy);
	}
	if (write_size != read_size)
	{
		const int olderrno = errno;
		close(fd_copy);
		free(buff);
		errno = olderrno;
		errx(11, "Cannot write correctly in file %s", copy);
	}

	free(buff);
	close(fd_copy);

	exit(0);
}
