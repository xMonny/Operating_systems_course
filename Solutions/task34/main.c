#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

void shut(const int olderrno, int fd1, int fd2, int fd3, int fd4)
{
	if (fd1 != -1)
	{
		close(fd1);
	}
	if (fd2 != -1)
	{
		close(fd2);
	}
	if (fd3 != -1)
	{
		close(fd3);
	}
	if (fd4 != -1)
	{
		close(fd4);
	}

	if (olderrno != -1)
	{
		errno = olderrno;
	}
}

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		errx(1, "Invalid number of arguments");
	}

	char* f1_dat = argv[1];
	char* f1_idx = argv[2];
	char* f2_dat = argv[3];
	char* f2_idx = argv[4];

	struct stat st;
	if (stat(f1_dat, &st) < 0)
	{
		err(2, "Cannot stat file %s", f1_dat);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(3, "'%s' should be a file", f1_dat);
	}

	if (stat(f1_idx, &st) < 0)
	{
		err(4, "Cannot stat file %s", f1_idx);
	}

	if (!S_ISREG(st.st_mode))
	{
		errx(5, "'%s' should be a file", f1_idx);
	}
	off_t f1_idx_size = st.st_size;

	struct triple_t
	{
		uint16_t padding;
		uint8_t length;
		uint8_t saved;
	}__attribute__((packed)) triple_f1_idx, triple_f2_idx;

	if (f1_idx_size % sizeof(struct triple_t) != 0)
	{
		errx(6, "File %s is corrupted", f1_idx);
	}

	int open_f1_dat;
	int open_f1_idx;
	int open_f2_dat;
	int open_f2_idx;

	open_f1_dat = open(f1_dat, O_RDONLY);
	if (open_f1_dat < 0)
	{
		err(7, "Cannot open file %s", f1_dat);
	}

	open_f1_idx = open(f1_idx, O_RDONLY);
	if (open_f1_idx < 0)
	{
		shut(errno, open_f1_dat, -1, -1, -1);
		err(8, "Cannot open file %s", f1_idx);
	}

	open_f2_dat = open(f2_dat, O_CREAT | O_TRUNC | O_RDWR, 0644);
        if (open_f2_dat < 0)
        {
		shut(errno, open_f1_dat, open_f1_idx, -1, -1);
                err(9, "Cannot open file %s", f2_dat);
        }

        open_f2_idx = open(f2_idx, O_CREAT | O_TRUNC | O_RDWR, 0644);
        if (open_f2_idx < 0)
        {
                shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, -1);
                err(10, "Cannot open file %s", f2_idx);
        }

	bool startOfFile = true;

	ssize_t rd_f1_idx;

	while((rd_f1_idx=read(open_f1_idx, &triple_f1_idx, sizeof(triple_f1_idx))) > 0)
	{
		if (rd_f1_idx != sizeof(triple_f1_idx))
		{
			shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
			errx(11, "Cannot read correctly file %s", f1_idx);
		}

		uint16_t position = triple_f1_idx.padding;
		uint8_t len = triple_f1_idx.length;

		off_t check = lseek(open_f1_dat, position*sizeof(uint8_t), SEEK_SET);
		if (check < 0)
		{
			shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
                        err(12, "Error in positioning in file %s", f1_dat);
		}

		ssize_t rd_f1_dat;
		char symbol;
		uint8_t cnt = 0;
		bool isFirstLetter = true;
		while((rd_f1_dat=read(open_f1_dat, &symbol, sizeof(symbol))) > 0)
		{
			if (rd_f1_dat != sizeof(symbol))
			{
				shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
                        	errx(13, "Cannot read correctly file %s", f1_dat);
			}
	
			if (isFirstLetter)
			{
				if (symbol < 'A' || symbol > 'Z')
				{
					break;
				}
			}
			
			isFirstLetter = false;

			ssize_t wr_f2_dat;
			wr_f2_dat = write(open_f2_dat, &symbol, rd_f1_dat);
			if (wr_f2_dat == -1)
			{
				shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
				err(15, "Error in writing in file %s", f2_dat);
			}

			if (wr_f2_dat != rd_f1_dat)
			{
				shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
                                errx(16, "Cannot write correctly in file %s", f2_dat);
			}

			cnt++;
			if (cnt == len)
			{
				if (startOfFile)
				{
					startOfFile = false;
					triple_f2_idx.padding = 0;
				}
				triple_f2_idx.length = len;
				triple_f2_idx.saved = triple_f1_idx.saved;

				ssize_t wr_f2_idx;
				wr_f2_idx = write(open_f2_idx, &triple_f2_idx, sizeof(triple_f2_idx));
				if (wr_f2_idx == -1)
				{
					shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
					err(17, "Error in writing in file %s", f2_idx);
				}

				if (wr_f2_idx != sizeof(triple_f2_idx))
				{
					shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
                                        errx(18, "Cannot write correctly in file %s", f2_idx);
				}

				triple_f2_idx.padding += len; 

				break;
			}		
		}

		if (rd_f1_dat == -1)
		{
			shut(errno, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);
                        err(14, "Error in reading file %s", f1_dat);
		}
	}

	shut(-1, open_f1_dat, open_f1_idx, open_f2_dat, open_f2_idx);

	if (rd_f1_idx == -1)
	{
		err(12, "Error in reading from file %s", f1_idx);
	}	
}
