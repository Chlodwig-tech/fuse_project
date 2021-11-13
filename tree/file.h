#ifndef FILE_H
#define FILE_H

#include "directory.h"

typedef struct FILE_H{
    char *name;
    char *path;
    char *content;
    Directory *parent;
}File;

File* file_init(char *name){
    File *f=(File*)malloc(sizeof(File));
    f->name=(char*)malloc(strlen(name)+1);
    strcpy(f->name,name);
    f->content=NULL;
    return f;
}

#endif // FILE_H