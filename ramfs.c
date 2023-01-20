#include "ramfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct File
{
	char name[35];
    bool type; // directory - true, file - false;
    size_t size;
    void * content;
	struct File *sonFile, *nextFile;
} rFile;

rFile *root;

typedef struct Descritptor {
    int desIndex;
    int flag;
    off_t offSize;
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
                        printf("Error : '%s' is not a directory.\n",str);
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
            len++;

            if (len > 32) {
                printf("Error : the directory name is too long.\n");
                return -1;
            }
        }
        i++;
    }
    if (len != 0) {
        rFile * p = ptr->sonFile;
        while(p != NULL) {
            if (strcmp(p->name, str) == 0) {
                rDescriptor * newDes = (rDescriptor *) malloc(sizeof(rDescriptor));
                newDes->desIndex = des_cnt;
                newDes->flag = flags;
                newDes->tarFile = p;

                if ((flags & O_APPEND) && !p->type) {
                    newDes->offSize = (off_t)p->size - 1;
                }

                if ((flags & O_TRUNC) && (flags & O_RDWR) && !p->type) {
                    newDes->offSize = 0;
                    p->content = (void *)realloc(p->content, 1);
                    memcpy(p->content, "\0", 1);
                    p->size = 0;
                }

                if(desHead == NULL) {
                    desHead = newDes;
                    newDes->nextDes = NULL;
                } else {
                    newDes->nextDes = desHead;
                    desHead = newDes;
                }
                printf("Success.\n");
                printf("Open file \"%s\".\n", newDes->tarFile->name);
                return des_cnt;
            }
            p = p->nextFile;
        }

        if (flags & O_CREAT) {
            rFile *newFile = (rFile *) malloc(sizeof(rFile));
            for (int k = 0; k < len; k++) // strcpy
                newFile->name[k] = str[k];
            newFile->name[len] = '\0';
            newFile->nextFile = ptr->sonFile;
            newFile->type = false;
            newFile->sonFile = NULL;
            newFile->content = (void *)malloc(1);
            memcpy(newFile->content, "\0", 1);
            newFile->size = 0;
            ptr->sonFile = newFile;

            rDescriptor * newDes = (rDescriptor *) malloc(sizeof(rDescriptor));
            newDes->desIndex = des_cnt;
            newDes->flag = flags;
            newDes->tarFile = newFile;
            newDes->offSize = 0;
            if (desHead == NULL) {
                desHead = newDes;
                newDes->nextDes = NULL;
            } else {
                newDes->nextDes = desHead;
                desHead = newDes;
            }

            printf("Success.\n");
            printf("Create new file '%s'.\n", newFile->name);
            return des_cnt;
        }
        else {
            printf("Error : there is no such file and it is not allowed to create.\n");
            return -1;
        }
    }
}

int rclose(int fd) {
    printf("rclose(%d):\n", fd);
    if (desHead == NULL) {
        printf("Error : there is no file opened.\n");
        return -1;
    }
    rDescriptor * ptr = desHead, * preptr = NULL;
    while (ptr != NULL) {
        if (ptr->desIndex == fd) {
            if (ptr == desHead) {
                desHead = ptr->nextDes;
                free(ptr);
                printf("Success.\n");
                return 0;
            } else {
                preptr->nextDes = ptr->nextDes;
                free(ptr);
                printf("Success.\n");
                return 0;
            }
        }
        preptr = ptr;
        ptr = ptr->nextDes;
    }
    printf("Error : such file is not opened yet.\n");
    return -1;
}

ssize_t rwrite(int fd, const void *buf, size_t count) {
    char * src = (char *) buf;
    printf("rwrite(%d, \"%s\", %zu):\n", fd, src, count);
    if (desHead == NULL) {
        printf("Error : there is no file opened.\n");
        return -1;
    }
    rDescriptor * ptr = desHead;
    while (ptr != NULL) {
        static int ind = 0;
        ind++;
        if (ptr->desIndex == fd) {
            if (!((ptr->flag & 3) == O_WRONLY || (ptr->flag & 3) == O_RDWR)) {
                printf("Error : it is not allowed to write.\n");
                return -1;
            }
//            printf("target file : %s\n", ptr->tarFile->name);
            if (ptr->tarFile->type) {
                printf("Error : '%s' is a directory.\n", ptr->tarFile->name);
                return -1;
            }

            // expand content
            if (ptr->offSize + count >= ptr->tarFile->size) {
                void * tmpContent = (void *)malloc(ptr->offSize + count + 1);
                memset(tmpContent, '\0', ptr->offSize + count + 1);
                memcpy(tmpContent, ptr->tarFile->content, ptr->tarFile->size);
                ptr->tarFile->size = ptr->offSize + count;
                free(ptr->tarFile->content);
                ptr->tarFile->content = tmpContent;
            }
            size_t srcSize = sizeof(src);
            for (int i = 0; i < count; i++) {
                if (i >= srcSize)
                    *((char *)ptr->tarFile->content + ptr->offSize + i) = '\0';
                else
                    *((char *)ptr->tarFile->content + ptr->offSize + i) = src[i];
            }
            *((char *)ptr->tarFile->content + ptr->tarFile->size) = '\0';
            ptr->offSize += (off_t)count;
            printf("offSize = %ld, fileSize = %zu\n",ptr->offSize, ptr->tarFile->size);
            printf("Success.\n");
            return (ssize_t)count;
        }
        ptr = ptr->nextDes;
    }
    printf("Error : such file is not opened yet.\n");
    return -1;
}

ssize_t rread(int fd, void *buf, size_t count) {
    printf("rread(%d, buf, %zu):\n", fd, count);
    if (desHead == NULL) {
        printf("Error : there is no file opened.\n");
        return -1;
    }
    rDescriptor * ptr = desHead;
    while (ptr != NULL) {
        if (ptr->desIndex == fd) {
            if(ptr->tarFile->type) {
                printf("Error : '%s' is a directory\n", ptr->tarFile->name);
                return -1;
            }
            if((ptr->flag & 3) == O_WRONLY) {
                printf("Error : the file is not allowed to read.\n");
                return -1;
            }
            ssize_t cntSize = 0;
            for(int i = 0; i < count && ptr->offSize < ptr->tarFile->size && i < sizeof(buf); i++) {
                cntSize++;
                *((char *)buf + i) =  *((char *)ptr->tarFile->content + ptr->offSize);
                ptr->offSize++;
            }
            printf("Succeed and return %zd.\n", cntSize);
            printf("filename: \"%s\", offSize = %ld, fileSize = %zu, destSize = %lu\n",ptr->tarFile->name, ptr->offSize, ptr->tarFile->size, sizeof(buf));
            return cntSize;
        }
        ptr = ptr->nextDes;
    }
    printf("Error : such file is not opened yet.\n");
    return -1;
}

off_t rseek(int fd, off_t offset, int whence) {
    printf("rseek(%d, %ld, %d)\n", fd, offset, whence);
    if (desHead == NULL) {
        printf("Error : there is no file opened.\n");
        return -1;
    }
    rDescriptor * ptr = desHead, * preptr = NULL;
    while (ptr != NULL) {
        if (ptr->desIndex == fd) {
            switch (whence) {
                case SEEK_SET :
                    ptr->offSize = offset;
                    break;
                case SEEK_CUR :
                    ptr->offSize += offset;
                    break;
                case SEEK_END :
                    ptr->offSize = (off_t)ptr->tarFile->size + offset;
                    break;
                default:
                    break;
            }
            printf("Succeed and return %ld.\n", ptr->offSize);
            printf("offSize = %ld, fileSize = %zu\n",ptr->offSize, ptr->tarFile->size);
            return ptr->offSize;
        }
        preptr = ptr;
        ptr = ptr->nextDes;
    }
    printf("Error : such file is not opened yet.\n");
    return -1;
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
                            printf("Error : the directory has existed\n");
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
                        printf("Error : '%s' is not a directory.\n", str);
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
                printf("Error : the directory name is too long\n");
                return -1;
            }
        }
        i++;
    }
    if (len != 0) {
        rFile * p = ptr->sonFile;
        str[len] = '\0';
        while(p != NULL) {
            if (strcmp(p->name, str) == 0) {
                printf("Error : the directory has existed\n");
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
    printf("rrmdir(\"%s\"): \n", pathname);
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
                    rFile * p = ptr->sonFile, * prep = NULL;
                    while(p != NULL) {
                        if (strcmp(p->name, str) == 0) {
                            if (p->sonFile == NULL && p->type) {
                                if (ptr->sonFile == p) {
                                    ptr->sonFile = p->nextFile;
                                    free(p);
                                } else {
                                    prep->nextFile = p->nextFile;
                                    free(p);
                                }
                                printf("Success.\n");
                                return 0;
                            } else if(p->type) {
                                printf("Error : the directory is not clear.\n");
                                return -1;
                            } else {
                                printf("Error : '%s' is not a directory.\n", str);
                                return -1;
                            }
                        }
                        prep = p;
                        p = p->nextFile;
                    }
                    printf("Error : there is no such file.\n");
                    return -1;
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
                        printf("Error : '%s' is not a directory.\n", str);
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
                printf("Error : the directory name is too long\n");
                return -1;
            }
        }
        i++;
    }
    if (len != 0) {
        str[len] = '\0';
        rFile * p = ptr->sonFile, * prep = NULL;
        while(p != NULL) {
            if (strcmp(p->name, str) == 0) {
                if (p->sonFile == NULL && p->type) {
                    if (ptr->sonFile == p) {
                        ptr->sonFile = p->nextFile;
                        free(p);
                    } else {
                        prep->nextFile = p->nextFile;
                        free(p);
                    }
                    printf("Success.\n");
                    return 0;
                } else if(p->type) {
                    printf("Error : the directory is not clear.\n");
                    return -1;
                } else {
                    printf("Error : '%s' is not a directory.\n", str);
                    return -1;
                }
            }
            prep = p;
            p = p->nextFile;
        }
        printf("Error : there is no such file.\n");
        return -1;
    }
}

int runlink(const char *pathname) {
    printf("rrmdir(\"%s\"): \n", pathname);
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
                    printf("Error : a file should not end with '/'.\n");
                    return -1;
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
                        printf("Error : '%s' is not a directory.\n", str);
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
                printf("Error : the directory name is too long\n");
                return -1;
            }
        }
        i++;
    }
    if (len != 0) {
        str[len] = '\0';
        rFile * p = ptr->sonFile, * prep = NULL;
        while(p != NULL) {
            if (strcmp(p->name, str) == 0) {
                if (!p->type) {
                    printf("Success.\n");
                    return 0;
                } else {
                    printf("Error : this is a file.\n");
                    return -1;
                }
            }
            prep = p;
            p = p->nextFile;
        }
        printf("Error : there is no such file.\n");
        return -1;
    }
}

void init_ramfs() {
    root = (rFile *)malloc(sizeof(rFile));
    strcpy(root->name, "root");
    root->nextFile = NULL;
    root->sonFile = NULL;
    root->type = true; // this is a directory, and it is allowed to create file under it.
}
