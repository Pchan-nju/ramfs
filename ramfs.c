#include "ramfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct File
{
	char name[35];
	int descriptor;
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
    if (strlen(pathname) > 1024)
        return -1;
    int i = 0;
    char str[35];
    int len = 0;
    rFile * ptr = root;
    while (pathname[i] != '\0') {
        if (pathname[i] == '/') {
            if (len != 0) {
                int j = i + 1;
                bool isLastOne = true;
                while (pathname[j] != '\0') {
                    if (pathname[j] != '/') {
                        isLastOne = false;
                        break;
                    }
                    j++;
                }
                if (isLastOne) {
                    rFile * newDir = (rFile *)malloc(sizeof(rFile));
                    strcpy(newDir->name, str);
                    newDir->nextFile = ptr->sonFile;
                    ptr->sonFile = newDir;
                }
                else {
                    rFile * p = ptr->sonFile;
                    while (p != NULL)
                    {
                        if (strcmp(p->name, str) == 0) {
                            ptr = p;
                            break;
                        }
                    }
                }
                len = 0;
            }
        }
        else {
            if ((pathname[i] < '0' || pathname[i] > '9') &&
                (pathname[i] < 'a' || pathname[i] > 'z') &&
                (pathname[i] < 'A' || pathname[i] > 'Z') &&
                pathname[i] != '.')
                return -1;
            str[len] = pathname[i];
            len ++;
            if (len > 32)
                return  -1;
        }
        i++;
    }
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
    root->descriptor = 1;
}
