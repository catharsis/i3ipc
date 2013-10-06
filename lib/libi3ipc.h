#include "i3/ipc.h"
#include <stdint.h>
typedef struct __attribute__((packed)) {
	char magic[6];
	uint32_t len;
	uint32_t type;
} i3ipc_header;

typedef struct __attribute__((packed)) {
	i3ipc_header *header;
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
