#include "clientlib.h"

int vectEmpty(struct Clients clients)
{
    /*if the clients is empty return 1
      else return 0
    */
    return !clients.head ? 1 : 0;
}

int vectExist(struct Clients* clients, char username[USERNAME_SIZE])
{
    /*If the data exist in clients return 1
      else return 0*/
    struct NodoClient *aux = clients->head;

    while(aux)
    {
        if(strcmp(aux->client.username, username)==0)
            return 1;

        aux = aux->next;
    }
    return 0;
}

int vectFindSock(struct Clients* clients, char username[USERNAME_SIZE])
{
    struct NodoClient *aux = clients->head;

    while(aux)
    {
        if(strcmp(aux->client.username, username)==0)
            return aux->client.sock;

        aux = aux->next;
    }

    return -1;
}

int vectInsert(struct Clients* clients, struct Client client)
{
    clients->n_clients++;
    if(clients->head)
    {
        struct NodoClient* aux = clients->head;
        while(aux->next)
            aux = aux->next;
        
        aux->next = (struct NodoClient*)malloc(sizeof(struct NodoClient)); 
        aux = aux->next;
        aux->client = client;
    }
    else
    {
        clients->head = (struct NodoClient*)malloc(sizeof(struct NodoClient)); 
        clients->head->client = client;
    }
}

void vectShow(struct Clients clients)
{
    struct NodoClient* aux = clients.head;
    while(aux)
    {
        fprintf(stdout, "%s, ", aux->client.username);
        aux = aux->next;
    }
    fprintf(stdout, "\n");
}

int vectDrop(struct Clients* clients, char username[USERNAME_SIZE])
{
    struct NodoClient* aux = clients->head;
    struct NodoClient* ant = NULL;

    if (aux && !ant && (strcmp(aux->client.username, username)==0))
    {
        clients->head = clients->head->next;
        free(aux);
        return 0;
    }

    while(aux)
    {
        ant = aux;
        aux = aux->next;
        if (strcmp(aux->client.username, username)==0)
        {
            ant->next = aux->next;
            free(aux);

            return 0;
        }
    }
    return -1;
}
