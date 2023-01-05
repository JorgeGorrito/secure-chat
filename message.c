#include "message.h"

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