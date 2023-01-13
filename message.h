/*
    AUTHOR:        Jorge Abella
    GITHUB:        JorgeGorrito
    ROL:           Student
    ESTABLISHMENT: Universidad de los llanos
*/
#include <stdlib.h>
#include <string.h>
#include "chat_config.h"

/*kind message*/
#define SPREAD_MESSAGE 1
#define PRIVATE_MESSAGE 2
#define CONNECTION_MESSAGE 3
#define DISCONNECTION_MESSAGE 4
#define ECHO_MESSAGE 5
#define PKEY_MESSAGE 6
#define ONLINE_MESSAGE 7

struct __attribute__((packed)) Message
{
    char from[USERNAME_SIZE];
    char to[USERNAME_SIZE];
    char message[BUFFER_SIZE];
    int kind;
};

struct NodeMessage
{
    struct Message message;
    struct NodeMessage* next;
};

struct QueueMessage
{
    struct NodeMessage *head;
    struct NodeMessage *tail;
};

void setMessage( struct Message* message, int kind, char msg[BUFFER_SIZE], char from[USERNAME_SIZE], char to[USERNAME_SIZE]);
void insertMessage(struct QueueMessage* messages, struct Message message);
struct Message dropMessage(struct QueueMessage* messages);
int isEmptyMessages(struct QueueMessage* messages);
void showMessages(struct QueueMessage* messages);
void cleanMessage(struct Message* message);
void packMessage(struct Message* message, char* buffer);
void unpackMessage(char* buffer, struct Message* message);