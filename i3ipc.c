#include <stdio.h>
#include <stdlib.h>
#include "i3ipc.h"
i3completions *i3ipc_get_completions(const char *so) {
	i3completions *compl = malloc(sizeof(i3completions));
	compl->s = malloc(1024);
	sprintf(compl->s, "foo");
	return compl;
}

int i3ipc_destroy_completions(i3completions *compl) {
	free(compl->s);
	free(compl);
	return 0;
}
