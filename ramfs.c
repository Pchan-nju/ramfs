#include "ramfs.h"
/* modify this file freely */

int ropen(const char *pathname, int flags) {
  // TODO();
}

int rclose(int fd) {
  // TODO();
}

ssize_t rwrite(int fd, const void *buf, size_t count) {
  // TODO();
}

ssize_t rread(int fd, void *buf, size_t count) {
  // TODO();
}

off_t rseek(int fd, off_t offset, int whence) {
  // TODO();
}

int rmkdir(const char *pathname) {
  // TODO();
}

int rrmdir(const char *pathname) {
  // TODO();
}

int runlink(const char *pathname) {
  // TODO();
}

void init_ramfs() {
  // TODO();
}
