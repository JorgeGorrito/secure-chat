#include "chat_config.h"
/*kind message*/
#define SPREAD_MESSAGE 0
#define PRIVATE_MESSAGE 1
#define CONNECTION_MESSAGE 2
#define DISCONNECTION_MESSAGE 3

struct Message
{
    int kind;
    char from[USERNAME_SIZE];
    char to[USERNAME_SIZE];
    char message[BUFFER_SIZE];
};

struct NodeMessage
{
    struct Message message;
    struct NodeMessage* next;
};

struct QueueMessage
{
    struct NodeMessage *head;
};

void insertMessage(struct QueueMessage* messages, struct Message message);
struct Message dropMessage(struct QueueMessage* messages);