//Да се напише програма на C, която изпълнява команда sleep (за 60 секунди)
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main()
{
	if (execlp("sleep", "simona", "60", 0) == -1)
	{
		err(1, "Cannot execute command sleep");
	}

	exit(0);
}
