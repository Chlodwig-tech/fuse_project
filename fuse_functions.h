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

// (path)path to the file, (st)structure filled with the file's attributes
int getattr_function(const char *path,struct stat *st){

    printf("[getattr] function called [%s]\n",path);

    //st_mode -> is file, directory?
    if(strcmp(path,"/.Trash")!=0 && strcmp(path,"/.Trash-1000")!=0 && strcmp(path,"/.xdg-volume-info")!=0 && strcmp(path,"/autorun.inf")!=0){
        if(strcmp(path,"/")==0 || tree_is_dir(root,path)==1){
            Directory *dir=tree_get_dir(root,path);
            st->st_uid=dir->st->st_uid; // owner of the file
            st->st_gid=dir->st->st_gid; // owner group of the file
            st->st_atime=dir->st->st_atime; // last access time
            st->st_mtime=dir->st->st_mtime; // last modification time
            st->st_mode=dir->st->st_mode;
            st->st_nlink=dir->st->st_nlink;
        }else if(tree_is_file(root,path)==1){
            File *f=tree_get_file(root,path);
            st->st_uid=f->st->st_uid; // owner of the file
            st->st_gid=f->st->st_gid; // owner group of the file
            st->st_atime=f->st->st_atime; // last access time
            st->st_mtime=f->st->st_mtime; // last modification time
            st->st_mode=f->st->st_mode;
            st->st_nlink=f->st->st_nlink;
            st->st_size=f->st->st_size;
        }else{
            return -ENOENT;
        }
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

    for(list_element *helper=c_dir->files.first;helper!=NULL;helper=helper->next){
        File *file=(File*)helper->data;
        filler(buffer,file->name,NULL,0);
    }

    return 0;
}

// (path)path of the file, (buffer)pointer to file's content portion, (size)size of file's content portion
// (offset)place in the file's content where is starting reading from, (fi) file info
int read_function(const char *path,char *buffer,size_t size,off_t offset,struct fuse_file_info *fi){
    
    printf("[read] function called [%s]\n",path);
    
    File *file=tree_get_file(root,path);
    
    if(file==NULL){
        return -1;
    }

    memcpy(buffer,file->content+offset,size);
    
    return strlen(file->content)-offset;
}

// (path)path to the new directory, (mode)specifies permission bits and type
int mkdir_function( const char *path, mode_t mode ){
    
    printf("[mkdir] function called [%s]\n",path);

    Directory *c_dir=tree_get_dir(root,get_parent_directory(path));
    
    if(c_dir==NULL){
        printf("[mkdir] NULL c_dir\n");
        return 0;
    }

    c_dir->st->st_mtime=time(NULL);

    if(c_dir==root){
        tree_append_dir(c_dir,directory_init(get_name(path)));    
    }else{
        tree_append_dir(c_dir,directory_init(get_name(++path)));
    }

	return 0;
}

// (path)path to the new file, (mode)specifies permission bits and type, (rdev)specifies if the new file is a device file
int mknod_function(const char *path,mode_t mode,dev_t rdev){
    
    printf("[mknod] function called [%s]\n",path);

    Directory *c_dir=tree_get_dir(root,get_parent_directory(path));

    if(c_dir==NULL){
        printf("[mknod] NULL c_dir\n");
        return 0;
    }

    c_dir->st->st_mtime=time(NULL);

    if(c_dir==root){
        tree_append_file(c_dir,file_init(get_name(path)));
    }else{
        tree_append_file(c_dir,file_init(get_name(++path)));
    }

    return 0;
}

// (path)path to the file, (buffer)pointer to content wanted to be written into file, (size)size of file's content portion,
// // (offset)place in the file's content where is starting writing from, (fi) file info
int write_function(const char *path,const char *buffer,size_t size,off_t offset,struct fuse_file_info *fi){

    printf("[write] function called [%s]\n",path);

    File *file=tree_get_file(root,path);

    if(file==NULL){
        return -1;
    }

    file->content=(char*)malloc(strlen(buffer));
    strcpy(file->content,buffer);

    file->st->st_size=size;
    file->st->st_mtime=time(NULL);

    file->parent->st->st_mtime=time(NULL);

    return size;
}

// (path)path to thr file
int rmdir_function(const char *path){

    printf("[rmdir] function called [%s]\n",path);

    Directory *dir_to_rm=tree_get_dir(root,path);

    if(dir_to_rm==NULL || dir_to_rm==root){
        return 0;
    }

    tree_remove_directory(dir_to_rm);

    return  0;
}

// (path)path to the file
int unlink_function(const char *path){
    
    printf("[unlink] function called [%s]\n",path);
    
    File *file_to_rm=tree_get_file(root,path);

    if(file_to_rm==NULL){
        return 0;
    }

    file_to_rm->parent->st->st_mtime=time(NULL);

    tree_remove_file(file_to_rm);
    
    return 0;
}

static struct fuse_operations operations={
    .getattr=getattr_function,            // getting file's attributes
    .readdir =readdir_function,           // reading directory
    .read=read_function,                  // reading file
    .mkdir=mkdir_function,                // making directory
    .mknod=mknod_function,                // making file (i.e. text editor)
    //.create                             // making file (i.e. touch)
    .write=write_function,                // writing file
    .rmdir=rmdir_function,                // removing directory
    .unlink=unlink_function               //
};

#endif // FUSE_FUNCTIONS_H