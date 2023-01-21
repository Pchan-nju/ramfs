#include "ramfs.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef LOCAL
#include <assert.h>
#else
#define assert(cond)                                                           \
  do {                                                                         \
    if (cond)                                                                  \
      ;                                                                        \
    else {                                                                     \
      puts("false");                                                           \
      exit(EXIT_SUCCESS);                                                      \
    }                                                                          \
  } while (0)
#endif
#define KB * 1024
#define MB KB * 1024
#define PGSIZE 1 KB
#define SCALE 1024

#define test(func, expect, ...) assert(func(__VA_ARGS__) == expect)
#define succopen(var, ...) assert((var = ropen(__VA_ARGS__)) >= 0)
#define failopen(var, ...) assert((var = ropen(__VA_ARGS__)) == -1)

void gen_random(char *pg) {
    int *p = (int *)pg;
    for (int i = 0; i < PGSIZE / 4; i++) {
        p[i] = rand();
    }
}

int notin(int fd, int *fds, int n) {
    for (int i = 0; i < n; i++) {
        if (fds[i] == fd)
            return 0;
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

int fd[SCALE];
uint8_t buf[1 MB];
uint8_t ref[1 MB];

int main() {
    srand(time(NULL));
    init_ramfs();

    /* short */
    test(rmkdir, 0, "/a/");
    test(rmkdir, 0, "/a/b/");
    test(rmkdir, 0, "/a/b/c");
    test(rmkdir, -1, "//a/b");
    test(rmkdir, -1, "/a/b/c/");
    test(rmkdir, 0, "/a/d");
    test(rmkdir, -1, "/a/b/c/d/e");
    test(rmkdir, 0, "/a/b/c/d");
    test(rmkdir, 0, "/a/b/c/d/e");
    test(rmkdir, 0, "/a//c");
    test(rmkdir, 0, "/a/c/b");
    test(rmkdir, 0, "/a/c/c");
    test(rmkdir, 0, "/a/c/d");
    test(rmkdir, 0, "/a/c/d/e");

    /* long */
    test(rmkdir, 0, "/00000000000000000000000000000001/");
    test(rmkdir, 0,
         "/00000000000000000000000000000001/00000000000000000000000000000002/");
    test(rmkdir, 0,
         "/00000000000000000000000000000001/00000000000000000000000000000002/"
         "00000000000000000000000000000003");
    test(rmkdir, -1,
         "//00000000000000000000000000000001/00000000000000000000000000000002");
    test(rmkdir, -1,
         "/00000000000000000000000000000001/00000000000000000000000000000002/"
         "00000000000000000000000000000003/");
    test(rmkdir, 0,
         "/00000000000000000000000000000001/00000000000000000000000000000004");
    test(rmkdir, -1,
         "/00000000000000000000000000000001/00000000000000000000000000000002/"
         "00000000000000000000000000000003/00000000000000000000000000000004/"
         "00000000000000000000000000000005");
    test(rmkdir, 0,
         "/00000000000000000000000000000001/00000000000000000000000000000002/"
         "00000000000000000000000000000003/00000000000000000000000000000004");
    test(rmkdir, 0,
         "/00000000000000000000000000000001/00000000000000000000000000000002/"
         "00000000000000000000000000000003/00000000000000000000000000000004/"
         "00000000000000000000000000000005");

#ifndef REF
    /* more than long */
    test(rmkdir, -1, "/000000000000000000000000000000001");
    /* illegal */
    test(rmkdir, -1, "/000000000=0000000000000000000001");
    test(rmkdir, -1, "/000000000*0000000000000000000001");
    test(rmkdir, -1, "/000000000-0000000000000000000001");
    test(rmkdir, -1, "/000000000/0000000000000000000001");
    test(rmkdir, -1, "/0000000000000000000000000000001\x001");
    /* not started with / */
    test(rmkdir, -1, "abcdefghijklmn");
#endif

    /* a nice tree, haha */
    test(rmkdir, 0, "/never");
    test(rmkdir, 0, "/never/gonna");
    test(rmkdir, 0, "/never/gonna/give");
    test(rmkdir, 0, "/never/gonna/give/you");
    test(rmkdir, 0, "/never/gonna/give/you/up");
    test(rmkdir, 0, "/never/gonna/let");
    test(rmkdir, 0, "/never/gonna/let/you");
    test(rmkdir, 0, "/never/gonna/let/you/down");
    test(rmkdir, 0, "/never/gonna/run");
    test(rmkdir, 0, "/never/gonna/run/around");
    test(rmkdir, 0, "/never/gonna/and");
    test(rmkdir, 0, "/never/gonna/desert");
    test(rmkdir, 0, "/never/gonna/desert/you");
    test(rmkdir, 0, "/never/gonna/make");
    test(rmkdir, 0, "/never/gonna/make/you");
    test(rmkdir, 0, "/never/gonna/make/you/cry");
    test(rmkdir, 0, "/never/gonna/say");
    test(rmkdir, 0, "/never/gonna/say/goodbye");
    test(rmkdir, 0, "/never/gonna/tell");
    test(rmkdir, 0, "/never/gonna/tell/a");
    test(rmkdir, 0, "/never/gonna/tell/a/lie");
    test(rmkdir, -1, "/never/gonna/and");
    test(rmkdir, 0, "/never/gonna/hurt");
    test(rmkdir, 0, "/never/gonna/hurt/you");

#ifndef REF
    /* you can't escape this */
    /* not the same with linux syscall_open */
    succopen(fd[1 ], "/never", O_CREAT);
    succopen(fd[2 ], "/never/gonna", O_CREAT);
    succopen(fd[3 ], "/never/gonna/give", O_CREAT);
    succopen(fd[4 ], "/never/gonna/give/you", O_CREAT);
    succopen(fd[5 ], "/never/gonna/give/you/up", O_CREAT);
    succopen(fd[6 ], "/never/gonna/let", O_CREAT);
    succopen(fd[7 ], "/never/gonna/let/you", O_CREAT);
    succopen(fd[8 ], "/never/gonna/let/you/down", O_CREAT);
    succopen(fd[9 ], "/never/gonna/run", O_CREAT);
    succopen(fd[10], "/never/gonna/run/around", O_CREAT);
    succopen(fd[11], "/never/gonna/and", O_CREAT);
    succopen(fd[12], "/never/gonna/desert", O_CREAT);
    succopen(fd[13], "/never/gonna/desert/you", O_CREAT);
    succopen(fd[14], "/never/gonna/make", O_CREAT);
    succopen(fd[15], "/never/gonna/make/you", O_CREAT);
    succopen(fd[16], "/never/gonna/make/you/cry", O_CREAT);
    succopen(fd[17], "/never/gonna/say", O_CREAT);
    succopen(fd[18], "/never/gonna/say/goodbye", O_CREAT);
    succopen(fd[19], "/never/gonna/tell", O_CREAT);
    succopen(fd[20], "/never/gonna/tell/a", O_CREAT);
    succopen(fd[21], "/never/gonna/tell/a/lie", O_CREAT);
    succopen(fd[22], "/never/gonna/and", O_CREAT);
    succopen(fd[23], "/never/gonna/hurt", O_CREAT);
    succopen(fd[0 ], "/never/gonna/hurt/you", O_CREAT);
    for (int i = 0; i < 24; i++) {
        test(rread, -1, fd[i], buf, 0);
        test(rwrite, -1, fd[i], buf, 0);
        test(rclose, 0, fd[i]);
        /* read, write illegal fd */
        test(rread, -1, fd[i], buf, 0);
        test(rwrite, -1, fd[i], buf, 0);
    }
#endif

    /* first round */
    test(rrmdir, -1, "/never");
    test(rrmdir, -1, "/never/gonna");
    test(rrmdir, -1, "/never/gonna/give");
    test(rrmdir, -1, "/never/gonna/give/you");
    test(rrmdir, 0, "/never/gonna/give/you/up");
    test(rrmdir, -1, "/never/gonna/let");
    test(rrmdir, -1, "/never/gonna/let/you");
    test(rrmdir, 0, "/never/gonna/let/you/down");
    test(rrmdir, -1, "/never/gonna/run");
    test(rrmdir, 0, "/never/gonna/run/around");
    test(rrmdir, 0, "/never/gonna/and");
    test(rrmdir, -1, "/never/gonna/desert");
    test(rrmdir, 0, "/never/gonna/desert/you");
    test(rrmdir, -1, "/never/gonna/make");
    test(rrmdir, -1, "/never/gonna/make/you");
    test(rrmdir, 0, "/never/gonna/make/you/cry");
    test(rrmdir, -1, "/never/gonna/say");
    test(rrmdir, 0, "/never/gonna/say/goodbye");
    test(rrmdir, -1, "/never/gonna/tell");
    test(rrmdir, -1, "/never/gonna/tell/a");
    test(rrmdir, 0, "/never/gonna/tell/a/lie");
    test(rrmdir, -1, "/never/gonna/and");
    test(rrmdir, -1, "/never/gonna/hurt");
    test(rrmdir, 0, "/never/gonna/hurt/you");

    /* first round deleted:
       give you up
       let you down
       run around
       and
       desert you
    create and unlink files */

    int f = 0;
    succopen(f, "/never/gonna/give/you/up", O_CREAT | O_WRONLY);
    /* able to read and write */
    test(rwrite, 5, f, "hello", 5);
    test(rclose, 0, f);
    succopen(f, "/never/gonna/give/you/up", O_CREAT);
    test(rread, 5, f, buf, 5);
    assert(memcmp(buf, "hello", 5) == 0);
    test(rclose, 0, f);
    /* can't rmdir file */
    test(rrmdir, -1, "/never/gonna/give/you/up");
    /* can't rmdir non-empty dir */
    test(rrmdir, -1, "/never/gonna/give/you");
    test(runlink, 0, "/never/gonna/give/you/up");
    /* can't open absent file */
    failopen(f, "/never/gonna/give/you/up", O_RDONLY);

    /* second round */
    test(rrmdir, -1, "/never");
    test(rrmdir, -1, "/never/gonna");
    test(rrmdir, -1, "/never/gonna/give");
    /* can't unlink dir */
    test(runlink, -1, "/never/gonna/give/you");
    test(rrmdir, 0, "/never/gonna/give/you");
    test(rrmdir, -1, "/never/gonna/give/you/up");
    test(rrmdir, -1, "/never/gonna/let");
    test(rrmdir, 0, "/never/gonna/let/you");
    test(rrmdir, -1, "/never/gonna/let/you/down");
    test(rrmdir, 0, "/never/gonna/run");
    test(rrmdir, -1, "/never/gonna/run/around");
    test(rrmdir, -1, "/never/gonna/and");
    test(rrmdir, 0, "/never/gonna/desert");
    test(rrmdir, -1, "/never/gonna/desert/you");
    test(rrmdir, -1, "/never/gonna/make");
    test(rrmdir, 0, "/never/gonna/make/you");
    test(rrmdir, -1, "/never/gonna/make/you/cry");
    test(rrmdir, 0, "/never/gonna/say");
    test(rrmdir, -1, "/never/gonna/say/goodbye");
    test(rrmdir, -1, "/never/gonna/tell");
    test(rrmdir, 0, "/never/gonna/tell/a");
    test(rrmdir, -1, "/never/gonna/tell/a/lie");
    test(rrmdir, -1, "/never/gonna/and");
    test(rrmdir, 0, "/never/gonna/hurt");
    test(rrmdir, -1, "/never/gonna/hurt/you");

    /* third round */
    test(rrmdir, -1, "/never");
    test(rrmdir, -1, "/never/gonna");
    test(rrmdir, 0, "/never/gonna/give");
    test(rrmdir, -1, "/never/gonna/give/you");
    test(rrmdir, -1, "/never/gonna/give/you/up");
    test(rrmdir, 0, "/never/gonna/let");
    test(rrmdir, -1, "/never/gonna/let/you");
    test(rrmdir, -1, "/never/gonna/let/you/down");
    test(rrmdir, -1, "/never/gonna/run");
    test(rrmdir, -1, "/never/gonna/run/around");
    test(rrmdir, -1, "/never/gonna/and");
    test(rrmdir, -1, "/never/gonna/desert");
    test(rrmdir, -1, "/never/gonna/desert/you");
    test(rrmdir, 0, "/never/gonna/make");
    test(rrmdir, -1, "/never/gonna/make/you");
    test(rrmdir, -1, "/never/gonna/make/you/cry");
    test(rrmdir, -1, "/never/gonna/say");
    test(rrmdir, -1, "/never/gonna/say/goodbye");
    test(rrmdir, 0, "/never/gonna/tell");
    test(rrmdir, -1, "/never/gonna/tell/a");
    test(rrmdir, -1, "/never/gonna/tell/a/lie");
    test(rrmdir, -1, "/never/gonna/and");
    test(rrmdir, -1, "/never/gonna/hurt");
    test(rrmdir, -1, "/never/gonna/hurt/you");

    /* fourth round */
    test(rrmdir, -1, "/never");
    test(rrmdir, 0, "/never/gonna");
    test(rrmdir, -1, "/never/gonna/give");
    test(rrmdir, -1, "/never/gonna/give/you");
    test(rrmdir, -1, "/never/gonna/give/you/up");
    test(rrmdir, -1, "/never/gonna/let");
    test(rrmdir, -1, "/never/gonna/let/you");
    test(rrmdir, -1, "/never/gonna/let/you/down");
    test(rrmdir, -1, "/never/gonna/run");
    test(rrmdir, -1, "/never/gonna/run/around");
    test(rrmdir, -1, "/never/gonna/and");
    test(rrmdir, -1, "/never/gonna/desert");
    test(rrmdir, -1, "/never/gonna/desert/you");
    test(rrmdir, -1, "/never/gonna/make");
    test(rrmdir, -1, "/never/gonna/make/you");
    test(rrmdir, -1, "/never/gonna/make/you/cry");
    test(rrmdir, -1, "/never/gonna/say");
    test(rrmdir, -1, "/never/gonna/say/goodbye");
    test(rrmdir, -1, "/never/gonna/tell");
    test(rrmdir, -1, "/never/gonna/tell/a");
    test(rrmdir, -1, "/never/gonna/tell/a/lie");
    test(rrmdir, -1, "/never/gonna/and");
    test(rrmdir, -1, "/never/gonna/hurt");
    test(rrmdir, -1, "/never/gonna/hurt/you");

    /* fifth round */
    test(rrmdir, 0, "/never");
    test(rrmdir, -1, "/never/gonna");

    /* sixth round */
    test(rrmdir, -1, "/never");

    /* can't have subdir in file */
    succopen(f, "/never", O_CREAT);
    test(rclose, 0, f);
    test(rmkdir, -1, "/never/gonna");
    test(rmkdir, -1, "/never/gonna/give");
    test(rmkdir, -1, "/never/gonna/give/you");
    test(rrmdir, -1, "/never/gonna");
    test(rrmdir, -1, "/never/gonna/give");
    test(rrmdir, -1, "/never/gonna/give/you");

    puts("true");
}
