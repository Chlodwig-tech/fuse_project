#ifndef FUSE_FUNCTIONS_H
#define FUSE_FUNCTIONS_H

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "tree/tree.h"

char* get_parent_directory(const char* path){
    if(strcmp(path,"/")==0){
        return "/";
    }
    char *parent=NULL;
    int parentLen;
    char* last=strrchr(path, '/');
    if (last!=NULL) {
        parentLen=strlen(path)-strlen(last);
        parent=(char*)malloc(parentLen);
        strncpy(parent,path,parentLen);
    }

    return parent;
}

char* get_name(const char *path){
    if(strcmp(path,"/")==0){
        return NULL;
    }
    char *name=NULL;
    char *last=strrchr(path,'/');
    if(last!=NULL){
        int l=strlen(last);
        name=(char*)malloc(l);
        strncpy(name,last,l);
    }

    return ++name;
}

Directory *root;

// (path)path to the file, (st)structure filled with the file's attributes
int getattr_function(const char *path,struct stat *st){

    printf("[getattr] function called [%s]\n",path);

    st->st_uid=getuid(); // owner of the file
    st->st_gid=getgid(); // owner group of the file
    st->st_atime=time(NULL); // last access time
    st->st_mtime=time(NULL); // last modification time

    //st_mode -> is file, directory?
    if(strcmp(path,"/")==0 || tree_is_dir(root,path)==1){
        st->st_mode=S_IFDIR | 0755; // directory
        st->st_nlink=2;
    }else if(tree_is_file(root,path)==1){
        st->st_mode=S_IFREG | 0644; // regular file
        st->st_nlink=1;
        st->st_size=1024;
    }else{
        return -ENOENT;
    }

    return 0;
}

// (path)path to the directory, (buffer)pointer to names of files/directories in directory
// (filler) function from fuse that allowed to fill the buffr with available files in path
int readdir_function(const char *path,void *buffer,fuse_fill_dir_t filler,off_t offset,struct fuse_file_info *fi){
    
    printf("[readdir] function called [%s]\n",path);

    filler(buffer,".",NULL,0); // curent directory
    filler(buffer,"..",NULL,0); // parent directory

    Directory *c_dir=tree_get_dir(root,path);

    for(list_element *helper=c_dir->directories.first;helper!=NULL;helper=helper->next){
        Directory *dir=(Directory*)helper->data;
        filler(buffer,dir->name,NULL,0);
    }

    return 0;
}

// (path)path to the new directory, (mode)specifies permission birs
static int do_mkdir( const char *path, mode_t mode ){
    
    printf("[mkdir] function called [%s]\n",path);

    Directory *c_dir=tree_get_dir(root,get_parent_directory(path)); 

    if(c_dir==NULL){
        printf("[mkdir] NULL c_dir\n");
        return 0;
    }

    tree_append_dir(c_dir,directory_init(get_name(++path)));
    
	return 0;
}

static struct fuse_operations operations={
    .getattr=getattr_function,
    .readdir =readdir_function,
    .mkdir=do_mkdir
};

#endif // FUSE_FUNCTIONS_H