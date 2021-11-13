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

    File *f1=file_init("f1.txt");
    f1->content="xd\n";
    tree_append_file(root,f1);

    return fuse_main(argc,argv,&operations,NULL);
}