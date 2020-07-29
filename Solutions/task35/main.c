#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		errx(1, "Invalid numbe of arguments");
	}

	struct stat st;
	if (stat(argv[1], &st) < 0)
	{
		err(2, "Cannot stat file %s", argv[1]);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "First argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File %s cannot be read", argv[1]);
	}
	off_t f1_size = st.st_size;

	if (stat(argv[2], &st) < 0)
        {
                err(5, "Cannot stat file %s", argv[2]);
        }
        if (!S_ISREG(st.st_mode))
        {
                errx(6, "Second argument should be a file");
        }
        if (!(st.st_mode & S_IRUSR))
        {
                errx(7, "File %s cannot be read", argv[2]);
        }
	off_t f2_size = st.st_size;

	if (f1_size != f2_size)
	{
		errx(8, "File %s and file %s have different size", argv[1], argv[2]);
	}

	struct triple_t
	{
		uint16_t padding;
		uint8_t original;
		uint8_t new;
	}__attribute__((packed)) triple;

	int f1;
	int f2;
	int f3;

	f1 = open(argv[1], O_RDONLY);
	if (f1 < 0)
	{
		err(9, "Cannot open file %s", argv[1]);
	}

	f2 = open(argv[2], O_RDONLY);
	if (f2 < 0)
	{
		int olderrno = errno;
		close(f1);
		errno = olderrno;
		err(10, "Cannot open file %s", argv[2]);
	}

	f3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (f3 < 0)
	{
		int olderrno = errno;
		close(f1);
		close(f2);
		errno = olderrno;
		err(11, "Cannot open file %s", argv[3]);
	}

	uint8_t a;
	uint8_t b;

	ssize_t read_a = -1;
	ssize_t read_b = -1;

	uint16_t position = 0;

	while((read_a=read(f1, &a, sizeof(a))) > 0 && (read_b=read(f2, &b, sizeof(b))) > 0)
	{
		if (read_a != sizeof(a))
		{
			int olderrno = errno;
			close(f1);
			close(f2);
			close(f3);
			errno = olderrno;
			errx(12, "Cannot read correctly file %s", argv[1]);
		}
	
		if (read_b != sizeof(b))
		{
			int olderrno = errno;
			close(f1);
			close(f2);
			close(f3);
			errno = olderrno;
			errx(13, "Cannot read correctly file %s", argv[2]);
		}

		if (a != b)
		{
			triple.padding = position;
			triple.original = a;
			triple.new = b;

			ssize_t write_size;
			write_size = write(f3, &triple, sizeof(triple));
			if (write_size == -1)
			{
				int olderrno = errno;
				close(f1);
				close(f2);
				close(f3);
				errno = olderrno;
				err(14, "Error in writing in file %s", argv[3]);
			}
			
			if (write_size != sizeof(triple))
			{
				int olderrno = errno;
				close(f1);
				close(f2);
				close(f3);
				errno = olderrno;
				errx(15, "Cannot write correctly in file %s", argv[3]);
			}
		}
		position++;
	}

	if (read_a == -1)
	{
		int olderrno = errno;
		close(f1);
		close(f2);
		close(f3);
		errno = olderrno;
		err(16, "Error in reading file %s", argv[1]);	
	}

	if (read_b == -1)
	{
		int olderrno = errno;
		close(f1);
		close(f2);
		close(f3);
		errno = olderrno;
		err(17, "Error in reading file %s", argv[2]);
	}

	close(f1);
	close(f2);
	close(f3);

	exit(0);
}
