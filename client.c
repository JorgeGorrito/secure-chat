//Library
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<pthread.h>
//Library Third
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "message.h"

/*Global variables*/
RSA *pair_keys = NULL;
BIO *bio = NULL;

void send_echo(int sock)
{
    struct Message message = {0, "", "", ""};
    while(TRUE)
    {
        cleanMessage(&message);
        char sendline [BUFFER_SIZE] = {};
        fgets(sendline, BUFFER_SIZE, stdin);
        
        setMessage(&message, SPREAD_MESSAGE, sendline, "", ""); 
        send(sock, &message, sizeof(struct Message), 0);
    }
}

void receive_echo(int sock)
{
    struct Message message = {0, "", "", ""};
    while(TRUE)
    {
        cleanMessage(&message);
        if (recv(sock, &message, sizeof(struct Message), 0) == 0)
        {
            printf("Client> The server is off.\n");
            exit(0);
        }

        printf("chat@%s> %s", message.from, message.message);
        if (message.kind == DISCONNECTION_MESSAGE)
            exit(0);
    }
}

void printChar(char* str, int len)
{
    for (int i=0; i<len; i++)
        printf("%c",str[i]);
    printf("\n");
}

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in adr;
    struct hostent *hp, *gethostbyname();
    struct Message message = {0, "","", ""};
    char bufferMessage[sizeof(struct Message)];
    pthread_t pth_send, pth_receive;
    int bytes_io = 0;

    BIGNUM *bn = BN_new();
    BUF_MEM *buf = NULL;

    pair_keys = RSA_new();
    bio = BIO_new(BIO_s_mem());

    BN_set_word(bn, RSA_F4);
    
    RSA_generate_key_ex(pair_keys, 1024, bn, NULL);
    PEM_write_bio_RSAPublicKey(bio, pair_keys);
    BIO_get_mem_ptr(bio, &buf);

    char pub_key[buf->length + 1];
    memcpy(pub_key, buf->data, buf->length);
    pub_key[buf->length] = '\0';

    if(argc != 4)
    {
        fprintf(stderr, "format: %s <HOST> <PORT> <USERNAME>\n", argv[0]);
        exit(1);
    }

    if((sock = socket(PF_INET, SOCK_STREAM, 0))==-1)
    {
        perror("Error: imposible create sock");
        exit(2);
    }

    if((hp=gethostbyname(argv[1]))==NULL)
    {
        perror("Error: the machine name unknown");
        exit(3);
    }

    adr.sin_family = PF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[2]));
    bcopy(hp->h_addr, &adr.sin_addr, hp->h_length);

    if(connect(sock, &adr, sizeof(adr))==-1)
    {
        perror("Error: connection failed");
        exit(4);
    }

    printf("tamanno de la key: %d\n", strlen(pub_key));
    setMessage(&message, CONNECTION_MESSAGE, pub_key, argv[3], "\0");
    packMessage(&message, bufferMessage);
    printf("username: %s.\nkey: %s\n", message.from, ((struct Message*)bufferMessage)->message);
    printf("size of Message: %d - %d\n", sizeof(struct Message),SOCK_STREAM);
    bytes_io = send(sock, bufferMessage, sizeof(struct Message), 0);
    printf("Se enviaron %d bytes\n", bytes_io);
    if (bytes_io==-1)
        printf("Error al enviar datos\n");

    recv(sock, &message, sizeof(struct Message), 0);
    if (message.kind == DISCONNECTION_MESSAGE)
    {
        printf("%s.\n", message.message);
        exit(0);
    }

    recv(sock, &message, sizeof(struct Message), 0);
    printf("chat@%s> %s.\n", message.from, message.message);
    pthread_create(&pth_send, NULL, (void*)&send_echo, (void*)sock);
    receive_echo(sock);

    return 0;
}