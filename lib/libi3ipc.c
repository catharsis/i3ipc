#include "libi3ipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>

static int i3ipc_sock;
/* calls `i3 --get-socketpath` and returns the output
 * it is the responsibility of the caller to free() the
 * returned buffer 
 * */
char *get_sock_path(){
	char sockpath[PATH_MAX];
	FILE *fp = NULL;
	fp = popen("i3 --get-socketpath", "r");
	if (fp == NULL) {
		return NULL;
	}
	if (fgets(sockpath, sizeof(sockpath), fp) == NULL) {
		return NULL;
	}
	if (pclose(fp) == -1){
		perror("pclose");
		exit(1);
	}
	/*drop newline*/
	sockpath[strlen(sockpath)-1] = '\0';
	return strdup(sockpath);

}


/* connects to the UNIX socket at sockpath
 * if sockpath is NULL, attempts to retrieve path
 * automatically
 */
int i3ipc_sock_connect(const char *sockpath) {
	int len = 0;
	struct sockaddr_un i3sockaddr;
	char *path = NULL;

	if ((i3ipc_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	i3sockaddr.sun_family = AF_UNIX;
	if (!sockpath) {
		path = get_sock_path();
		strcpy(i3sockaddr.sun_path, path);
	}
	else {
		strcpy(i3sockaddr.sun_path, sockpath);
	}
	printf("Connecting to i3 socket at %s\n", i3sockaddr.sun_path);
	len = strlen(i3sockaddr.sun_path) + sizeof(i3sockaddr.sun_family);

	if(connect(i3ipc_sock, (struct sockaddr *)&i3sockaddr, len) == -1) {
		perror("connect");
		exit(1);
	}

	printf("Connected.\n");
	if (path)
		free(path);
	return 0;

}

void i3ipc_sock_disconnect(void) {
	if ( close(i3ipc_sock) != 0) {
		perror("close");
		exit(1);
	}
}
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

