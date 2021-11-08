#ifndef TREE_H
#define TREE_H

#include "directory.h"

Directory *tree_init(){
    Directory *root=(Directory*)malloc(sizeof(Directory));
    root->parent=NULL;
    root->name="/";
    root->path="/";
    list_init(&root->directories);
    list_init(&root->files);
    return root;
}

#endif // TREE_H