#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

struct record_t
{
	uint32_t uid;
	uint16_t saved1;
	uint16_t saved2;
	uint32_t time1;
	uint32_t time2;
}__attribute__((packed)) record;

uint32_t getAverage(int fd, uint16_t rows)
{
	uint32_t sum = 0;
	ssize_t read_size = -1;

	while((read_size=read(fd, &record, sizeof(record))) > 0)
	{
		if (read_size != sizeof(record))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(8, "Cannot read correctly input file");
		}

		uint32_t difference = record.time2 - record.time1;
		sum += difference;
	}

	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(9, "Error in reading from input file");
	}

	off_t check = lseek(fd, 0, SEEK_SET);
	if (check < 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(10, "Error in positioning in input file");
	}

	uint32_t average = sum/rows;
	return average;
}

uint64_t getDispersion(int fd, uint16_t rows)
{
	uint32_t average = getAverage(fd, rows);

	uint64_t sum = 0;

	ssize_t read_size;
	while((read_size=read(fd, &record, sizeof(record))) > 0)
	{
		if (read_size != sizeof(record))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(11, "Cannot read input file correctly");
		}

		uint32_t difference = record.time2 - record.time1;
		sum += (difference - average)*(difference - average);
	}

	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(12, "Error in reading input file");
	}

	uint64_t dispersion = sum/rows;
	return dispersion;
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

	if (f_size % sizeof(struct record_t) != 0)
	{
		errx(5, "Format of file %s is incorrect", argv[1]);
	}

	uint16_t rows = f_size/sizeof(struct record_t);
	if (rows > 16384)
	{
		errx(6, "File %s has more than 16 384 records", argv[1]);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		err(7, "Cannot open file %s", argv[1]);
	}	

	uint64_t dispersion = getDispersion(fd, rows);

	off_t check = lseek(fd, 0, SEEK_SET);
	if (check < 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(13, "Error in positioning in file %s", argv[1]);
	}

	struct users_t
	{
		uint32_t uid;
		uint32_t session;
	};

	struct users_t user[2048];
	for (uint16_t i = 0; i < 2048; i++)
	{
		user[i].uid = 0;
		user[i].session = 0;
	}

	uint32_t difference;
	uint64_t square;
	ssize_t read_size = -1;
	while((read_size=read(fd, &record, sizeof(record))) > 0)
	{
		if (read_size != sizeof(record))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(14, "Cannot read correctly file %s", argv[1]);
		}

		uint32_t current_uid = record.uid;
		difference = record.time2 - record.time1;
		square = difference*difference;

		if (square < dispersion)
		{
			uint16_t i = 0;
			for (; i < 2048; i++)
			{
				if (user[i].uid == current_uid)
				{
					if (user[i].session < difference)
					{
						user[i].session = difference;
					}
					break;
				}
				else if (user[i].uid == 0)
				{
					user[i].uid = current_uid;
					user[i].session = difference;
					break;
				}
			}
		}
	}
	
	if (read_size == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(15, "Error in reading file %s", argv[1]);
	}
	close(fd);

	for (uint16_t i = 0; i < 2048; i++)
	{
		if (user[i].uid == 0)
		{
			break;
		}

		printf("ID: %d SESSION: %d\n", user[i].uid, user[i].session);
	}

	exit(0);
}
