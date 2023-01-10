#include "message.h"

void setMessage( struct Message* message, int kind, char msg[BUFFER_SIZE], char from[USERNAME_SIZE], char to[USERNAME_SIZE])
{
    strncpy(message->message, msg, BUFFER_SIZE);
    strncpy(message->from, from, USERNAME_SIZE);
    strncpy(message->to, to, USERNAME_SIZE);
    message->kind = kind; 
}

void showMessages(struct QueueMessage* messages)
{
    struct NodeMessage* temp = messages->head;
    
    while(temp)
    {
        printf("%s\n", temp->message.message);
        temp = temp->next;
    }
}

void insertMessage(struct QueueMessage* messages, struct Message message)
{
    struct NodeMessage* temp = (struct NodeMessage*)malloc(sizeof(struct NodeMessage));
    if(!temp)
    {
        printf("Error al reservar memoria");
        return;
    }

    if (!messages->tail)
    {
        messages->tail = temp;
        messages->head = messages->tail;
        memcpy(&messages->tail->message, &message, sizeof(struct Message));
    }
    else
    {
        memcpy(&temp->message, &message, sizeof(struct Message));
        messages->tail->next = temp;
        messages->tail = temp;    
    }
}

struct Message dropMessage(struct QueueMessage* messages)
{
    struct Message message;
    if (messages->head)
    {
        if(messages->tail == messages->head)
        {
            message = messages->head->message;
            free(messages->head);
            messages->head = NULL;
            messages->tail = NULL;
        }
        else
        {   
            struct NodeMessage* temp = messages->head;
            messages->head = temp->next;
            message = temp->message; 
            free(temp);
        }
    }

    return message;
}

int isEmptyMessages(struct QueueMessage* messages)
{
    return messages->head? 0 : 1;
}

void cleanMessage(struct Message* message)
{  
    setMessage(message, 0, "", "", "");
}

void packMessage(struct Message* message, char* buffer)
{
    memcpy(buffer, message->from, USERNAME_SIZE);
    memcpy(buffer + USERNAME_SIZE, message->to, USERNAME_SIZE);
    memcpy(buffer + USERNAME_SIZE*2, message->message, BUFFER_SIZE);
    memcpy(buffer + USERNAME_SIZE*2 + BUFFER_SIZE, &message->kind, sizeof(int));
}

void unpackMessage(char* buffer, struct Message* message)
{
    memcpy(message->from, buffer, USERNAME_SIZE);
    memcpy(message->to, buffer + USERNAME_SIZE, USERNAME_SIZE);
    memcpy(message->message, buffer + USERNAME_SIZE*2, BUFFER_SIZE);
    memcpy(&message->kind, buffer + USERNAME_SIZE*2 + BUFFER_SIZE, sizeof(int));
}