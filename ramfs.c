#include "ramfs.h"
#include <string.h>
#include <stdlib.h>
#include "stdio.h"

typedef struct File
{
	char name[32];
	int index;
	struct File *sonFile, *nextFile;
} rFile;
rFile *root;

int ropen(const char *pathname, int flags) {
    // TODO();
}

int rclose(int fd) {
}

ssize_t rwrite(int fd, const void *buf, size_t count) {
}

ssize_t rread(int fd, void *buf, size_t count) {
}

off_t rseek(int fd, off_t offset, int whence) {
}

int rmkdir(const char *pathname) {
    printf("index : %d", root -> index);
}

int rrmdir(const char *pathname) {
}

int runlink(const char *pathname) {
}

void init_ramfs() {
    root = (rFile *)malloc(sizeof(rFile));
    strcpy(root->name, "root");
    root->nextFile = NULL;
    root->sonFile = NULL;
    root->index = 1;
}
