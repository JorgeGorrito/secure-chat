/*
    AUTHOR:        Jorge Abella
    GITHUB:        JorgeGorrito
    ROL:           Student
    ESTABLISHMENT: Universidad de los llanos
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

#include "chat_config.h"

struct Client
{
    pthread_t thread_id;
    int sock;
    char username[USERNAME_SIZE];
};

struct NodoClient
{   
    struct Client client;
    struct NodoClient* next;
};

struct Clients
{
    int n_clients;
    struct NodoClient* head;
};

int vectEmpty(struct Clients clients);
int vectExist(struct Clients* clients, char username[USERNAME_SIZE]);
int vectFindSock(struct Clients* clients, char username[USERNAME_SIZE]);
int vectInsert(struct Clients* clients, struct Client client);
int vectDropClient(struct Clients* clients, int sock);
void vectShow(struct Clients clients);