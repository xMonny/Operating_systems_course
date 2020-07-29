//Да се напише програма на C, която е аналогична на горния пример, но принуждава бащата да изчака сина си да завърши
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main()
{
	int n = 1000000;
	pid_t a = fork();
	int status;

	if (a > 0)
	{
		wait(&status);
		for (int i = 0; i < n; i++)
		{
			printf("Parent\n");
		}
	}
	else if (a == 0)
	{
		for (int i = 0; i < n; i++)
		{
			printf("Child\n");
		}
	}

	exit(0);
}
