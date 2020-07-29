//Да се напише програма на C, която изпълнява команда ls с точно един аргумент
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "Invalid number of arguments");
	}

	if (execlp("ls", "ls", argv[1], (char *) NULL) == -1)
	{
		err(2, "Cannot execute command ls");
	}

	exit(0);
}
