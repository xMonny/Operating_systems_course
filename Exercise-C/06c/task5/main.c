//Реализирайте команда cp, работеща с два аргумента, подадени като входни параметри.

#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		errx(1, "Invalid number of arguments");
	}

	struct stat st;
	if (stat(argv[1], &st) < 0)
	{
		err(2, "Cannot stat file %s", argv[1]);
	}

	int fd1;
	fd1 = open(argv[1], O_RDONLY);
	if (fd1 < 0)
	{
		err(3, "Cannot open file %s", argv[1]);
	}

	int fd2;
	fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd2 < 0)
	{
		int olderrno = errno;
		close(fd1);
		errno = olderrno;
		err(4, "Cannot open file %s", argv[2]);
	}

	char c[st.st_size];

	if (read(fd1, &c, st.st_size) != st.st_size)
	{
		int olderrno = errno;
		close(fd1);
		close(fd2);
		errno = olderrno;
		err(5, "Error in reading from file %s", argv[1]);
	}

	if (write(fd2, &c, st.st_size) != st.st_size)
	{
		int olderrno = errno;
                close(fd1);
                close(fd2);
                errno = olderrno;
                err(6, "Error in writing in file %s", argv[2]);
	}
	
	close(fd1);
	close(fd2);
	exit(0);
}
