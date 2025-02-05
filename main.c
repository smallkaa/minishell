#include <stdio.h>

extern char **environ;

void mini_env(void) {
	for (int i = 0; environ[i]; i++) {
		printf("%s\n", environ[i]);
	}
}

int main() {
	mini_env();
	return 0;
}
