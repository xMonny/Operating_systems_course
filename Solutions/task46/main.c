#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

int max_arguments = 2;
int length = 4;

void execute(char* command, char arguments[][length+1])
{
	char* execute_commands[max_arguments+2];
	execute_commands[0] = command;
	int i = 1;
	int j = 0;
	while (arguments[j][0] != '\0')
	{
		execute_commands[i] = arguments[j];
		i++;
		j++;
	}
	execute_commands[i] = NULL;

	pid_t p = fork();
	if (p < 0)
	{
		err(5, "Cannot fork");
	}
	if (p == 0)
	{
		if (execvp(command, execute_commands) < 0)
		{
			err(6, "Cannot execute commands %s", command);
		}
	}
	if (wait(NULL) < 0)
	{
		err(7, "Error in waiting");
	}
}

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		errx(1, "Program can accept 0 or 1 argument");
	}

	char* command;
	if (argc == 1)
	{
		command = "echo";
	}
	else
	{
		command = argv[1];
	}

	if (strlen(command) > 4)
	{
		errx(2, "Length of input command can be maximum 4 characters");
	}

	char arguments[max_arguments+1][length+1];
	char buff[length+1];

	int position = 0;
	int arg_num = 0;

	ssize_t read_size = -1;
	while((read_size=read(0, buff+position, 1)) > 0)
	{
		if (read_size != 1)
		{
			errx(3, "Cannot read correctly from stdin");
		}
		if (arg_num == max_arguments)
		{
			arguments[arg_num][0] = '\0';
			execute(command, arguments);
			arg_num = 0;
		}
		if (buff[position] == ' ' || buff[position] == '\n')
		{
			buff[position] = '\0';
			if (strlen(buff) > 4)
			{
				err(20, "Length of input argument from stdin can be maximum 4 characters");
			}
			strcpy(arguments[arg_num], buff);
			arg_num++;
			position = 0;
		}
		else
		{
			position++;
		}
	}

	if (read_size == -1)
	{
		err(4, "Error in reading from stdin");
	}

	if (arg_num != 0)
	{
		arguments[arg_num][0] = '\0';
		execute(command, arguments);
	}

	exit(0);
}
