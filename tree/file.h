#ifndef FILE_H
#define FILE_H

#include "directory.h"

typedef struct FILE_{
    char *name;
    char *path;
    char *content;
    Directory *parent;
    struct stat *st;
}File;

File* file_init(char *name){
    File *f=(File*)malloc(sizeof(File));
    f->name=(char*)malloc(strlen(name)+1);
    strcpy(f->name,name);
    f->content=NULL;
    f->st=(struct stat*)malloc(sizeof(struct stat));
    f->st->st_uid=getuid();
    f->st->st_gid=getgid();
    f->st->st_atime=time(NULL);
    f->st->st_mtime=time(NULL);
    f->st->st_mode=S_IFREG | 0644; // regular file;
    f->st->st_nlink=1;
    f->st->st_size=1024;
    return f;
}
/*
void encrypt_file(File *f,unsigned char *key,unsigned char *iv){
    char *plaintext=f->content;
    char *cipher_text=(char*)malloc(strlen(plaintext));
    unsigned char tag[16];
    unsigned char aad[] = "AAD";
    
    f->st->st_size=encrypt(plaintext,aad,key,iv,cipher_text,tag);
    f->content=cipher_text;
}

void decrypt_file(File *f,unsigned char *key,unsigned char *iv){
    char *cipher_text=f->content;
    char *plaintext=(char*)malloc(strlen(cipher_text));
    unsigned char tag[16];
    unsigned char aad[] = "AAD";

    f->st->st_size=decrypt(cipher_text,f->st->st_size,aad,tag,key,iv,plaintext);
    f->content=plaintext;
}
*/
#endif // FILE_H