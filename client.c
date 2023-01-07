//Library
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<pthread.h>

#include "message.h"
//Constant
#define USERNAME_SIZE 32
#define TRUE 1

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
        recv(sock, &message, sizeof(struct Message), 0);
        printf("chat@%s> %s", message.from, message.message);
        if (message.kind == DISCONNECTION_MESSAGE)
            exit(0);
    }
}

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in adr;
    struct hostent *hp, *gethostbyname();
    struct Message message = {0, "", "", ""};
    pthread_t pth_send, pth_receive;

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

    setMessage(&message, CONNECTION_MESSAGE, "\0", argv[3], "\0");
    send(sock, &message, sizeof(struct Message), 0);

    recv(sock, &message, sizeof(struct Message), 0);
    if (message.kind == DISCONNECTION_MESSAGE)
    {
        printf("%s.\n", message.message);
        exit(0);
    }

    printf("You have successfully connected\n");
    pthread_create(&pth_send, NULL, (void*)&send_echo, (void*)sock);
    receive_echo(sock);

    return 0;
}

struct NodoClient* vectDrop(struct Clients* clients, int sock)
{
    struct NodoClient* temp; 
}