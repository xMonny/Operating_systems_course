#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

void copy(const char* source, const char* destination)
{
	struct stat st;
	if (stat(source, &st) < 0)
	{
		err(13, "Cannot stat file '%s'", source);
	}
	if (!S_ISREG(st.st_mode))
	{
		errx(14, "Argument '%s' should be a regular file", source);
	}
	if (!(st.st_mode & S_IRUSR))
	{
		errx(15, "File '%s' cannot be read", source);
	}
	off_t source_size = st.st_size;

	char* path = (char*)malloc(strlen(destination)+1+strlen(source)+1);
	if (!path)
	{
		err(16, "No memory");
	}

	strcpy(path, destination);
	strcat(path, "/");
	strcat(path, source);

	void* buff = malloc(source_size);
	if (!buff)
	{
		free(path);
		err(17, "No memory");
	}

	int fd_source;
	int fd_path;

	fd_source = open(source, O_RDONLY);
	if (fd_source < 0)
	{
		free(path);
		free(buff);
		err(18, "Cannot open file '%s'", source);
	}

	ssize_t read_size;
	read_size = read(fd_source, buff, source_size);
	if (read_size == -1)
	{
		const int olderrno = errno;
		free(path);
		free(buff);
		close(fd_source);
		errno = olderrno;
		err(19, "Error in reading file '%s'", source);
	}
	if (read_size != source_size)
	{
		const int olderrno = errno;
		free(path);
		free(buff);
		close(fd_source);
		errno = olderrno;
		errx(20, "Cannot read file '%s' correctly", source);
	}

	close(fd_source);

	fd_path = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd_path < 0)
	{
		free(path);
		free(buff);
		err(21, "Cannot open file '%s/%s'", destination, source);
	}

	ssize_t write_size;
	write_size = write(fd_path, buff, read_size);
	if (write_size == -1)
	{
		const int olderrno = errno;
		free(path);
		free(buff);
		close(fd_path);
		errno = olderrno;
		err(22, "Error in writing in file '%s/%s'", destination, source);
	}
	if (write_size != read_size)
	{
		const int olderrno = errno;
		free(path);
		free(buff);
		close(fd_path);
		errno = olderrno;
		errx(23, "Cannot write to file '%s/%s' correctly", destination, source);
	}

	free(path);
	free(buff);
	close(fd_path);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		errx(1, "Expect at least 2 arguments");
	}

	const char* destination = argv[argc-1];
	struct stat st;
	int val = stat(destination, &st);

	if (val < 0 || !S_ISDIR(st.st_mode))
	{
		errx(7, "Last argument should be a directory");
	}
	
	for (int i = 1; i < argc-1; i++)
	{
		copy(argv[i], destination);
	}

	exit(0);
}
