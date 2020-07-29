#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int comparator(const void *a, const void *b)
{
	//return *((unsigned char*)a) - *((unsigned char*)b);

	if ( *((unsigned char*)a) > *((unsigned char*)b) )
	{
		return 1;
	}
	if ( *((unsigned char*)a) < *((unsigned char*)b) )
	{
		return -1;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Invalid number of arguments");
	}

	struct stat st;
	if (stat(argv[1], &st) < 0)
	{
		err(1, "Cannot stat file %s", argv[1]);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(2, "Input argument should be a file");
	}
	off_t f_size = st.st_size;

	void *buff = malloc(f_size);
	if (!buff)
	{
		err(3, "No memory");
	}

	int input;
	input = open(argv[1], O_RDONLY);
	if (input < 0)
	{
		free(buff);
		err(4, "Cannot open file %s", argv[1]);
	}

	ssize_t read_size;
	read_size = read(input, buff, f_size);
	
	close(input);

	if (read_size == -1)
	{
		int olderrno = errno;
		free(buff);
		errno = olderrno;
		err(5, "Error in reading file %s", argv[1]);
	}

	if (read_size != f_size)
	{
		int olderrno = errno;
		free(buff);
		errno = olderrno;
		errx(6, "Cannot read all content of file %s", argv[1]);
	}

	qsort(buff, f_size, 1 , comparator);

	int output;
	output = open("sorted", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (output < 0)
	{
		free(buff);
		err(7, "Cannot open file sorted");
	}

	ssize_t write_size;
	write_size = write(output, buff, read_size);
	
	close(output);
	free(buff);

	if (write_size == -1)
	{
		err(8, "Error in writing in file sorted");
	}

	if (write_size != read_size)
	{
		errx(9, "Cannot write all content from file %s to file sorted", argv[1]);
	}

	exit(0);
}
