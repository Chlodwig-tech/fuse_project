#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "list/list.h"

typedef struct DIRECTORY_{
    char *name;
    char *path;
    struct DIRECTORY_ *parent;
    list directories;
    list files;
}Directory;

Directory* directory_init(const char *name){
    Directory *dir=(Directory*)malloc(sizeof(Directory));
    dir->name=(char*)malloc(strlen(name)+1);
    strcpy(dir->name,name);
    list_init(&dir->directories);
    list_init(&dir->files);
    return dir;
}

#endif // DIRECTORY_H