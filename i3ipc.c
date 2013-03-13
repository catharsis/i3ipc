#define _GNU_SOURCE
#include <stdio.h>
#include "i3ipc.h"
int i3ipc_get_completions(char *s, char *into) {
	asprintf(&into, "foo");
	return 0;
}
