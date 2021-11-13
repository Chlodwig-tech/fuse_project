#ifndef TREE_H
#define TREE_H

#include <string.h>
#include "directory.h"
#include "file.h"
#include "string_operations.h"

Directory *tree_init(){
    Directory *root=(Directory*)malloc(sizeof(Directory));
    root->parent=NULL;
    root->name="/";
    root->path="/";
    list_init(&root->directories);
    list_init(&root->files);
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
    for(list_element *helper=dir->directories.first;helper!=NULL;helper=helper->next){
        Directory *directory=(Directory*)helper->data;
        if(strcmp(directory->path,path)==0){
            return 1;
        }
        if(tree_is_dir(directory,path)==1){
            return 1;
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

#endif // TREE_H