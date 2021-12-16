#ifndef RSA_H
#define RSA_H

#include<openssl/rsa.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

RSA* get_public_keypair(char *publicKey){

    FILE *f1;
    f1=fopen(publicKey,"r");
    fseek(f1,0,SEEK_END);
    long f1_size=ftell(f1);
    rewind(f1);
    char *pub_key=malloc(f1_size+1);
    fread(pub_key,1,f1_size,f1);
    fclose(f1);

    BIO *bio1=BIO_new_mem_buf((void*)pub_key,strlen(pub_key));

    return PEM_read_bio_RSAPublicKey(bio1,NULL,NULL,NULL);

}

RSA* get_private_keypair(char *privateKey){

    FILE *f1;
    f1=fopen(privateKey,"r");
    fseek(f1,0,SEEK_END);
    long f1_size=ftell(f1);
    rewind(f1);
    char *pri_key=malloc(f1_size+1);
    fread(pri_key,1,f1_size,f1);
    fclose(f1);

    BIO *bio1=BIO_new_mem_buf((void*)pri_key,strlen(pri_key));

    return PEM_read_bio_RSAPrivateKey(bio1,NULL,NULL,NULL);

}


void encrypt(char *message,char *publicKey,char *destination){
    RSA *keypair=get_public_keypair(publicKey);
    char *encrypted_message= malloc(RSA_size(keypair));
    int encrypt_len=RSA_public_encrypt(strlen(message)+1,(unsigned char*)message,(unsigned char*)encrypted_message,keypair,RSA_PKCS1_OAEP_PADDING);
    FILE *f=fopen(destination,"w");
    fwrite(encrypted_message,sizeof(*encrypted_message),RSA_size(keypair),f);
    fclose(f);
    free(encrypted_message);
    RSA_free(keypair);
}

char* decrypt(char *privateKey,char *source){
    FILE *f=fopen(source,"r");
    fseek(f,0,SEEK_END);
    int encrypt_len=ftell(f);
    rewind(f);
    char *encrypted_message=malloc(encrypt_len);
    fread(encrypted_message,1,encrypt_len,f);
    fclose(f);

    char *decrypted_message=malloc(encrypt_len);
    RSA *keypair=get_private_keypair(privateKey);
    RSA_private_decrypt(encrypt_len,(unsigned char*)encrypted_message,(unsigned char*)decrypted_message,keypair,RSA_PKCS1_OAEP_PADDING);

    return decrypted_message;
}


#endif // RSA_H