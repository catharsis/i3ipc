#include <string.h>
#include <stdlib.h>
#include "i3ipc.h"
#include "lib/libi3ipc.h"
char *i3ipc_get_completions(const char *so) {
	char *s = NULL;
	i3ipc_sock_connect(NULL);
	i3Workspace *root = get_workspaces();
	i3ipc_sock_disconnect();
	s = strdup(workspace_name(root));
	destroy_workspaces(root);
	return s;
}
