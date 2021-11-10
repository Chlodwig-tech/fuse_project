#include <stdio.h>
#include <stdlib.h>

#include "fuse_functions.h"


int main(int argc,char *argv[]){

    Directory *root=tree_init();

    printf("Starting fuse...\n");

    return fuse_main(argc,argv,&operations);
}