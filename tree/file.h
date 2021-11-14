#ifndef FILE_H
#define FILE_H

#include "directory.h"

typedef struct FILE_H{
    char *name;
    char *path;
    char *content;
    Directory *parent;
    struct stat *st;
}File;

File* file_init(char *name){
    File *f=(File*)malloc(sizeof(File));
    f->name=(char*)malloc(strlen(name)+1);
    strcpy(f->name,name);
    f->content=NULL;
    f->st=(struct stat*)malloc(sizeof(struct stat));
    f->st->st_uid=getuid();
    f->st->st_gid=getgid();
    f->st->st_atime=time(NULL);
    f->st->st_mtime=time(NULL);
    f->st->st_mode=S_IFREG | 0644; // regular file;
    f->st->st_nlink=1;
    f->st->st_size=1024;
    return f;
}

#endif // FILE_H