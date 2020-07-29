//Да се напише програма на C, която изпълнява команда date
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

int main()
{
	if (execl("/bin/date", "date", (char *) NULL) == -1)
	{
		err(1, "Cannot execute command date");
	}
	else //if execl is successful, it won't come in this case
	{
		printf("Sucess");
	}
	
	exit(0);
}
