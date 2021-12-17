#include<stdlib.h>
#include<stdio.h>
#include<openssl/rsa.h>
#include<string.h>

#define PRINT_KEYS

void generate_keys(int key_length,char *publicKey,char *privateKey){
    RSA *keypair=RSA_generate_key(key_length,3,NULL,NULL);

    BIO *pri=BIO_new(BIO_s_mem());
    BIO *pub=BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(pri,keypair,NULL,NULL,0,NULL,NULL);
    PEM_write_bio_RSAPublicKey(pub, keypair);

    //PEM_write_RSAPublicKey();

    int pri_len = BIO_pending(pri);
    int pub_len = BIO_pending(pub);

    char *pri_key = malloc(pri_len + 1);
    char *pub_key = malloc(pub_len + 1);

    BIO_read(pri, pri_key, pri_len);
    BIO_read(pub, pub_key, pub_len);

    pri_key[pri_len] = '\0';
    pub_key[pub_len] = '\0';

    #ifdef PRINT_KEYS
        printf("\n%s\n%s\n", pri_key, pub_key);
    #endif

    FILE *pubfile,*prifile;

    pubfile=fopen(publicKey,"w");
    fprintf(pubfile,"%s",pub_key);
    fclose(pubfile);
    prifile=fopen(privateKey,"w");
    fprintf(prifile,"%s",pri_key);
    fclose(prifile);

    printf("done.\n");

    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);
    free(pri_key);
    free(pub_key);
}

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

int main(){

    //generate_keys(2048,"public.txt","private.txt");
    encrypt("dddpolskagola","public.txt","dane.dat");

    decrypt("private.txt","dane.dat");
    

    printf("qwerty12345\n");

    return 0;
}