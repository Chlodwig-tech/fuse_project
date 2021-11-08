#ifndef TREE_H
#define TREE_H

#include <string.h>
#include "directory.h"
#include "file.h"

Directory *tree_init(){
    Directory *root=(Directory*)malloc(sizeof(Directory));
    root->parent=NULL;
    root->name="/";
    root->path="/";
    list_init(&root->directories);
    list_init(&root->files);
    return root;
}

void tree_append_dir(Directory *parent,Directory *dir_to_append){
    dir_to_append->parent=parent;
    dir_to_append->path=(char*)malloc(2+strlen(parent->path)+strlen(dir_to_append->name));
    strcpy(dir_to_append->path,parent->path);
    strcat(dir_to_append->path,"/");
    strcat(dir_to_append->path,dir_to_append->name);
    list_append(&parent->directories,(void*)dir_to_append);
}

void tree_apend_file(Directory *parent,File *file_to_append){
    file_to_append->parent=parent;
    file_to_append->path=(char*)malloc(2+strlen(parent->path)+strlen(file_to_append->name));
    strcpy(file_to_append->path,parent->path);
    strcat(file_to_append->path,"/");
    strcat(file_to_append->path,file_to_append->name);
    list_append(&parent->files,(void*)file_to_append);
}

#endif // TREE_H