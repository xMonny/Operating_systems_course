#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

struct format_t
{
	uint32_t magic;
	uint8_t header;
	uint8_t data;
	uint16_t count;
	uint32_t reserved1;
	uint32_t reserved2;
}__attribute__((packed));

struct data00_format
{
	uint16_t offset;
	uint8_t original_byte;
	uint8_t new_byte;
}__attribute__((packed));

struct data01_format
{
	uint32_t offset;
	uint16_t original_word;
	uint16_t new_word;
}__attribute__((packed));

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		errx(1, "Invalid number of arguments");
	}

	const char* patch = argv[1];
	const char* f1 = argv[2];
	const char* f2 = argv[3];

	struct stat st;
	if (stat(patch, &st) < 0)
	{
		err(2, "Cannot stat file '%s'", patch);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "First argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(4, "File '%s' cannot be read", patch);
	}
	off_t patch_size = st.st_size;
	patch_size -= sizeof(struct format_t);
	if (patch_size % sizeof(struct data00_format) != 0 || patch_size % sizeof(struct data01_format) != 0)
	{
		errx(30, "Error in format of file '%s'", patch);
	}

	if (stat(f1, &st) < 0)
	{
		err(5, "Cannot tsta file '%s'", f1);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(6, "Second argument should be a file");
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(7, "File '%s' cannot be read", f1);
	}
	off_t f1_size = st.st_size;

	int fd_patch;
	int fd_f1;
	int fd_f2;

	fd_patch = open(patch, O_RDONLY);
	if (fd_patch < 0)
	{
		err(8, "Cannot open file '%s'", patch);
	}

	struct format_t format;
	ssize_t read_patch = -1;
	read_patch = read(fd_patch, &format, sizeof(format));
	if (read_patch == -1)
	{
		const int olderrno = errno;
		close(fd_patch);
		errno = olderrno;
		err(9, "Error in reading file '%s'", patch);
	}
	if (read_patch != sizeof(format))
	{
		const int olderrno = errno;
		close(fd_patch);
		errno = olderrno;
		errx(10, "Cannot read correctly file '%s'", patch);
	}

	bool format00 = false;
	bool format01 = false;
	
	if (format.data == 0x00)
	{
		format00 = true;
	}
	else if (format.data == 0x01)
	{
		format01 = true;
	}

	if (format00)
	{
		if (patch_size % sizeof(struct data00_format) != 0)
		{
			const int olderrno = errno;
			close(fd_patch);
			errno = olderrno;
			errx(30, "Error in format of file '%s'", patch);
		}
		if (f1_size % sizeof(uint8_t) != 0)
		{
			const int olderrno = errno;
			close(fd_patch);
			errno = olderrno;
			errx(11, "Error in format of file '%s'", f1);
		}
	}

	if (format01)
	{
		if (patch_size % sizeof(struct data01_format) != 0)
		{
			const int olderrno = errno;
			close(fd_patch);
			errno = olderrno;
			errx(30, "Error in format of file '%s'", patch);
		}
		if (f1_size % sizeof(uint16_t) != 0)
		{
			const int olderrno = errno;
			close(fd_patch);
			errno = olderrno;
			errx(12, "Error in format of file '%s'", f1);
		}
	}

	fd_f1 = open(f1, O_RDONLY);
	if (fd_f1 < 0)
	{
		const int olderrno = errno;
		close(fd_patch);
		errno = olderrno;
		err(13, "Cannot open file '%s'", f1);
	}

	void* buff = malloc(f1_size);
	if (!buff)
	{
		const int olderrno = errno;
		close(fd_patch);
		close(fd_f1);
		errno = olderrno;
		err(14, "No memory");
	}

	ssize_t read_f1 = -1;
	read_f1 = read(fd_f1, buff, f1_size);
	if (read_f1 == -1)
	{
		const int olderrno = errno;
		free(buff);
		close(fd_patch);
		close(fd_f1);
		errno = olderrno;
		err(15, "Error in reading file '%s'", f1);
	}
	if (read_f1 != f1_size)
	{
		const int olderrno = errno;
		free(buff);
		close(fd_patch);
		close(fd_f1);
		errno = olderrno;
		errx(16, "Cannot read correctly file '%s'", f1);
	}

	close(fd_f1);

	fd_f2 = open(f2, O_CREAT | O_TRUNC | O_RDWR, 0644);
	if (fd_f2 < 0)
	{
		const int olderrno = errno;
		free(buff);
		close(fd_patch);
		errno = olderrno;
		err(17, "Cannot open file '%s'", f2);
	}

	ssize_t write_f2 = -1;
	write_f2 = write(fd_f2, buff, read_f1);
	if (write_f2 == -1)
	{
		const int olderrno = errno;
		free(buff);
		close(fd_patch);
		close(fd_f2);
		errno = olderrno;
		err(18, "Error in writing in file '%s'", f2);
	}
	if (write_f2 != read_f1)
	{
		const int olderrno = errno;
		free(buff);
		close(fd_patch);
		close(fd_f2);
		errno = olderrno;
		errx(19, "Cannot write correctly in file '%s'", f2);
	}

	free(buff);

	off_t check;
	check = lseek(fd_f2, 0, SEEK_SET); //not necessary
	if (check < 0)
	{
		const int olderrno = errno;
		close(fd_patch);
		close(fd_f2);
		errno = olderrno;
		err(20, "Error in positioning in file '%s'", f2);
	}

	if (format00)
	{
		struct data00_format d0;
		while((read_patch=read(fd_patch, &d0, sizeof(d0))) > 0)
		{
			if (read_patch != sizeof(d0))
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				errx(21, "Cannot read correctly file '%s'", patch);	
			}
			
			check = lseek(fd_f2, d0.offset*sizeof(uint8_t), SEEK_SET);
			if (check < 0)
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				err(22, "Error in positioning in file '%s'", f2);
			}

			uint8_t current;
			ssize_t read_f2 = -1;
			read_f2 = read(fd_f2, &current, sizeof(current));
			if (read_f2 == -1)
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				err(23, "Error in reading file '%s'", f2);
			}
			if (read_f2 != sizeof(current))
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				errx(24, "Cannot read correctly file '%s'", f2);
			}

			if (current == d0.original_byte)
			{
				check = lseek(fd_f2, d0.offset*sizeof(uint8_t), SEEK_SET);
				if (check < 0)
				{
					const int olderrno = errno;
					close(fd_patch);
					close(fd_f2);
					errno = olderrno;
					err(25, "Error in positioning in file '%s'", f2);
				}

				uint8_t new = d0.new_byte;
				write_f2 = write(fd_f2, &new, sizeof(new));
				if (write_f2 == -1)
				{
					const int olderrno = errno;
					close(fd_patch);
					close(fd_f2);
					errno = olderrno;
					err(26, "Error in writing in file '%s'", f2);
				}
				if (write_f2 != sizeof(new))
				{
					const int olderrno = errno;
					close(fd_patch);
					close(fd_f2);
					errno = olderrno;
					errx(27, "Cannot write correctly in file '%s'", f2);
				}
			}
			else
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				errx(28, "Invalid byte");
			}
		}
		if (read_patch == -1)
		{
			const int olderrno = errno;
			close(fd_patch);
			close(fd_f2);
			errno = olderrno;
			err(29, "Error in reading file '%s'", patch);
		}
	}

	if (format01)
	{
		struct data01_format d1;
		while((read_patch=read(fd_patch, &d1, sizeof(d1))) > 0)
		{
			if (read_patch != sizeof(d1))
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				errx(21, "Cannot read correctly file '%s'", patch);	
			}
			
			check = lseek(fd_f2, d1.offset*sizeof(uint16_t), SEEK_SET);
			if (check < 0)
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				err(22, "Error in positioning in file '%s'", f2);
			}

			uint16_t current;
			ssize_t read_f2 = -1;
			read_f2 = read(fd_f2, &current, sizeof(current));
			if (read_f2 == -1)
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				err(23, "Error in reading file '%s'", f2);
			}
			if (read_f2 != sizeof(current))
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				errx(24, "Cannot read correctly file '%s'", f2);
			}

			if (current == d1.original_word)
			{
				check = lseek(fd_f2, d1.offset*sizeof(uint16_t), SEEK_SET);
				if (check < 0)
				{
					const int olderrno = errno;
					close(fd_patch);
					close(fd_f2);
					errno = olderrno;
					err(25, "Error in positioning in file '%s'", f2);
				}

				uint16_t new = d1.new_word;
				write_f2 = write(fd_f2, &new, sizeof(new));
				if (write_f2 == -1)
				{
					const int olderrno = errno;
					close(fd_patch);
					close(fd_f2);
					errno = olderrno;
					err(26, "Error in writing in file '%s'", f2);
				}
				if (write_f2 != sizeof(new))
				{
					const int olderrno = errno;
					close(fd_patch);
					close(fd_f2);
					errno = olderrno;
					errx(27, "Cannot write correctly in file '%s'", f2);
				}
			}
			else
			{
				const int olderrno = errno;
				close(fd_patch);
				close(fd_f2);
				errno = olderrno;
				errx(28, "Invalid word");
			}
		}
		if (read_patch == -1)
		{
			const int olderrno = errno;
			close(fd_patch);
			close(fd_f2);
			errno = olderrno;
			err(29, "Error in reading file '%s'", patch);
		}
	}

	close(fd_patch);
	close(fd_f2);

	exit(0);
}
