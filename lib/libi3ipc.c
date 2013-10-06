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
	memset(&i3sockaddr, 0, sizeof(i3sockaddr));
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
	len = strlen(i3sockaddr.sun_path) + sizeof(i3sockaddr.sun_family);

	if(connect(i3ipc_sock, (struct sockaddr *)&i3sockaddr, len) == -1) {
		perror("connect");
		exit(1);
	}

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
	i3ipc_msg *m = calloc(1, sizeof(i3ipc_msg));
	m->header = calloc(1, sizeof(i3ipc_header));
	m->header->magic[0] = 'i';
	m->header->magic[1] = '3';
	m->header->magic[2] = '-';
	m->header->magic[3] = 'i';
	m->header->magic[4] = 'p';
	m->header->magic[5] = 'c';
	if (!i3ipc_valid_type(type) ) {
		return NULL;
	}
	m->header->type = type;
	if (payload) {
		m->header->len = strlen(payload);
		m->payload = strndup(payload, m->header->len);
	}
	else {
		m->header->len = 0;
		m->payload = NULL;
	}
	return m;
}

void i3ipc_msg_destroy(i3ipc_msg *msg) {
	free(msg->payload);
	free(msg->header);
	free(msg);
}

void i3ipc_print_message(i3ipc_msg *msg) {
	if (!msg)
		return;
	printf("I3 IPC Message: %s,%d,%d,%s\n", msg->header->magic, msg->header->len, msg->header->type, msg->payload);
}

void i3ipc_receive_message() {
	/*get header to figure out length*/
	size_t headersz = sizeof(I3_IPC_MAGIC) + sizeof(uint32_t) + sizeof(uint32_t);
	void *tmp, *buffer = malloc(headersz);
	tmp = buffer;
	ssize_t received_bytes = 0, n = 0;
	while (received_bytes < headersz) {
		n = recv(i3ipc_sock, buffer + received_bytes, headersz-received_bytes, 0);
		if ( n == 0) {
			fprintf(stderr, "peer (i3) closed connection (%d bytes received)\n", (int)received_bytes);
			exit(1);
		}
		else if ( n < 0  && errno != EAGAIN) {
			perror("recv");
			exit(1);
		}
		else {
			received_bytes += n;
		}
	}
	free(tmp);
}
void i3ipc_send_message(uint32_t message_type, uint32_t payload_len) {
	i3ipc_msg *message = i3ipc_msg_create(message_type, NULL);
	if (!message) {
		fprintf(stderr, "Failed to create message\n");
	}
	uint32_t buffersz = sizeof(i3ipc_header);
	ssize_t sent_bytes = 0, n = 0;
	while (sent_bytes < buffersz) {
		n = send(i3ipc_sock, (void*)(message->header+sent_bytes), (buffersz - sent_bytes), 0);
		if ( n == 0) {
			fprintf(stderr, "peer (i3) closed connection (%d bytes sent)\n", (int)sent_bytes);
			exit(1);
		}
		else if ( n < 0 && errno != EAGAIN) {
			perror("send");
			exit(1);
		}
		else
			sent_bytes += n;
	}
	i3ipc_msg_destroy(message);
}

workspace *get_workspaces(void) {
	i3ipc_send_message(I3_IPC_MESSAGE_TYPE_GET_WORKSPACES, 0);
	workspace *root = malloc(sizeof(workspace));
	root->next = NULL;
	root->name = strdup("foo");
	i3ipc_receive_message();
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

