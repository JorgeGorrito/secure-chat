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

void spreadMessage(struct Message message)
{
    struct NodoClient* aux = clients.head;

    while(aux)
    {
        printf("se envio mensaje a %d\n", aux->client.sock);
        send(aux->client.sock,(char*)&message, sizeof(struct Message), 0);
        aux = aux->next;
    }
}

void* service(void* args)
{
    /*Se inicializa con los datos del cliente*/
    struct Client client;
    client.thread_id = ((struct Client*)args)->thread_id;
    client.sock = ((struct Client*)args)->sock;
    strcpy(client.username, ((struct Client*)args)->username);

    while(TRUE)
    {
    }
}

int main(int argc, char *argv[])
{
    int listen_socket, service_socket;
    struct sockaddr_in adr;
    int lgadr = sizeof(adr);
    char buffer_username[USERNAME_SIZE];
    struct Client client;
    int port;
    pthread_t thread_id;
    struct Message message;

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
        vectShow(clients);

        lgadr = sizeof(adr);
        service_socket = accept(listen_socket, &adr, &lgadr);

        if(read(service_socket, buffer_username, USERNAME_SIZE)==-1)
        {
            printf("Username hasn't been received\n");
        }
        
        if (vectExist(&clients, buffer_username))
        {
            printf("Username %s has already been registered\n", buffer_username);
            close(service_socket);
            continue;
        }

        strcpy(client.username, buffer_username);
        client.sock = service_socket;
        pthread_create(&thread_id, NULL, service, (void*)& client);
        client.thread_id = thread_id;
        vectInsert(&clients, client);
        
        strcpy(message.from, "Server");
        sprintf(message.message, "The user %s is online.", client.username);
        message.kind =  SPREAD_MESSAGE;

        spreadMessage(message);
    } 
    return 0;
}