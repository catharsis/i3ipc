#include <string.h>
#include <stdlib.h>
#include "i3ipc.h"
#include "lib/libi3ipc.h"
i3completions *i3ipc_get_completions(const char *so) {
	workspace *root = get_workspaces();
	i3completions *compl = malloc(sizeof(i3completions));
	compl->s = strdup(root->name);
	return compl;
}

int i3ipc_destroy_completions(i3completions *compl) {
	free(compl->s);
	free(compl);
	return 0;
}
