#include <stdio.h>
#include <stdlib.h>

#include "fuse_functions.h"


int main(int argc,char *argv[]){

    root=tree_init();
    argv[2][strlen(argv[2])-1] = 0;
    tree_add_directory_from_argv(argv[2],root,"tree/keys/private.txt");

    File *f=tree_get_file(root,"/tree/main.c");
    return fuse_main(argc-2,argv+2,&operations,NULL);
}