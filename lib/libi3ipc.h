typedef char* workspace_name;
typedef struct workspace {
	workspace_name name;
	struct workspace *next;
} workspace;

workspace *get_workspaces(void);
void destroy_workspaces(workspace *root);
