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
		errx(1, "Invalid number of arguments");
	}

	char* patch = argv[1];
	char* f1 = argv[2];
	char* f2 = argv[3];

	struct stat st;

	//check file f1
	if (stat(f1, &st) < 0)
	{
		err(2, "Cannot stat file %s", f1);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "First argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File %s cannot be read", f1);
	}
	off_t f1_size = st.st_size;

	if (f1_size > 0xFFFF)
	{
		warnx("Size of file %s is greater than 65535", f1);
	}

	//check file patch
	if (stat(patch, &st) < 0)
	{
		err(5, "Cannot stat file %s", patch);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(6, "Third argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(7, "File %s cannot be read", patch);
	}
	off_t patch_size = st.st_size;

	struct triple_t
	{
		uint16_t padding;
		uint8_t original;
		uint8_t new;
	}__attribute__((packed));

	if (patch_size % sizeof(struct triple_t) != 0)
	{
		errx(8, "Error in format of file %s", patch);
	}

	//open files
	int fd1 = open(f1, O_RDONLY);
	if (fd1 < 0)
	{
		err(9, "Cannot open file %s", f1);
	}

	int fd2 = open(f2, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd2 < 0)
	{
		const int olderrno = errno;
		close(fd1);
		errno = olderrno;
		err(10, "Cannot open file %s", f2);
	}

	int fd3 = open(patch, O_RDONLY);
	if (fd3 < 0)
	{
		const int olderrno = errno;
		close(fd1);
		close(fd2);
		errno = olderrno;
		err(11, "Cannot open file %s", patch);
	}

	void *buff = malloc(f1_size);
	if (!buff)
	{
		const int olderrno = errno;
		close(fd1);
		close(fd2);
		close(fd3);
		errno = olderrno;
		err(12, "No memory");
	}

	ssize_t read_size;
	read_size = read(fd1, buff, f1_size);
	if (read_size < 0)
	{
		const int olderrno = errno;
		close(fd1);
		close(fd2);
		close(fd3);
		free(buff);
		errno = olderrno;
		err(13, "Error in reading file %s", f1);
	}
	if (read_size != f1_size)
	{
		const int olderrno = errno;
		close(fd1);
		close(fd2);
		close(fd3);
		free(buff);
		errno = olderrno;
		errx(14, "Cannot read correctly file %s", f1);
	}

	close(fd1);

	ssize_t write_size;
	write_size = write(fd2, buff, read_size);
	if (write_size < 0)
	{
		const int olderrno = errno;
                close(fd2);
                close(fd3);
		free(buff);
                errno = olderrno;
                err(15, "Error in writing in file %s", f2);
	}
	if (write_size != read_size)
	{
		const int olderrno = errno;
                close(fd2);
                close(fd3);
		free(buff);
                errno = olderrno;
                errx(16, "Cannot write correctly in file %s", f2);
	}

	free(buff);
	
	struct triple_t triple;

	while ((read_size=read(fd3, &triple, sizeof(triple))) > 0)
	{
		if (read_size != sizeof(triple))
		{
			const int olderrno = errno;
			close(fd2);
			close(fd3);
			errno = olderrno;
			errx(17, "Cannot read correctly file %s", patch);
		}

		uint16_t position = triple.padding;
		uint8_t original_byte = triple.original;
		uint8_t new_byte = triple.new;

		off_t check = lseek(fd2, position*sizeof(uint8_t), SEEK_SET);
		if (check < 0)
		{
			const int olderrno = errno;
                        close(fd2);
                        close(fd3);
                        errno = olderrno;
                        err(18, "Error in positioning in file %s", patch);
		}

		uint8_t current_byte;
		ssize_t read_current;
		read_current = read(fd2, &current_byte, sizeof(current_byte));
		if (read_current == -1)
		{
			const int olderrno = errno;
			close(fd2);
			close(fd3);
			errno = olderrno;
			err(19, "Error in reading file %s", f2);
		}
		if (read_current != sizeof(current_byte))
		{
			const int olderrno = errno;
                        close(fd2);
                        close(fd3);
                        errno = olderrno;
                        errx(20, "Cannot read correctly file %s", f2);
		}

		if (current_byte == original_byte)
		{
			check = lseek(fd2, -1*sizeof(current_byte), SEEK_CUR);
			if (check < 0)
			{
				const int olderrno = errno;
				close(fd2);
				close(fd3);
				errno = olderrno;
				err(21, "Error in positioning in file %s", f2);
			}
		
			ssize_t write_current;
			write_current = write(fd2, &new_byte, sizeof(original_byte));
			if (write_current == -1)
			{
				const int olderrno = errno;
                                close(fd2);
                                close(fd3);
                                errno = olderrno;
                                err(22, "Error in writing in file %s", f2);
			}
			if (write_current != sizeof(original_byte))
			{
				const int olderrno = errno;
                                close(fd2);
                                close(fd3);
                                errno = olderrno;
                                errx(23, "Cannot write correctly in file %s", f2);
			}
		}
	}

	if (read_size == -1)
	{
		const int olderrno = errno;
                close(fd2);
                close(fd3);
                errno = olderrno;
                err(24, "Error in reading file %s", patch);
	}

	close(fd2);
	close(fd3);
	
	exit(0);
}
