#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int comparator(const void* a, const void* b)
{
	return (*(uint32_t*)a) - (*(uint32_t*)b);
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
		err(2, "Cannot stat file %s", argv[1]);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "Input argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File %s cannot be read", argv[1]);
	}
	off_t f_size = st.st_size;

	if (f_size % sizeof(uint32_t) != 0)
	{
		errx(5, "File %s has not only 4B numbers", argv[1]);
	}

	uint32_t numbers = f_size/sizeof(uint32_t);
	if (numbers > 100000000)
	{
		errx(6, "File %s can have maximum 100 000 000 numbers", argv[1]);
	}
	uint32_t numbers_half1 = numbers/2;
	uint32_t *half1 = malloc(numbers_half1*sizeof(uint32_t));
	if (!half1)
	{
		err(7, "No memory for first half of file %s", argv[1]);
	}

	int fd;
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		free(half1);
		err(8, "Cannot open file %s", argv[1]);
	}

	size_t read_size;
	read_size = read(fd, half1, numbers_half1*sizeof(uint32_t));
	/*if (read_size < 0)
	{
		int olderrno = errno;
		close(fd);
		free(half1);
		errno = olderrno;
		err(9, "Error in reading first half of file %s", argv[1]);
	}*/
	if (read_size != numbers_half1*sizeof(uint32_t))
	{
		int olderrno = errno;
		close(fd);
		free(half1);
		errno = olderrno;
		errx(10, "Cannot read correctly first half of file %s", argv[1]);
	}

	qsort(half1, numbers_half1, sizeof(uint32_t), comparator);

	int temp1;
	temp1 = open("temp1", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp1 < 0)
	{
		int olderrno = errno;
		close(fd);
		free(half1);
		errno = olderrno;
		err(11, "Cannot create file temp1");
	}

	size_t write_size;
	write_size = write(temp1, half1, read_size);
	/*if (write_size < 0)
	{
		int olderrno = errno;
		close(fd);
		unlink("temp1");
		free(half1);
		errno = olderrno;
		err(12, "Error in writing in file temp1");
	}*/
	if (write_size != read_size)
	{
		int olderrno = errno;
                close(fd);
                unlink("temp1");
                free(half1);
                errno = olderrno;
                errx(13, "Cannot write correctly in file temp1");
	}

	free(half1);

	uint32_t numbers_half2 = numbers - numbers_half1;
	uint32_t *half2 = malloc(numbers_half2*sizeof(uint32_t));
	if (!half2)
	{
		int olderrno = errno;
		close(fd);
		unlink("temp1");
		errno = olderrno;
		err(14, "No memory for second half of file %s", argv[1]);
	}

	read_size = read(fd, half2, numbers_half2*sizeof(uint32_t));
	/*if (read_size < 0)
	{
		int olderrno = errno;
		close(fd);
		unlink("temp1");
		free(half2);
		errno = olderrno;
		err(15, "Error in reading second half of file %s", argv[1]);
	}*/
	if (read_size != numbers_half2*sizeof(uint32_t))
	{
		int olderrno = errno;
                close(fd);
                unlink("temp1");
                free(half2);
                errno = olderrno;
                errx(16, "Cannot read correctly second half of file %s", argv[1]);
	}

	qsort(half2, numbers_half2, sizeof(uint32_t), comparator);

	int temp2;
	temp2 = open("temp2", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp2 < 0)
	{
		int olderrno = errno;
                close(fd);
                unlink("temp1");
                free(half2);
                errno = olderrno;
                err(17, "Cannot open file temp2");
	}

	write_size = write(temp2, half2, read_size);
	/*if (write_size < 0)
	{
		int olderrno = errno;
                close(fd);
                unlink("temp1");
		unlink("temp2");
                free(half2);
                errno = olderrno;
                err(18, "Error in writing in file temp2");
	}*/
	if (write_size != read_size)
        {
                int olderrno = errno;
                close(fd);
                unlink("temp1");
                unlink("temp2");
                free(half2);
                errno = olderrno;
                err(19, "Cannot write correctly in file temp2");
        }

	free(half2);
	close(fd);

	off_t check;
	check =  lseek(temp1, 0, SEEK_SET);
	if (check < 0)
	{
		int olderrno = errno;
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(20, "Error in positioning in file temp1");
	}

	check =  lseek(temp2, 0, SEEK_SET);
        if (check < 0)
        {
                int olderrno = errno;
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(21, "Error in positioning in file temp2");
        }

	int output;
	output = open("sorted", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (output < 0)
	{
		int olderrno = errno;
		unlink("temp1");
		unlink("temp2");
		errno = olderrno;
		err(22, "Cannot open file sorted");
	}

	uint32_t a;
	uint32_t b;

	ssize_t read_a = -1;
	ssize_t read_b = -1;

	ssize_t write_a;
	ssize_t write_b;

	while((read_a=read(temp1, &a, sizeof(a))) > 0 && (read_b=read(temp2, &b, sizeof(b))) > 0)
	{
		if (read_a != sizeof(a))
		{
			int olderrno = errno;
			close(output);
			unlink("temp1");
			unlink("temp2");
			errno = olderrno;
			errx(23, "Cannot read correctly file temp1");
		}
		if (read_b != sizeof(b))
                {
                        int olderrno = errno;
			close(output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(24, "Cannot read correctly file temp2");
                }

		if (a <= b)
		{
			write_a = write(output, &a, read_a);
			if (write_a < 0)
			{
				int olderrno = errno;
				close(output);
				unlink("temp1");
				unlink("temp2");
				errno = olderrno;
				err(25, "Error in writing in file sorted");
			}
			if (write_a != read_a)
                        {
                                int olderrno = errno;
				close(output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                errx(26, "Cannot write correctly in file sorted");
                        }

			check = lseek(temp2, -1*sizeof(b), SEEK_CUR);
			if (check < 0)
			{
				int olderrno = errno;
				close(output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                err(27, "Error in positioning in file temp2");
			}
		}
		else
		{
			write_b = write(output, &b, read_b);
                        if (write_b < 0)
                        {
                                int olderrno = errno;
				close(output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                err(28, "Error in writing in file sorted");
                        }
                        if (write_b != read_b)
                        {
                                int olderrno = errno;
				close(output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                errx(29, "Cannot write correctly in file sorted");
                        }

			check = lseek(temp1, -1*sizeof(a), SEEK_CUR);
                        if (check < 0)
                        {
                                int olderrno = errno;
				close(output);
                                unlink("temp1");
                                unlink("temp2");
                                errno = olderrno;
                                err(30, "Error in positioning in file temp1");
                        }
		}
	}

	if (read_a == -1)
	{
		int olderrno = errno;
		close(output);
		unlink("temp1");
		unlink("temp2");
		errno = olderrno;
		err(27, "Error in reading file temp1");
	}
	if (read_b == -1)
        {
                int olderrno = errno;
		close(output);
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(28, "Error in reading file temp2");
        }

	while ((read_a=read(temp1, &a, sizeof(a))) > 0)
	{
		if (read_a != sizeof(a))
		{
			int olderrno = errno;
			close(output);
			unlink("temp1");
			unlink("temp2");
			errno = olderrno;
			errx(29, "Cannot read correctly file temp1");
		}	

		write_a = write(output, &a, read_a);
		if (write_a < 0)
		{
			int olderrno = errno;
			close(output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        err(30, "Error in writing in file sorted");
		}
		if (write_a != read_a)
		{
			int olderrno = errno;
			close(output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(31, "Cannot write correctly file sorted");
		}
	}

	while ((read_b=read(temp2, &b, sizeof(b))) > 0)
	{
		if (read_b != sizeof(b))
                {
                        int olderrno = errno;
                        close(output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(32, "Cannot read correctly file temp2");
                }

                write_b = write(output, &b, read_b);
                if (write_b < 0)
                {
                        int olderrno = errno;
                        close(output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        err(33, "Error in writing in file sorted");
                }
		if (write_b != read_b)
                {
                        int olderrno = errno;
                        close(output);
                        unlink("temp1");
                        unlink("temp2");
                        errno = olderrno;
                        errx(34, "Cannot write correctly file sorted");
                }
	}

	if (read_a == -1)
        {
                int olderrno = errno;
		close(output);
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(35, "Error in reading file temp1");
        }
        if (read_b == -1)
        {
                int olderrno = errno;
		close(output);
                unlink("temp1");
                unlink("temp2");
                errno = olderrno;
                err(36, "Error in reading file temp2");
        }

	close(output);
	unlink("temp1");
	unlink("temp2");

	exit(0);
}
