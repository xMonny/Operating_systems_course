//Реализирайте команда cp, работеща с произволен брой подадени входни параметри.

#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void do_copy(const char* file, const char* destination)
{
	char *dest = (char*)malloc(strlen(destination)+1+strlen(file)+1);

	strcpy(dest, destination);
	strcat(dest, "/");
	strcat(dest, file);

	struct stat st;
	if (stat(file, &st) < 0 || !S_ISREG(st.st_mode))
	{
		free(dest);
		errx(3, "File %s has to be a regular file", file);
	}

	ssize_t file_size = st.st_size;

	int fd1 = open(file, O_RDONLY);
	if (fd1 < 0)
	{
		free(dest);
		err(4, "Cannot open file %s", file);
	}

	int fd2 = open(dest, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd2 < 0)
	{
		int olderrno = errno;
		close(fd1);
		free(dest);
		errno = olderrno;
		err(4, "Cannot open file %s", dest);
	}

	void *buff = malloc(file_size);
	if (!buff)
	{
		int olderrno = errno;
		close(fd1);
		close(fd2);
		free(dest);
		errno = olderrno;
	}

	if (read(fd1, buff, file_size) != file_size)
	{
		int olderrno = errno;
                close(fd1);
                close(fd2);
                free(dest);
		free(buff);
                errno = olderrno;
		err(5, "Error in reading from file %s", file);
	}

	if (write(fd2, buff, file_size) != file_size)
        {
                int olderrno = errno;
                close(fd1);
                close(fd2);
                free(dest);
                free(buff);
                errno = olderrno;
		err(6, "Error in writing in file %s", dest);
        }

	close(fd1);
	close(fd2);
	free(dest);
	free(buff);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		errx(1, "Input atleast 2 arguments");
	}

	char* destination = argv[argc-1];
	struct stat st;
	if (stat(destination, &st) < 0 || !S_ISDIR(st.st_mode))
	{
		err(2, "Last argument should be a directory");
	}

	for (int i = 1; i < argc-1; i++)
	{
		do_copy(argv[i], destination);
	}

	exit(0);
}
