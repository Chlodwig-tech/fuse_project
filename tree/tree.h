#ifndef TREE_H
#define TREE_H

#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "directory.h"
#include "file.h"
#include "string_operations.h"
#include "rsa.h"

char *public_key_path;
char *path_dest;

Directory *tree_init(char *public_key,char *pd){
    public_key_path=public_key;
    path_dest=pd;
    Directory *root=(Directory*)malloc(sizeof(Directory));
    root->parent=NULL;
    root->name=(char*)malloc(strlen("/")+1);
    root->name="/";
    root->path="/";
    list_init(&root->directories);
    list_init(&root->files);
    root->st=(struct stat*)malloc(sizeof(struct stat));
    root->st->st_uid=getuid();
    root->st->st_gid=getgid();
    root->st->st_atime=time(NULL);
    root->st->st_mtime=time(NULL);
    root->st->st_mode=S_IFDIR | 0755; // directory;
    root->st->st_nlink=2;
    return root;
}

void tree_append_dir(Directory *parent,Directory *dir_to_append){
    dir_to_append->parent=parent;
    dir_to_append->path=(char*)malloc(2+strlen(parent->path)+strlen(dir_to_append->name));
    strcpy(dir_to_append->path,parent->path);
    if(strcmp(parent->path,"/")!=0){
        strcat(dir_to_append->path,"/");
    }
    strcat(dir_to_append->path,dir_to_append->name);
    list_append(&parent->directories,(void*)dir_to_append);
}

void tree_append_file(Directory *parent,File *file_to_append){
    file_to_append->parent=parent;
    file_to_append->path=(char*)malloc(2+strlen(parent->path)+strlen(file_to_append->name));
    strcpy(file_to_append->path,parent->path);
    if(strcmp(parent->path,"/")!=0){
        strcat(file_to_append->path,"/"); 
    }
    strcat(file_to_append->path,file_to_append->name);
    list_append(&parent->files,(void*)file_to_append);
}

int tree_is_dir(Directory *dir,const char *path){
    if(strncmp(dir->path,path,strlen(dir->path))==0){
        for(list_element *helper=dir->directories.first;helper!=NULL;helper=helper->next){
            Directory *directory=(Directory*)helper->data;
            if(strcmp(directory->path,path)==0){
                return 1;
            }
            if(tree_is_dir(directory,path)==1){
                return 1;
            }
        }
    }
    return 0;
}

int tree_is_file(Directory *dir,const char *path){
    for(list_element *helper=dir->files.first;helper!=NULL;helper=helper->next){
        File *file=(File*)helper->data;
        if(strcmp(file->path,path)==0){
            return 1;
        }
    }
    for(list_element *helper=dir->directories.first;helper!=NULL;helper=helper->next){
        Directory *directory=(Directory*)helper->data;
        if(tree_is_file(directory,path)==1){
            return 1;
        }        
    }
    return 0;
}

Directory* tree_get_dir(Directory *dir,const char *path){
    if(strcmp(dir->path,path)==0){
        return dir;
    }
    for(list_element *helper=dir->directories.first;helper!=NULL;helper=helper->next){
        Directory *d=(Directory*)helper->data;
        Directory *directory=tree_get_dir(d,path);
        if(directory!=NULL){
            return directory;
        }
    }
    return NULL;
}

File* tree_get_file(Directory *dir,const char *path){
    for(list_element *helper=dir->files.first;helper!=NULL;helper=helper->next){
        File *f=(File*)helper->data;
        if(strcmp(f->path,path)==0){
            return f;
        }
    }
    for(list_element *helper=dir->directories.first;helper!=NULL;helper=helper->next){
        Directory *d=(Directory*)helper->data;
        File *f=tree_get_file(d,path);
        if(f!=NULL){
            return f;
        }
    }
    return NULL;
}

void tree_remove_file_rec(File *f){
    printf("removing file [%s]\n",f->path);
    free(f->path);
    free(f->name);
    free(f->content);
    free(f);
}

void tree_remove_file(File *f){
    list_remove(&f->parent->files,(void*)f);
    tree_remove_file_rec(f);
}

void tree_remove_directory_recursive(Directory *dir){

    while(dir->directories.size!=0){
        Directory *d=(Directory*)dir->directories.last->data;
        list_pop(&dir->directories);
        tree_remove_directory_recursive(d);
    }

    while(dir->files.size!=0){
        File *f=(File*)dir->files.last->data;
        list_pop(&dir->files);
        tree_remove_file_rec(f);
    }

    printf("removing directory [%s]\n",dir->path);

    if(strcmp(dir->name,"/")!=0){
        free(dir->name);
        free(dir->path);
    }
    free(dir);
}

void tree_remove_directory(Directory *dir){
    list_remove(&dir->parent->directories,(void*)dir);
    tree_remove_directory_recursive(dir);
}


void file_print(File *f){
    printf("{F}: name: %s, path: %s\n",f->name,f->path);
}
void directory_print(Directory *dir,int depth){
    for(int i=1;i<depth;i++){
        printf("\t");
    }

    printf("{D}: name: %s, path: %s\n",dir->name,dir->path);

    for(list_element *file=dir->files.first;file!=NULL;file=file->next){
        for(int i=0;i<depth;i++){
            printf("\t");
        }
        File *f=(File*)file->data;
        file_print(f);
    }

    for(list_element *directory=dir->directories.first;directory!=NULL;directory=directory->next){
        Directory *d=(Directory*)directory->data;
        directory_print(d,depth+1);
    }
}
void tree_directory_print(Directory *dir){
    directory_print(dir,1);
}

void tree_add_directory_from_argv(const char *path,Directory *parent,char *privateKey){
    

    struct dirent *dp;
    DIR *dir=opendir(path);

    if(!dir){
        return;
    }   

    while((dp=readdir(dir))!=NULL){
        
        if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0){
            char *child_path=(char*)malloc(sizeof(path)+1+(sizeof(dp->d_name)));
            strcpy(child_path,path);
            strcat(child_path,"/");
            strcat(child_path,dp->d_name);
            if(dp->d_type==4){
                Directory *directory=directory_init(dp->d_name);
                tree_append_dir(parent,directory);
                tree_add_directory_from_argv(child_path,directory,privateKey);
            }else if(dp->d_type==8){
                File *file=file_init(dp->d_name);
                char *path_to_file=malloc(strlen(path)+strlen(dp->d_name)+1);
                strcpy(path_to_file,path);
                strcat(path_to_file,"/");
                strcat(path_to_file,dp->d_name);

                char *string=decrypt(privateKey,path_to_file);
                file->content=string;
                file->st->st_size=strlen(string);
                tree_append_file(parent,file);
                
                //char *string=decrypt("keys/private.txt",)
                /*FILE *f;
                f=fopen(child_path,"r");

                fseek(f, 0, SEEK_END);
                long fsize = ftell(f);
                printf("%s %ld:\n",child_path,fsize);
                fseek(f, 0, SEEK_SET);

                char *string = malloc(fsize + 1);
                fread(string, 1, fsize, f);
                fclose(f);
                */

                //string[fsize] = 0;
                //file->content=string;
                //file->st->st_size=fsize;
                
            }
        }
            
    }
    closedir(dir);
}

void save(char *path,Directory *dir){
    struct stat st={0};
    mkdir(path,0700);
    for(list_element *helper=dir->files.first;helper!=NULL;helper=helper->next){
        File *f=(File*)helper->data;
        char *destination=malloc(strlen(path)+strlen(f->path));
        strcpy(destination,path);
        strcat(destination,"/");
        strcat(destination,f->name);
        encrypt(f->content,public_key_path,destination);
    }
    for(list_element *helper=dir->directories.first;helper!=NULL;helper=helper->next){
        Directory *d=(Directory*)helper->data;
        char *destination=malloc(strlen(path)+strlen(d->path));
        strncpy(destination,path,strlen(path)-1);
        strcat(destination,d->path);
        save(destination,d);
    }
}

#endif // TREE_H