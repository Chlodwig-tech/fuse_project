#include <stdio.h>
#include <stdlib.h>

#include "fuse_functions.h"


int main(int argc,char *argv[]){

    root=tree_init();

    Directory *dir1=directory_init("d1");
    tree_append_dir(root,dir1);
    Directory *dir=directory_init("d2");
    tree_append_dir(root,dir);
    Directory *dir2=directory_init("d3");
    tree_append_dir(dir1,dir2);
    Directory *dir3=directory_init("d4");
    tree_append_dir(root,dir3);
    Directory *dir4=directory_init("d5");
    tree_append_dir(root,dir4);

    File *f1=file_init("f1.txt");
    char *content="xd\n";
    f1->content=(char*)malloc(strlen(content));
    strcpy(f1->content,content);
    tree_append_file(root,f1);

    return fuse_main(argc,argv,&operations,NULL);
}