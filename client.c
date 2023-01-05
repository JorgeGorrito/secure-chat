//Library
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<pthread.h>

#include "message.h"
//Constant
#define SIZE 256
#define USERNAME_SIZE 32
#define TRUE 1

void send_echo(int sock)
{
    while(TRUE)
    {
        char sendline [BUFFER_SIZE] = {};
        fgets(sendline, BUFFER_SIZE, stdin);
        write(sock, sendline, strlen(sendline));
    }
}

void receive_echo(int sock)
{
    char message[BUFFER_SIZE];

    while(TRUE)
    {
        recv(sock, message, sizeof(struct Message), 0);
        printf("chat@%s> %s.\n", ((struct Message*)message)->from, ((struct Message*)message)->message);
    }
}

int main(int argc, char* argv[])
{
    int sock;
    char com[SIZE];
    struct sockaddr_in adr;
    struct hostent *hp, *gethostbyname();

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

    char username[USERNAME_SIZE];
    strcpy(username, argv[3]);
    write(sock, username, USERNAME_SIZE);

    printf("You have successfully connected\n");
    pthread_create(&pth_send, NULL, (void*)&send_echo, (void*)sock);
    receive_echo(sock);

    return 0;
}