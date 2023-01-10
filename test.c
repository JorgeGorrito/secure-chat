#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <string.h>

#define BUFFER_SIZE 1024


void hexdump(unsigned char *buf, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

int main()
{   
    BIO *bio = BIO_new(BIO_s_mem());
    RSA *rsa = RSA_new();
    BIGNUM *bn = BN_new();
    BUF_MEM *buf = NULL;

    BN_set_word(bn, RSA_F4);
    RSA_generate_key_ex(rsa, 1024, bn, NULL);

    PEM_write_bio_RSAPublicKey(bio, rsa);

    BIO_get_mem_ptr(bio, &buf);
    

    char pub_key[buf->length + 1];
    memcpy(pub_key, buf->data, buf->length);
    pub_key[buf->length] = '\0';

    printf("%d\n",buf->length);
    printf(pub_key);

    char mensaje[BUFFER_SIZE];
    unsigned char mensajeCifrado[RSA_size(rsa)];
    char mensajeDescifrado[RSA_size(rsa)];

    strcpy(mensaje, "Mi nombre es Jorge");
    printf("Mensaje: %s\n", mensaje);

    if (RSA_public_encrypt(strlen(mensaje)+1, mensaje, mensajeCifrado, rsa, RSA_PKCS1_OAEP_PADDING) == -1)
    {
        printf("Error al cifrar\n");
        exit(1);   
    }

    printf("Mensaje cifrado:\n");
    hexdump(mensajeCifrado, RSA_size(rsa));

    if(RSA_private_decrypt(RSA_size(rsa), mensajeCifrado, mensajeDescifrado, rsa, RSA_PKCS1_OAEP_PADDING) == 0)
        printf("Algo salió mal al descifrar\n");
    else
        printf("Mensaje descifrado: %s\n", mensajeDescifrado);


    printf("rsa size: %d\n", RSA_size(rsa));

    //Llave recibida
    BIO* bio_received =  BIO_new_mem_buf(pub_key, -1);
    RSA* rsa_received = PEM_read_bio_RSAPublicKey(bio_received, NULL, NULL, NULL);

     if (RSA_public_encrypt(strlen(mensaje)+1, mensaje, mensajeCifrado, rsa_received, RSA_PKCS1_OAEP_PADDING) == -1)
    {
        printf("Error al cifrar\n");
        exit(1);   
    }

    printf("Mensaje cifrado:\n");
    hexdump(mensajeCifrado, RSA_size(rsa));

    if(RSA_private_decrypt(RSA_size(rsa), mensajeCifrado, mensajeDescifrado, rsa, RSA_PKCS1_OAEP_PADDING) == 0)
        printf("Algo salió mal al descifrar\n");
    else
        printf("Mensaje descifrado: %s\n", mensajeDescifrado);    

    BIO_free(bio);
    BIO_free(bio_received);
    BN_free(bn);
    RSA_free(rsa);

    return 0;
}