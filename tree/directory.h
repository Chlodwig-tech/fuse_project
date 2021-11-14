#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "list/list.h"

typedef struct DIRECTORY_{
    char *name;
    char *path;
    struct DIRECTORY_ *parent;
    list directories;
    list files;
    struct stat *st;
}Directory;

Directory* directory_init(const char *name){
    Directory *dir=(Directory*)malloc(sizeof(Directory));
    dir->name=(char*)malloc(strlen(name)+1);
    strcpy(dir->name,name);
    list_init(&dir->directories);
    list_init(&dir->files);
    dir->st=(struct stat*)malloc(sizeof(struct stat));
    dir->st->st_uid=getuid();
    dir->st->st_gid=getgid();
    dir->st->st_atime=time(NULL);
    dir->st->st_mtime=time(NULL);
    dir->st->st_mode=S_IFDIR | 0755; // directory;
    dir->st->st_nlink=2;
    return dir;
}

#endif // DIRECTORY_H