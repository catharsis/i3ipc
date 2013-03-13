typedef struct {
	char *s;
} i3completions;
i3completions *i3ipc_get_completions(const char *s);
int i3ipc_destroy_completions(i3completions *compl);
