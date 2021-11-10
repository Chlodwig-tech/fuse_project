#ifndef FUSE_FUNCTIONS_H
#define FUSE_FUNCTIONS_H

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "tree/tree.h"

Directory *root;
Directory *current_dir;

// (path)path to the file, (st)structure filled with the file's attributes
int getattr_function(const char *path,struct stat *st){

    printf("%s  %s\n",path,current_dir->path);

    st->st_uid=getuid(); // owner of the file
    st->st_gid=getgid(); // owner group of the file
    st->st_atime=time(NULL); // last access time
    st->st_mtime=time(NULL); // last modification time

    //st_mode -> is file, directory?
    if(strcmp(path,"/")==0 || tree_is_dir(current_dir,path)==1){
        st->st_mode=S_IFDIR | 0755; // directory
        st->st_nlink=2;
        //current_dir=tree_get_dir(current_dir,path);
    }else if(tree_is_file(current_dir,path)==1){
        st->st_mode=S_IFREG | 0644; // regular file
        st->st_nlink=1;
        st->st_size=1024;
    }else{
        return -ENOENT;
    }

    //printf("[getattr] function called\n");
    //printf("\tAttributes of %s requested\n",path);

    return 0;
}

// (path)path to the directory, (buffer)pointer to names of files/directories in directory
// (filler) function from fuse that allowed to fill the buffr with available files in path
int readdir_function(const char *path,void *buffer,fuse_fill_dir_t filler,off_t offset,struct fuse_file_info *fi){
    
    filler(buffer,".",NULL,0); // curent directory
    filler(buffer,"..",NULL,0); // parent directory

    if(strcmp(path,"/")==0){
        for(list_element *helper=current_dir->directories.first;helper!=NULL;helper=helper->next){
            Directory *dir=(Directory*)helper->data;
            filler(buffer,dir->name,NULL,0);
        }
    }

    //printf("[readdir] function called\n");

    return 0;
}

// (path)path to the new directory, (mode)specifies permission birs
static int do_mkdir( const char *path, mode_t mode ){
    
    path++;
    
    Directory *dir=directory_init(path);
    tree_append_dir(current_dir,dir);

	return 0;
}

static struct fuse_operations operations={
    .getattr=getattr_function,
    .readdir =readdir_function,
    .mkdir=do_mkdir
};

#endif // FUSE_FUNCTIONS_H