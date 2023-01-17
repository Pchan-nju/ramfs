#include "ramfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct File
{
	char name[35];
    bool type; // direction - true, file - false;
	struct File *sonFile, *nextFile;
} rFile;

rFile *root;

typedef struct Descritptor {
    int desIndex;
    int flag;
    rFile * tarFile;
    struct Descritptor * nextDes;
} rDescriptor;

rDescriptor * desHead = NULL;

int ropen(const char *pathname, int flags) {
    static int des_cnt = 1;
    des_cnt++;

    printf("ropen(\"%s\", %o): \n", pathname, flags);
    if(pathname[0] != '/') {
        printf(("Error : the pathname is not started with '/' \n"));
        return -1;
    }
    if (strlen(pathname) > 1024) {
        printf("Error : the pathname is too long\n");
        return -1;
    }

    int i = 0;
    char str[35];
    int len = 0;
    rFile * ptr = root;
    while (pathname[i] != '\0') {
        if (pathname[i] == '/') {
            if (len != 0) {
                str[len] = '\0';
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
                    // TODO();
                    // Is it legal to create a new direction?
                    rFile * p = ptr->sonFile;
                    while(p != NULL) {
                        if (strcmp(p->name, str) == 0) {
                            if(p->type) {
                                rDescriptor *newDes = (rDescriptor *) malloc(sizeof(rDescriptor));
                                newDes->desIndex = des_cnt;
                                newDes->flag = flags;
                                newDes->tarFile = p;
                                if (desHead == NULL) {
                                    desHead = newDes;
                                    newDes->nextDes = NULL;
                                } else {
                                    newDes->nextDes = desHead;
                                    desHead = newDes;
                                }
                                printf("Success.\n");
                                return des_cnt;
                            } else {
                                printf("Error : file should not end with '/'.\n");
                                return -1;
                            }
                        }
                        p = p->nextFile;
                    }
                    if (flags & O_CREAT) {
                        printf("Error : file should not end with '/'.\n");
                        return -1;
                    } else {
                        printf("Error : there is no such file.\n");
                        return -1;
                    }
                } else {
                    rFile * p = ptr->sonFile;
                    bool flag = false;
                    while (p != NULL)
                    {
                        if (strcmp(p->name, str) == 0) {
                            flag = true;
                            ptr = p;
                            break;
                        }
                        p = p->nextFile;
                    }
                    if (!flag) {
                        printf("Error : there is no such path.\n");
                        return -1;
                    }
                    if (!ptr->type) {
                        printf("Error : '%s' is not a direction.\n",str);
                        return -1;
                    }
                }
                memset(str, 0, sizeof(str));
                len = 0;
            }
        }
        else {
            if ((pathname[i] < '0' || pathname[i] > '9') &&
                (pathname[i] < 'a' || pathname[i] > 'z') &&
                (pathname[i] < 'A' || pathname[i] > 'Z') &&
                pathname[i] != '.') {
                printf("Error : pathname is illegal.\n");
                return -1;
            }

            str[len] = pathname[i];
            len ++;

            if (len > 32) {
                printf("Error : the direction name is too long.\n");
                return -1;
            }
        }
        i++;
    }
    if (len != 0) {
        rFile * p = ptr->sonFile;
        while(p != NULL) {
            if (strcmp(p->name, str) == 0) {
//                p->right = flags;
//                Descriptor[des_cnt] = p;
                rDescriptor * newDes = (rDescriptor *) malloc(sizeof(rDescriptor));
                newDes->desIndex = des_cnt;
                newDes->flag = flags;
                newDes->tarFile = p;
                if(desHead == NULL) {
                    desHead = newDes;
                    newDes->nextDes = NULL;
                } else {
                    newDes->nextDes = desHead;
                    desHead = newDes;
                }
                return des_cnt;
            }
            p = p->nextFile;
        }

        if (flags & O_CREAT) {
            rFile *newFile = (rFile *) malloc(sizeof(rFile));
            for (int k = 0; k <= len; k++) // strcpy
                newFile->name[k] = str[k];
            newFile->nextFile = ptr->sonFile;
            newFile->type = false;
            ptr->sonFile = newFile;

            rDescriptor * newDes = (rDescriptor *) malloc(sizeof(rDescriptor));
            newDes->desIndex = des_cnt;
            newDes->flag = flags;
            newDes->tarFile = p;
            if(desHead == NULL) {
                desHead = newDes;
                newDes->nextDes = NULL;
            } else {
                newDes->nextDes = desHead;
                desHead = newDes;
            }

            printf("Success.\n");
            return des_cnt;
        }
        else {
            printf("Error : there is no such file and it is not allowed to create.\n");
            return -1;
        }
    }
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
    static int cnt = 0;
    cnt++;
    printf("rmkdir(\"%s\"): \n", pathname);
    if(pathname[0] != '/') {
        printf(("Error : the pathname is not started with '/' \n"));
        return -1;
    }
    if (strlen(pathname) > 1024) {
        printf("Error : the pathname is too long\n");
        return -1;
    }
    int i = 0;
    char str[35];
    int len = 0;
    rFile * ptr = root;
    while (pathname[i] != '\0') {
        if (pathname[i] == '/') {
            if (len != 0) {
                str[len] = '\0';
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
                    rFile * p = ptr->sonFile;
                    while(p != NULL)
                    {
                        if (strcmp(p->name, str) == 0) {
                            printf("Error : the direction has existed\n");
                            return -1;
                        }
                        p = p->nextFile;
                    }
                    rFile * newDir = (rFile *)malloc(sizeof(rFile));
                    for(int k = 0; k <= len; k++) // strcpy(newDir->name, str);
                        newDir->name[k] = str[k];
                    newDir->type = true;
                    newDir->nextFile = ptr->sonFile;
                    ptr->sonFile = newDir;
                }
                else {
                    rFile * p = ptr->sonFile;
                    bool flag = false;
                    while (p != NULL)
                    {
                        if (strcmp(p->name, str) == 0) {
                            flag = true;
                            ptr = p;
                            break;
                        }
                        p = p->nextFile;
                    }
                    if (!flag) {
                        printf("Error : there is no such path\n");
                        return -1;
                    }
                    if (!ptr->type) {
                        printf("Error : '%s' is not a direction.\n", str);
                        return -1;
                    }
                }
                memset(str, 0, sizeof(str));
                len = 0;
            }
        }
        else {
            if ((pathname[i] < '0' || pathname[i] > '9') &&
                (pathname[i] < 'a' || pathname[i] > 'z') &&
                (pathname[i] < 'A' || pathname[i] > 'Z') &&
                pathname[i] != '.') {
                printf("Error : pathname is illegal\n");
                return -1;
            }

            str[len] = pathname[i];
            len ++;

            if (len > 32) {
                printf("Error : the direction name is too long\n");
                return -1;
            }
        }
        i++;
    }
    if (len != 0) {
        rFile * p = ptr->sonFile;
        str[len] = '\0';
        printf("len = %d && str = %s\n",len,str);
        while(p != NULL) {
            if (strcmp(p->name, str) == 0) {
                printf("Error : the direction has existed\n");
                return -1;
            }
            p = p->nextFile;
        }
        rFile * newDir = (rFile *)malloc(sizeof(rFile));
//        strcpy(newDir->name, str);
        for(int k = 0; k <= len; k++)
            newDir->name[k] = str[k];
        newDir->type = true;
        newDir->nextFile = ptr->sonFile;
        ptr->sonFile = newDir;
    }
    printf("Success\n");
    return 0;
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
    root->type = true; // this is a direction, and it is allowed to create file under it.
}
