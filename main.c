#include "i3ipc.h"
#include <stdlib.h>
#include <stdio.h>
int main(void) {
	char *comp;
	comp = i3ipc_get_completions(NULL);
	printf("completions: %s\n", comp);
	return 0;
}
