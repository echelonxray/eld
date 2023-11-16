#include "util.h"
#include <string.h>

int strcmp_prefix(char* str, char* prefix) {
	size_t prefix_len;
	prefix_len = strlen(prefix);
	return strncmp(str, prefix, prefix_len);
}
