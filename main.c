#include "ramfs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

char alphabet[53] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
int fd[26];
char buf[26][52];

void openall_succ(int flag) {
    char name[32];
    for (int i = 0; i < 26; i++) {
        sprintf(name, "/thisisthe%dthfile", i);
        assert((fd[i] = ropen(name, flag)) >= 0);
    }
}

void openall_fail(int flag) {
    char name[32];
    for (int i = 0; i < 26; i++) {
        sprintf(name, "/thisisthe%dthfile", i);
        assert((fd[i] = ropen(name, flag)) == -1);
    }
}

void readall(int len, int expect) {
    for (int i = 0; i < 26; i++) {
        assert(rread(fd[i], buf[i], len) == expect);
    }
}

void closeall(int expect) {
    for (int i = 0; i < 26; i++) {
        assert(rclose(fd[i]) == expect);
    }
}

void writeall(int off) {
    for (int i = 0; i < 26; i++) {
        assert(rwrite(fd[i], alphabet + ((i + off) % 26), 1) == 1);
    }
}
void writeall_fail(int off) {
    for (int i = 0; i < 26; i++) {
        assert(rwrite(fd[i], alphabet + ((i + off) % 26), 1) == -1);
    }
}

void seekall(int off, int whence, int expect) {
    for (int i = 0; i < 26; i++) {
        assert(rseek(fd[i], off, whence) == expect);
    }
}

int main() {
    init_ramfs();
    openall_succ(O_CREAT | O_RDWR | O_WRONLY);  // can't read
    getchar();
    readall(5, -1);
    for (int i = 0; i < 26; i++) {
        seekall(i, SEEK_SET, i);
        for (int j = 0; j < 26; j++) {
            writeall(j);
        }
        seekall(0, SEEK_END, 26 + i);
    }
    seekall(0, SEEK_END, 51);
    assert(rclose(genfd(fd, 26)) == -1);
    closeall(0);
    closeall(-1);
    openall_succ(O_RDONLY);
    readall(54, 51);
    for (int i = 0; i < 26; i++) {
        assert(memcmp(buf[i] + 25, alphabet + i, 26) == 0);
        for (int j = 0; j < 25; j++) {
            assert(buf[i][j] == alphabet[i]);
        }
    }
    closeall(0);
    for (int i = 0; i < 26; i++) {
        openall_succ(O_APPEND | O_RDWR);
        readall(5, 0);  // at rear
        writeall(i);
        closeall(0);
    }

    memset(buf, 0, sizeof(buf));
    openall_succ(O_RDONLY);
    seekall(0, SEEK_END, 77);
    seekall(51, SEEK_SET, 51);
    readall(52, 26);
    for (int i = 0; i < 26; i++) {
        assert(memcmp(buf[i], alphabet + i, 26) == 0);
    }
    closeall(0);
}

