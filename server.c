//Library
//Standard Library
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
//Own Library
#include "clientlib.h"
#include "message.h"

/*Global Variables*/
struct Clients clients = {0, NULL};
struct QueueMessage messages = {NULL, NULL};

/*Specific task functions*/

/*Server connection functions*/
int createSocket(int *port, int type)
{
    int sockfd;
    struct sockaddr_in adr;
    int length;

    if ((sockfd = socket(PF_INET, type, 0))==-1)
    {
        perror("Error: impossible create socket");
        exit(2);
    }

    bzero((char*)&adr, sizeof(adr));
    adr.sin_port = htons(*port);
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_family = PF_INET;

    if(bind(sockfd, (struct sockaddr*)&adr, sizeof(adr))==-1)
    {
        perror("Error: bind");
        exit(3);
    }

    length = sizeof(adr);

    if(getsockname(sockfd, &adr, &length))
    {
        perror("Error: get sock name");
        exit(4);
    }

    *port = ntohs(adr.sin_port);  

    return sockfd;
}

void spreadMessage(struct Message* message)
{
    struct NodoClient* aux = clients.head;

    while(aux)
    {
        send(aux->client.sock, message, sizeof(struct Message), 0);
        aux = aux->next;
    }
}

void sendMessage(struct Message message)
{
    int sock = vectFindSock(&clients, message.to);
    send(sock,(char*)&message, sizeof(struct Message), 0);
}

void* service(void* args)
{
    struct Client client = {};
    memcpy(&client, (struct Client*)args, sizeof(struct Client));

    printf("%d %s %d \n%s\n", client.sock, client.username, client.thread_id, ((struct Client*)args)->public_key);
    struct Message message = {0, "", "", ""};
    int bytes_received = 0;

    while(TRUE)
    {
        cleanMessage(&message);
        bytes_received = recv(client.sock, &message, sizeof(struct Message), 0);
        
        /*Checking error in received message*/
        if(bytes_received == 0)
        {
            cleanMessage(&message);
            strcpy(message.from, client.username);
            char buffer[BUFFER_SIZE];
            sprintf(buffer, "The user %s has been disconnected", client.username);
            setMessage(&message, SPREAD_MESSAGE, buffer, "Server", "");
            if(vectDropClient(&clients, client.sock) == client.sock)
            {
                printf("\nThe user %s was disconnected.\n", client.username);
                printf("user online:");
                vectShow(&clients);
            }
            spreadMessage(&message);
            break;
        }
        else if(bytes_received == -1)
        {
            continue;
        }

        strcpy(message.from, client.username);
        switch (message.kind)
        {
            case SPREAD_MESSAGE:
            {
                spreadMessage(&message);
                break;
            }
            case PRIVATE_MESSAGE:
            {
                sendMessage(message);
                break;
            }       
            default:
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    int listen_socket, service_socket;
    struct sockaddr_in adr;
    int lgadr = sizeof(adr);
    struct Client client;
    int port;
    pthread_t thread_id;
    struct Message message = {0, "", "", ""};
    char bufferMessage[sizeof(struct Message)];
    int bytes_io = 0;

    if (argc!=2)
    {
        printf("format: server <port>\n");
        exit(1);
    }

    port = atoi(argv[1]);

    if((listen_socket = createSocket(&port, SOCK_STREAM))==-1)
    {
        fprintf(stderr, "Error: Could not create sock");
        exit(2);
    } 

    listen(listen_socket, 1024);
    printf("The server is running on port %d.\n", port);

    while(TRUE)
    {   
        printf("\nWaiting for connections\n");
        printf("User online: ");
        vectShow(&clients);

        lgadr = sizeof(adr);
        service_socket = accept(listen_socket, &adr, &lgadr);

        cleanMessage(&message);
        bytes_io = recv(service_socket, bufferMessage, sizeof(struct Message), 0);
        unpackMessage(bufferMessage, &message);
        
        if( bytes_io == -1)
        {
            printf("Username hasn't been received\n");
            setMessage(&message, DISCONNECTION_MESSAGE, "Username hasn't been received", "\0", "\0");
            send(service_socket, (char*)&message, sizeof(struct Message), 0);
            close(service_socket);
        }

        if (vectExist(&clients, message.from))
        {
            char msg[BUFFER_SIZE];
            printf("Username %s has already been registered\n", message.from);
            sprintf(msg, "The username %s is already in use", message.from);
            setMessage(&message, DISCONNECTION_MESSAGE, msg, "Server", "\0");
            send(service_socket, (char*)&message, sizeof(struct Message), 0);
            close(service_socket);
            continue;
        }
        
        //setClient(&client, &thread_id, service_socket, message.from, message.message);
        printf("usario: %s\nmensaje: %s\ntipo: %d\n\n", message.from, message.message, message.kind );
        //pthread_create(&thread_id, NULL, service, (void*)&client);
        continue;
        vectInsert(&clients, &client);
        
        cleanMessage(&message);
        sprintf(message.message, "The user %s is online.\n", client.username);
        //setMessage(&message, SPREAD_MESSAGE, message.message, "Server", "");
        //spreadMessage(&message);
        
        //cleanMessage(&message);
        //setMessage(&message, CONNECTION_MESSAGE, "you have successfully connected", "Server", "" );
        //send(service_socket, &message, sizeof(struct Message), 0);
    } 
    return 0;
}