//Да се напише програма на C, която създава процес дете и демонстрира принцина на конкурентност при процесите
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

int main()
{
	int n = 1000000;

	if (fork() > 0)
	{
		for (int i = 0; i < n; i++)
		{
			printf("Parent\n");
		}
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			printf("Child\n");
		}
	}

	exit (0);
}
