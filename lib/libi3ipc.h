#include "i3/ipc.h"
#include <stdint.h>
#define MAGIC_LEN (sizeof(I3_IPC_MAGIC)/sizeof(char))
typedef struct {
	char magic[MAGIC_LEN];
	uint32_t len;
	uint32_t type;
	char *payload;
} i3ipc_msg;

int i3ipc_sock_connect(const char *sockpath);
void i3ipc_sock_disconnect(void);

typedef char* workspace_name;
typedef struct workspace {
	workspace_name name;
	struct workspace *next;
} workspace;

workspace *get_workspaces(void);
void destroy_workspaces(workspace *root);
