#include "i3/ipc.h"
#include <stdint.h>
typedef struct i3ipc_msg i3ipc_msg;
typedef struct i3Workspace i3Workspace;

int i3ipc_sock_connect(const char *sockpath);
void i3ipc_sock_disconnect(void);

i3Workspace *get_workspaces(void);
void destroy_workspaces(i3Workspace *root);
const char *workspace_name(const i3Workspace *workspace);
