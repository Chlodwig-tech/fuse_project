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

    st->st_uid=getuid(); // owner of the file
    st->st_gid=getgid(); // owner group of the file
    st->st_atime=time(NULL); // last access time
    st->st_mtime=time(NULL); // last modification time

    //st_mode -> is file, directory?
    if(strcmp(path,"/.Trash")!=0 && strcmp(path,"/.Trash-1000")!=0 && strcmp(path,"/.xdg-volume-info")!=0 && strcmp(path,"/autorun.inf")!=0){
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

int unlink_function(const char *path){
    
    printf("[unlink] function called [%s]\n",path);
    
    File *file_to_rm=tree_get_file(root,path);

    if(file_to_rm==NULL){
        return 0;
    }

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