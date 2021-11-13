#ifndef STRING_OPERATIONS_H
#define STRING_OPERATIONS_H

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
        parent[parentLen]='\0';
    }

    if(strcmp(parent,"")==0){
        return "/";
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

    if(strcmp(++name,"")==0){
        return "/";
    }

    return name;
}

#endif // STRING_OPERATIONS_H