#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int comparator (const void* a, const void* b)
{
	return ( (*(uint16_t*)a) - (*(uint16_t*)b) );
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		errx(1, "Invalid number of arguments");
	}

	const char* input = argv[1];
	const char* output = argv[2];
	
	struct stat st;

	if (stat(input, &st) < 0)
	{
		err(2, "Cannot stat file %s", input);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "First argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File %s cannot be read", input);
	}
	off_t input_size = st.st_size;

	if (input_size % sizeof(uint16_t) != 0)
	{
		errx(5, "Error in format of file %s", input);
	}

	uint16_t numbers = input_size/sizeof(uint16_t);

	uint16_t *buff = malloc(input_size);
	if (!buff)
	{
		err(6, "No free memory");
	}

	int fd_i = open(input, O_RDONLY);
	if (fd_i < 0)
	{	
		free(buff);
		err(7, "Cannot open file %s", input);
	}

	ssize_t read_size;
	read_size = read(fd_i, buff, input_size);
	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd_i);
		free(buff);
		errno = olderrno;
		err(8, "Error in reading from file %s", input);
	}
	if (read_size != input_size)
	{
		const int olderrno = errno;
                close(fd_i);
                free(buff);
                errno = olderrno;
                errx(9, "Cannot read correctly from file %s", input);
	}

	close(fd_i);
	
	qsort(buff, numbers, sizeof(uint16_t), comparator);

	int fd_o = open(output, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd_o < 0)
	{
		free(buff);
		err(10, "Cannot open file %s", output);
	}

	ssize_t write_size;
	write_size = write(fd_o, buff, read_size);
	
	if (write_size == -1)
        {
                const int olderrno = errno;
                close(fd_o);
                free(buff);
                errno = olderrno;
                err(11, "Error in writing in file %s", output);
        }
        if (write_size != read_size)
        {
                const int olderrno = errno;
                close(fd_o);
                free(buff);
                errno = olderrno;
                errx(12, "Cannot write correctly in file %s", output);
        }

	free(buff);
	close(fd_o);

	exit(0);
}
