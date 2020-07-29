#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

const char* input;
const char* output;

int comparator (const void* a, const void* b)
{
	if ( *((uint32_t*)a) > *((uint32_t*)b) )
	{
		return 1;
	}
	if ( *((uint32_t*)a) < *((uint32_t*)b) )
	{
		return -1;
	}
	return 0;

	//return (*(uint32_t*)a - *(uint32_t*)b);

	//return *((uint32_t*)a) - *((uint32_t*)b);
}

void merge(int fd_temp1, int fd_temp2, int fd_output)
{
	uint32_t a;
	uint32_t b;
	
	ssize_t read_a = -1;
	ssize_t read_b = -1;

	ssize_t write_a;
	ssize_t write_b;

	while ( (read_a=read(fd_temp1, &a, sizeof(a))) > 0 && (read_b=read(fd_temp2, &b, sizeof(b))) > 0)
	{
		if (read_a != sizeof(a))
		{
			const int olderrno = errno;
			close(fd_output);
			unlink("temp1");
			unlink("temp2");
			errno = olderrno;
			errx(17, "Cannot read correctly numbers from first temporary file");
		}
		if (read_b != sizeof(b))
                {
                        const int olderrno = errno;
                        close(fd_output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(18, "Cannot read correctly numbers from second temporary file");
                }

		if (a <= b)
		{
			write_a = write(fd_output, &a, read_a);
			if (write_a == -1)
			{
				const int olderrno = errno;
				close(fd_output);
				unlink("temp1");
				unlink("temp2");
				errno = olderrno;
				err(19, "Error in writing first sorted half in file %s", output);
			}
			if (write_a != read_a)
			{
                                const int olderrno = errno;
                                close(fd_output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                errx(20, "Cannot write correctly first sorted half in file %s", output);
                        }

			off_t check = lseek(fd_temp2, -1*sizeof(b), SEEK_CUR);
			if (check < 0)
			{
                                const int olderrno = errno;
                                close(fd_output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                err(20, "Error in positioning in second temporary file");
			}
		}
		else
		{
			write_b = write(fd_output, &b, read_b);
                        if (write_b == -1)
                        {
                                const int olderrno = errno;
                                close(fd_output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                err(21, "Error in writing second sorted half in file %s", output);
                        }
                        if (write_b != read_b)
                        {
                                const int olderrno = errno;
                                close(fd_output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                errx(22, "Cannot write correctly second sorted half in file %s", output);
                        }

			off_t check = lseek(fd_temp1, -1*sizeof(a), SEEK_CUR);
                        if (check < 0)
                        {
                                const int olderrno = errno;
                                close(fd_output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                err(23, "Error in positioning in first temporary file");
                        }
		}
	}

	if (read_a == -1)
	{
		const int olderrno = errno;
		close(fd_output);
		unlink("temp1");
		unlink("temp2");
		errno = olderrno;
		err(24, "Error in reading first temporary file");
	}

	if (read_b == -1)
	{
		const int olderrno = errno;
                close(fd_output);
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(25, "Error in reading second temporary file");
	}

	while ((read_a=read(fd_temp1, &a, sizeof(a))) > 0)
	{
		if (read_a != sizeof(a))
		{
			const int olderrno = errno;
			close(fd_output);
			unlink("temp1");
			unlink("temp2");
			errno = olderrno;
			errx(26, "Cannot read correctly numbers from first temporary file");
		}

		write_a = write(fd_output, &a, read_a);
		if (write_a == -1)
		{
			const int olderrno = errno;
                        close(fd_output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        err(27, "Error in reading numbers from first temporary file");
		}
		if (write_a != read_a)
		{
			const int olderrno = errno;
                        close(fd_output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(28, "Error in reading numbers from first temporary file");
		}
	}

	if (read_a == -1)
        {
                const int olderrno = errno;
                close(fd_output);
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(29, "Error in reading first temporary file");
        }

	while ((read_b=read(fd_temp2, &b, sizeof(b))) > 0)
	{
		if (read_b != sizeof(b))
                {
                        const int olderrno = errno;
                        close(fd_output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(30, "Cannot read correctly numbers from second temporary file");
                }

                write_b = write(fd_output, &b, read_b);
                if (write_b == -1)
                {
                        const int olderrno = errno;
                        close(fd_output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        err(31, "Error in reading numbers from second temporary file");
                }
		if (write_b != read_b)
                {
                        const int olderrno = errno;
                        close(fd_output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(32, "Error in reading numbers from second temporary file");
                }
	}

        if (read_b == -1)
        {
                const int olderrno = errno;
                close(fd_output);
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(33, "Error in reading second temporary file");
        }
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		errx(1, "Invalid number of arguments");
	}

	input = argv[1];
	output = argv[2];

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

	if (input_size % sizeof(uint32_t) != 0)
	{
		errx(5, "Error in format of file %s", input);
	}

	uint32_t numbers = input_size/sizeof(uint32_t);
	if (numbers > 4194304)
	{
		errx(6, "File %s can have maximum 4 194 304 numbers", input);
	}
	uint32_t numbers_half1 = numbers/2;
	uint32_t *half1 = malloc(numbers_half1*sizeof(uint32_t));
	if (!half1)
	{
		err(7, "No free memory for first half of file %s", input);
	}
	
	int fd_input = open(input, O_RDONLY);
	if (fd_input < 0)
	{
		free(half1);
		err(8, "Cannot open file %s", input);
	}

	ssize_t read_size;
	read_size = read(fd_input, half1, numbers_half1*sizeof(uint32_t));
	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd_input);
		free(half1);
		errno = olderrno;
		err(9, "Error in reading file %s", input);
	}
	if ((size_t)read_size != numbers_half1*sizeof(uint32_t))
	{
		const int olderrno = errno;
		close(fd_input);
		free(half1);
		errno = olderrno;
		errx(10, "Cannot read first half of file %s", input);
	}

	qsort(half1, numbers_half1, sizeof(uint32_t), comparator);

	int fd_temp1 = open("temp1", O_CREAT | O_RDWR | S_IRUSR | S_IWUSR);
	if (fd_temp1 < 0)
	{
		const int olderrno = errno;
                close(fd_input);
                free(half1);
                errno = olderrno;
                errx(11, "Cannot create first temporary file");
	}

	ssize_t write_size;
	write_size = write(fd_temp1, half1, read_size);
	if (write_size == -1)
	{
		const int olderrno = errno;
                close(fd_input);
		unlink("temp1");
                free(half1);
                errno = olderrno;
                err(11, "Error in writing first sorted half from file %s", input);
	}
	if (write_size != read_size)
	{
		const int olderrno = errno;
                close(fd_input);
		unlink("temp1");
                free(half1);
                errno = olderrno;
                errx(10, "Cannot write all sorted numbers of first half from file %s", input);
	}

	free(half1);

	uint32_t numbers_half2 = numbers - numbers_half1;
	uint32_t *half2 = malloc(numbers_half2*sizeof(uint32_t));
	if (!half2)
	{
		const int olderrno = errno;
                close(fd_input);
		unlink("temp1");
                errno = olderrno;
                errx(11, "No free memory for second half of file %s", input);
	}

	read_size = read(fd_input, half2, numbers_half2*sizeof(uint32_t));
	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd_input);
		unlink("temp1");
		free(half2);
		errno = olderrno;
		err(12, "Error in reading second half of file %s", input);
	}
	if ((size_t)read_size != numbers_half2*sizeof(uint32_t))
	{
		const int olderrno = errno;
                close(fd_input);
                unlink("temp1");
                free(half2);
                errno = olderrno;
                errx(12, "Cannot read all numbers of second half from file %s", input);
	}

	close(fd_input);

	qsort(half2, numbers_half2, sizeof(uint32_t), comparator);
	
	int fd_temp2 = open("temp2", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd_temp2 < 0)
	{
		const int olderrno = errno;
		unlink("temp1");
		free(half2);
		errno = olderrno;
		err(13, "Cannot create second temporary file");
	}

	write_size = write(fd_temp2, half2, read_size);
	if (write_size == -1)
	{
		const int olderrno = errno;
                unlink("temp1");
		unlink("temp2");
                free(half2);
                errno = olderrno;
                err(14, "Error in writing second sorted half of file %s", input);
	}
	if (write_size != read_size)
	{
		const int olderrno = errno;
                unlink("temp1");
		unlink("temp2");
                free(half2);
                errno = olderrno;
                errx(14, "Cannot write all numbers of sorted second half from file %s", input);
	}

	free(half2);

	off_t check;
	check = lseek(fd_temp1, 0, SEEK_SET);
	if (check < 0)
	{
		const int olderrno = errno;
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(14, "Error in positioning in first temporary file");
	}
	check = lseek(fd_temp2, 0, SEEK_SET);
	if (check < 0)
        {
                const int olderrno = errno;
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(15, "Error in positioning in second temporary file");
        }

	int fd_output = open(output, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd_output < 0)
	{
		const int olderrno = errno;
		unlink("temp1");
		unlink("temp2");
		errno = olderrno;
		err(16, "Cannot open file %s", output);
	}

	merge(fd_temp1, fd_temp2, fd_output);

	close(fd_input);
	unlink("temp1");
	unlink("temp2");

	exit(0);
}
