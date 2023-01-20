#include "ramfs.h"
#include <assert.h>
#include <string.h>

int notin(int fd, int *fds, int n) {
    for (int i = 0; i < n; i++) {
        if (fds[i] == fd) return 0;
    }
    return 1;
}
int genfd(int *fds, int n) {
    for (int i = 0; i < 4096; i++) {
        if (notin(i, fds, n))
            return i;
    }
    return -1;
}

int main() {
    init_ramfs();
    int fd[10];
    int buf[10];
    assert(ropen("/abc==d", O_CREAT) == -1);
    assert((fd[0] = ropen("/0", O_RDONLY)) == -1);
    assert((fd[0] = ropen("/0", O_CREAT | O_WRONLY)) >= 0);
    assert((fd[1] = ropen("/1", O_CREAT | O_WRONLY)) >= 0);
    assert((fd[2] = ropen("/2", O_CREAT | O_WRONLY)) >= 0);
    assert((fd[3] = ropen("/3", O_CREAT | O_WRONLY)) >= 0);
    assert(rread(fd[0], buf, 1) == -1);
    assert(rread(fd[1], buf, 1) == -1);
    assert(rread(fd[2], buf, 1) == -1);
    assert(rread(fd[3], buf, 1) == -1);
    for (int i = 0; i < 100; i++) {
        assert(rwrite(fd[0], "\0\0\0\0\0", 5) == 5);
        assert(rwrite(fd[1], "hello", 5) == 5);
        assert(rwrite(fd[2], "world", 5) == 5);
        assert(rwrite(fd[3], "\x001\x002\x003\x0fe\x0ff", 5) == 5);
    }
    assert(rclose(fd[0]) == 0);
    assert(rclose(fd[1]) == 0);
    assert(rclose(fd[2]) == 0);
    assert(rclose(fd[3]) == 0);
    assert(rclose(genfd(fd, 4)) == -1);
    assert((fd[0] = ropen("/0", O_CREAT | O_RDONLY)) >= 0);
    assert((fd[1] = ropen("/1", O_CREAT | O_RDONLY)) >= 0);
    assert((fd[2] = ropen("/2", O_CREAT | O_RDONLY)) >= 0);
    assert((fd[3] = ropen("/3", O_CREAT | O_RDONLY)) >= 0);
    assert(rwrite(fd[0], buf, 1) == -1);
    assert(rwrite(fd[1], buf, 1) == -1);
    assert(rwrite(fd[2], buf, 1) == -1);
    assert(rwrite(fd[3], buf, 1) == -1);
    for (int i = 0; i < 50; i++) {
        assert(rread(fd[0], buf, 10) == 10);
        assert(memcmp(buf, "\0\0\0\0\0\0\0\0\0\0", 10) == 0);
        assert(rread(fd[1], buf, 10) == 10);
        assert(memcmp(buf, "hellohello", 10) == 0);
        assert(rread(fd[2], buf, 10) == 10);
        assert(memcmp(buf, "worldworld", 10) == 0);
        assert(rread(fd[3], buf, 10) == 10);
        assert(memcmp(buf, "\x001\x002\x003\x0fe\x0ff\x001\x002\x003\x0fe\x0ff", 10)
               == 0);
    }
    assert(rread(fd[0], buf, 10) == 0);
    assert(rread(fd[1], buf, 10) == 0);
    assert(rread(fd[2], buf, 10) == 0);
    assert(rread(fd[3], buf, 10) == 0);
    assert(rclose(fd[0]) == 0);
    assert(rclose(fd[1]) == 0);
    assert(rclose(fd[2]) == 0);
    assert(rclose(fd[3]) == 0);
    return 0;

    }
