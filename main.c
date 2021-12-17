#include <stdio.h>
#include <stdlib.h>

#include "fuse_functions.h"


int main(int argc,char *argv[]){

    root=tree_init(argv[5],argv[7]);
    argv[9][strlen(argv[9])-1] = 0;
    tree_add_directory_from_argv(argv[9],root,argv[3]);

    return fuse_main(argc-9,argv+9,&operations,NULL);
}