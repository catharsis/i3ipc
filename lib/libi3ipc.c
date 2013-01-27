#include "libi3ipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

static int i3ipc_sock = -1;
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

/* disconnects from i3 socket or
 * exits on error
 */
void i3ipc_sock_disconnect(void) {
	if ( close(i3ipc_sock) != 0) {
		perror("close");
		exit(1);
	}
}

/* returns true if type is a known, valid
 * i3 IPC message type, returns false otherwise
 */
bool i3ipc_valid_type(uint32_t type) {
	switch(type) {
		/* switch on known message types,
		 * fallthrough intentional
		 * */
		case I3_IPC_MESSAGE_TYPE_COMMAND:
		case I3_IPC_MESSAGE_TYPE_GET_WORKSPACES:
		case I3_IPC_MESSAGE_TYPE_SUBSCRIBE:
		case I3_IPC_MESSAGE_TYPE_GET_OUTPUTS:
		case I3_IPC_MESSAGE_TYPE_GET_TREE:
		case I3_IPC_MESSAGE_TYPE_GET_MARKS:
		case I3_IPC_MESSAGE_TYPE_GET_BAR_CONFIG:
		case I3_IPC_MESSAGE_TYPE_GET_VERSION:
			return true;
		default: return false;
	}
}


i3ipc_msg *i3ipc_msg_create(uint32_t type, const char *payload) {
	i3ipc_msg *m = malloc(sizeof(i3ipc_msg));
	strcpy(m->magic, I3_IPC_MAGIC);
	if (!i3ipc_valid_type(type) ) {
		return NULL;
	}
	m->type = type;
	if (payload) {
		m->len = strlen(payload);
		m->payload = strndup(payload, m->len);
	}
	else {
		m->len = 0;
	}
	return m;
}

void i3ipc_msg_destroy(i3ipc_msg *msg) {
	free(msg->payload);
	free(msg);
}

void i3ipc_print_message(i3ipc_msg *msg) {
	if (!msg)
		return;
	printf("I3 IPC Message: %s,%d,%d,%s\n", msg->magic, msg->len, msg->type, msg->payload);
}

void i3ipc_send_message(uint32_t message_type, uint32_t payload_len) {
	int buffersz = sizeof(I3_IPC_MAGIC) + sizeof(uint32_t) + sizeof(uint32_t);

	char *buffer = malloc(buffersz);
	memcpy(buffer, I3_IPC_MAGIC, sizeof(I3_IPC_MAGIC));
	memcpy(buffer, &payload_len, sizeof(uint32_t));
	memcpy(buffer, &message_type, sizeof(uint32_t));
	/*TODO: payload not supported yet*/

	ssize_t sent_bytes = 0, n = 0;
	while (sent_bytes < buffersz) {
		n = send(i3ipc_sock, buffer+sent_bytes, (buffersz - sent_bytes), 0);
		if ( n == 0) {
			printf("peer (i3) closed connection (%d bytes sent)\n", (int)sent_bytes);
			exit(1);
		}
		else if ( n < 0) {
			perror("send");
			exit(1);
		}
		else
			sent_bytes += n;
	}
}

workspace *get_workspaces(void) {
	i3ipc_send_message(I3_IPC_MESSAGE_TYPE_GET_WORKSPACES, 0);
	workspace *root = malloc(sizeof(workspace));
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

