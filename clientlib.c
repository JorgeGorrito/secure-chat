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

int vectInsert(struct Clients* clients, struct Client* client)
{
    clients->n_clients++;
    if(clients->head)
    {
        struct NodoClient* aux = clients->head;
        while(aux->next)
            aux = aux->next;
        
        aux->next = (struct NodoClient*)malloc(sizeof(struct NodoClient)); 
        aux = aux->next;
        memcpy(&aux->client, client, sizeof(struct Client));
    }
    else
    {
        clients->head = (struct NodoClient*)malloc(sizeof(struct NodoClient)); 
        memcpy(&clients->head->client, client, sizeof(struct Client));
    }
}

void vectShow(struct Clients* clients)
{
    struct NodoClient* aux = clients->head;
    while(aux)
    {
        fprintf(stdout, "%s, ", aux->client.username);
        aux = aux->next;
    }
    fprintf(stdout, "\n");
}

int vectDropClient(struct Clients* clients, int sock)
{
    struct NodoClient* tempAnt = NULL;
    struct NodoClient* temp = clients->head;
    int sockMath = -1;

    while(temp)
    {
        if (temp->client.sock == sock)
        {
            if (!tempAnt)
                clients->head=clients->head->next;
            else
                tempAnt->next = temp->next;

            
            sockMath = temp->client.sock;
            free(temp);
            break;
        }
        tempAnt = temp;
        temp = temp->next;
    }

    return sockMath;
}

void setClient(struct Client* client, pthread_t* thread_id, int sock, char username[USERNAME_SIZE], char* public_key)
{
    client->sock = sock;
    client->thread_id = thread_id;
    strncpy(client->username, username, USERNAME_SIZE);
    strncpy(client->public_key, public_key, RSA_SIZE);
}

void cleanClient(struct Client* client)
{
    setClient(client, NULL, 0, "", "");
}