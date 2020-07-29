#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>

void cut (char left, char right, char delimiter)
{
	char c;
	char cnt = '1';
	size_t read_size;
	while ((read_size=read(0, &c, sizeof(c))) > 0)
	{
		if (read_size != sizeof(c))
		{
			err(10, "Error in reading from stdin");
		}
		
		if (c == '\n')
                {
                       	cnt = '1';
                       	read_size = write(1, &c, sizeof(c));
                       	if (read_size != sizeof(c))
                       	{
                               	err(11, "Error in writing in stdout");
                       	}
			continue;
                }		

		if (cnt >= left && cnt <= right)
		{
			if (cnt == right && c == delimiter)
			{
				c = '\0';
			}
			read_size = write(1, &c, sizeof(c));
			if (read_size != sizeof(c))
			{
				err(11, "Error in writing in stdout");
			}
			if (c == '\0')
			{
				c = delimiter;
			}
		}
		if (delimiter == '\0')
		{
			cnt++;
		}
		else if (c == delimiter)
		{
			cnt++;
		}
	}	
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		errx(1, "Invalid number of arguments");
	}

	char* interval;
	char left;
	char right;
	char init;
	char delimiter;

	if (strcmp(argv[1], "-c") == 0)
	{
		if (argc != 3)
		{
			errx(2, "Invalid number of arguments for argument -c");
		}
		
		interval = argv[2];
		delimiter = '\0';
	}
	else if(strcmp(argv[1], "-d") == 0)
	{
		if (argc != 5)
		{
			errx(9, "Invalid number of arguments for argument -d");
		}

		char* second = argv[2];
		if (strlen(second) == 0)
		{
			errx(12, "Second argument should be non-empty");
		}
		delimiter = second[0];

		if (strcmp(argv[3], "-f") != 0)
		{
			errx(13, "Third argument should be -f");
		}

		interval = argv[4];
	}
	else
	{
		errx(20, "First argument should be -c or -d");
	}

        if (strlen(interval) == 1)
        {
        	left = interval[0];
                right = left;
                if (left < '1' || left > '9')
                {
                	errx(3, "Incorrect second argument");
                }
        }
	else if (strlen(interval) == 3)
        {
        	left = interval[0];
        	init = interval[1];
        	right = interval[2];
        	if (init != '-')
        	{
        		errx(4, "Error in input second argument");
        	}
        	if (left == '0')
        	{
        		errx(5, "Left bound couldn't be 0");
        	}
        	if (right > '9')
        	{
        		errx(6, "Right bound couldn't be more than 9");
        	}
        	if (left >= right)
        	{
       			errx(7, "Wrong interval");
        	}
        }
	else
        {
        	errx(8, "Incorrect second argument");
        }

	cut (left, right, delimiter);
}
