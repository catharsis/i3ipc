#include "libi3ipc.h"
#include <stdlib.h>
#include <stdio.h>
workspace *get_workspaces(void) {
	workspace *root = malloc(sizeof(workspace*));
	root->next = NULL;
	root->name = malloc(128);
	sprintf(root->name, "temp workspace name");
	return root;
}

void destroy_workspaces(workspace *root) {
	workspace *tmp;
	while(root != NULL) {
		tmp = root;
		root = root->next;
		free(tmp->name);
		free(tmp);
	}

}
