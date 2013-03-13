#include "i3ipc.h"
#include <stdlib.h>
#include <stdio.h>
int main(void) {
	char *completions = NULL;
	i3ipc_get_completions(NULL, completions);
	printf("completions: %s\n", completions);
	free(completions);
	return 0;
}
