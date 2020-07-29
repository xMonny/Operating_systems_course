//Koпирайте файл /etc/passwd в текущата ви работна директория и променете разделителят на копирания файл от ":", на "?"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

int main()
{
	int fd1;
	fd1 = open("/etc/passwd", O_RDONLY);
	if (fd1 < 0)
	{
		err(1, "Cannot open file /etc/passwd");
	}
	
	int fd2 = open("etc_passwd_copy", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd2 < 0)
	{
		int olderrno = errno;
		close(fd1);
		errno = olderrno;
		err(2, "Cannot open file etc_passwd_copy");
	}

	char c;
	ssize_t read_size;

	while ((read_size = read(fd1, &c, 1)) > 0)
	{
		if (c == ':')
		{
			c = '?';
		}

		if (write(fd2, &c, read_size) != read_size)
		{
			int olderrno = errno;
			close(fd1);
			close(fd2);
			errno = olderrno;
			err(3, "Error in writing");
		}
	}
	
	close(fd1);
	close(fd2);
	exit(0);
}
