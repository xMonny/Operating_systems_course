#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

void deleteFrom(char* string)
{
	char c;
	ssize_t read_size;
	while((read_size=read(0, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			err(3, "Error in reading from stdin");
		}

		char* meet = strchr(string, c);
		if (!meet)
		{
			if (write(1, &c, sizeof(c)) != read_size)
			{
				errx(4, "Error in writing to stdout");
			}	
		}
	}
}

void replace(char* from, char* to)
{
	char c;
	ssize_t read_size;
	while((read_size=read(0, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			err(5, "Error in reading from stdin");
		}

		for (size_t i = 0; i < strlen(from); i++)
		{
			if (c == from[i])
			{
				c = to[i];
				break;
			}
		}

		if (write(1, &c, sizeof(c)) != read_size)
		{
			errx(6, "Error in writing to stdout");
		}
	}
}

void squeeze(char* string)
{
	char c;
	char last = '\0';
	ssize_t read_size;
	while ((read_size=read(0, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			err(7, "Error in reading from stdin");
		}

		char* meet = strchr(string, c);

		if (!meet || c != last)
		{
			last = c;
			if (write(1, &c, sizeof(c)) != read_size)
			{
				errx(8, "Error in writing to stdout");
			}
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		errx(1, "Invalid number of arguments");
	}

	if (strcmp(argv[1], "-d") == 0)
	{
		if (argc != 3)
		{
			errx(2, "One string is needed for flag '-d'");
		}

		deleteFrom(argv[2]);
		exit(0);
	}

	if (strcmp(argv[1], "-s") == 0)
	{
		if (argc != 3)
		{
			errx(3, "One string is needed for flag '-s'");
		}

		squeeze(argv[2]);
		exit(0);
	}

	if (argc != 3)
	{
		errx(4, "If no flag, then two strings are expected");
	}

	if (strlen(argv[1]) != strlen(argv[2]))
	{
		errx(5, "Input strings should have same length");
	}
	replace(argv[1], argv[2]);

	exit(0);
}
