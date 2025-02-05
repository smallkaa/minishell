#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void handler(int signum)
{
	printf("Received signal %d\n", signum);
	exit(1);
}

int main()
{
	signal(SIGINT, handler); // Handle Ctrl+C
	while (1); // Infinite loop
	return (0);
}
