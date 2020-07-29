#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

void swap(const char* one, const char* two, off_t one_size, off_t two_size)
{
	//allocate a buffer for first file
	void* buff_one = malloc(one_size);
	if (!buff_one)
	{
		err(10, "No memory");
	}

	int fd_one;
	int fd_two;

	//open first file in read-only mode
	fd_one = open(one, O_RDONLY);
	if (fd_one < 0)
	{
		free(buff_one);
		err(11, "Cannot open file %s", one);
	}

	//read from first file and write the content in a buffer
	ssize_t read_one;
	read_one = read(fd_one, buff_one, one_size);
	if (read_one == -1)
	{
		const int olderrno = errno;
		free(buff_one);
		close(fd_one);
		errno = olderrno;
		err(12, "Error in reading file %s", one);
	}
	if (read_one != one_size)
	{
		const int olderrno = errno;
                free(buff_one);
                close(fd_one);
                errno = olderrno;
                errx(13, "Cannot read correctly file %s", one);
	}

	//close first file for a moment
	close(fd_one);

	//allocate a buffer for second file
	void* buff_two = malloc(two_size);
	if (!buff_two)
	{
		free(buff_one);
		err(14, "No memory");
	}

	//open second file in read-only mode
	fd_two = open(two, O_RDONLY);
	if (fd_two < 0)
	{
		free(buff_one);
		free(buff_two);
		err(14, "Cannot open file %s", two);
	}

	//read the second file and write the content in a buffer
	ssize_t read_two;
	read_two = read(fd_two, buff_two, two_size);
	if (read_two == -1)
	{
		const int olderrno = errno;
		free(buff_one);
		free(buff_two);
		close(fd_two);
		errno = olderrno;
		err(15, "Error in reading file %s", two);
	}
	if (read_two != two_size)
	{
		const int olderrno = errno;
                free(buff_one);
                free(buff_two);
                close(fd_two);
                errno = olderrno;
                errx(16, "Cannot read correctly file %s", two);
	}

	//close second file for a moment
	close(fd_two);

	//open first file again in write-only mode and truncate its content
	fd_one = open(one, O_TRUNC | O_WRONLY);
	if (fd_one < 0)
	{
		free(buff_one);
		free(buff_two);
		err(17, "Cannot open file %s", one);
	}

	//write in first file the content from second file which is in a buffer
	ssize_t write_one;
	write_one = write(fd_one, buff_two, read_two);
	if (write_one == -1)
	{
		const int olderrno = errno;
                free(buff_one);
                free(buff_two);
                close(fd_one);
                errno = olderrno;
                err(18, "Error in writing in file %s", one);
	}
	if (write_one != read_two)
	{
		const int olderrno = errno;
                free(buff_one);
                free(buff_two);
                close(fd_one);
                errno = olderrno;
                errx(19, "Cannot write correctly in file %s", one);
	}

	//we are ready with writing the content from second to first file
	free(buff_two);
	close(fd_one);

	//open second file again in write-only mode and truncate its content
	fd_two = open(two, O_TRUNC | O_WRONLY);
	if (fd_two < 0)
	{
		free(buff_one);
		err(20, "Cannot open file %s", two);
	}

	//write in second file the content from first file which is in a buffer
	ssize_t write_two;
	write_two = write(fd_two, buff_one, read_one);
	if (write_two == -1)
	{
		const int olderrno = errno;
                free(buff_one);
                close(fd_two);
                errno = olderrno;
                err(21, "Error in writing in file %s", two);
	}
	if (write_two != read_one)
	{
		const int olderrno = errno;
                free(buff_one);
                close(fd_two);
                errno = olderrno;
                errx(22, "Cannot write correctly in file %s", two);
	}

	//we are ready with writing the content from first to second file
	free(buff_one);
	close(fd_two);
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		errx(1, "Invalid number of arguments");
	}

	const char* one = argv[1];
	const char* two = argv[2];

	if (strcmp(one, two) == 0)
	{
		printf("Will not swap: Input files are same\n");
		exit(0);
	}

	struct stat st;
	if (stat(one, &st) < 0)
	{
		err(2, "Cannot stat file %s", one);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "First argument should be a regular file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File %s cannot be read", one);
	}
	if (!(st.st_mode & S_IWUSR))
	{
		errx(5, "Cannot write in file %s", one);
	}
	off_t one_size = st.st_size;

	if (stat(two, &st) < 0)
        {
                err(6, "Cannot stat file %s", two);
        }
        if (!S_ISREG(st.st_mode))
        {
                errx(7, "Second argument should be a regular file");
        }
        if (!(st.st_mode & S_IRUSR))
        {
                errx(8, "File %s cannot be read", two);
        }
        if (!(st.st_mode & S_IWUSR))
        {
                errx(9, "Cannot write in file %s", two);
        }
	off_t two_size = st.st_size;

	swap(one, two, one_size, two_size);

	exit(0);
}
