#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>

int main()
{
	while(1)
	{
		const char* prompt = "simple prompt: ";
		ssize_t write_size;
		write_size = write(1, prompt, strlen(prompt));
		if (write_size == -1)
		{
			err(1, "Error in writing prompt to stdout"); 
		}
		if (write_size != (ssize_t)strlen(prompt))
		{
			errx(2, "Cannot write correctly prompt to stdout");
		}

		char buffer[30];
		int position = 0;
		ssize_t read_buffer = -1;
		while((read_buffer=read(0, buffer+position, 1)) > 0)
		{
			if (read_buffer != 1)
			{
				errx(3, "Cannot read correctly from stdin");
			}
			if (buffer[position] == '\n')
			{
				buffer[position] = '\0';
				break;
			}
			else
			{
				position++;
			}
		}
		if (read_buffer == -1)
		{
			err(4, "Error in reading from stdin");
		}
		if (strcmp(buffer, "exit") == 0)
		{
			break;
		}
		pid_t p = fork();
		if (p < 0)
		{
			err(5, "Cannot fork");
		}
		if (p == 0)
		{
			if (execlp(buffer, "command", (char*)NULL) < 0)
			{
				err(6, "Cannot execute command %s", buffer);
			}
		}
		if (wait(NULL) < 0)
		{
			err(7, "Error in waiting");
		}
	}

	exit(0);
}
