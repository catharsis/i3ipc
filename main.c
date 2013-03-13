#include "i3ipc.h"
#include <stdlib.h>
#include <stdio.h>
int main(void) {
	i3completions *completions;
	completions = i3ipc_get_completions(NULL);
	printf("completions: %s\n", completions->s);
	free(completions);
	return 0;
}
