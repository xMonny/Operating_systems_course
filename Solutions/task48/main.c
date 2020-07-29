#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int execute(char* arguments[])
{
	pid_t p = fork();
	if (p < 0)
	{
		err(4, "Cannot fork");
	}
	if (p == 0)
	{
		if (execvp(arguments[0], arguments) < 0)
		{
			err(5, "Cannot execute command %s", arguments[0]);
		}
	}
	
	int status;
	pid_t wait_code = wait(&status);
	if (wait_code < 0)
	{
		err(6, "Error in waiting");
	}
	if (!WIFEXITED(status))
	{		
		err(7, "Process didn't terminate normally");
	}
	
	return WEXITSTATUS(status);
}

void record(time_t start, time_t end, int exitCode)
{
	int fd = open("run.log", O_APPEND | O_WRONLY);
	if (fd < 0)
	{
		err(10, "Cannot open file 'run.log'");
	}

	char buff[2*sizeof(time_t)+sizeof(exitCode)+5];
	sprintf(buff, "%ld %ld %d\n", start, end, exitCode);

	ssize_t write_size;
	write_size = write(fd, buff, strlen(buff));
	if (write_size == -1)
	{
		const int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(11, "Error in writing in file 'run.log'");
	}
	close(fd);
}

bool stopped(char* arguments[], int seconds)
{
	time_t start = time(NULL);
	int exitStatus = execute(arguments);
	time_t end = time(NULL);

	record(start, end, exitStatus);
	
	int difference = end - start;
	
	if (exitStatus != 0 && difference < seconds)
	{
		return true;
	}
	return false;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		errx(1, "Expect at least 2 arguments");
	}

	const char* limit = argv[1];
	if (strlen(limit) != 1)
	{
		errx(2, "First argument should be a digit between 1 and 9");
	}
	if (limit[0] < '1' || limit[0] > '9')
	{
		errx(3, "First argument is out of interval [1;9]");
	}

	int len = argc-1;
	char* arguments[len];
	int i = 0;
	int j = 2;
	for (; i < len-1; i++)
	{
		arguments[i] = argv[j];
		j++; 
	}
	arguments[i] = NULL;

	int seconds = limit[0] - '0';
	int counter = 0;

	int fd = open("run.log", O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd < 0)
	{
		err(20, "Cannot open file 'run.log'");
	}
	close(fd);

	while (counter != 2)
	{
		if (stopped(arguments, seconds))
		{
			counter++;
		}
		else
		{
			counter = 0;
		}
	}

	exit(0);
}
