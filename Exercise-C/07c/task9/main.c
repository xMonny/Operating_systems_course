#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Expect one name of file");
	}

	int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd < 0)
	{
		err(2, "Cannot open file %s", argv[1]);
	}

	const char* str1 = "foo\n";
	const char* str2 = "bar\n";

	pid_t p = fork();
	if (p < 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(3, "Cannot fork");
	}
	if (p == 0)
	{
		ssize_t write_child;
		write_child = write(fd, str1, strlen(str1));
		if (write_child == -1)
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(4, "Error in writing in file %s", argv[1]);
		}
		if (write_child != (ssize_t)strlen(str1))
		{
			const int olderrno = errno;
			close(fd);
			errno = olderrno;
			errx(5, "Cannot write correctly in file %s", argv[1]);
		}
		close(fd);
		exit(0);
	}
	
	//for ordering - uncomment
	
	/*int status;	
	pid_t wait_code = wait(&status);
	if (wait_code < 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(7, "Error in waiting");
	}
	if (!WIFEXITED(status))
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(8, "Child didn't terminate normally");
	}
	if (WEXITSTATUS(status) != 0)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(9, "Exit status of child process is not 0");
	}*/

	ssize_t write_parent;
	write_parent = write(fd, str2, strlen(str2));
	if (write_parent == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(10, "Error in writing in file %s", argv[1]);
	}
	if (write_parent != (ssize_t)strlen(str2))
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		errx(11, "Cannot write correctly in file %s", argv[1]);
	}
	close(fd);

	exit(0);
}
