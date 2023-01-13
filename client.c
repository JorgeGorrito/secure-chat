//Library
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<pthread.h>
//Library Third
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "error_conn.h"
#include "message.h"

/*Global variables*/
RSA *pair_keys = NULL;
RSA *rsa_extern = NULL;
BIO *bio = NULL;
char pub_key_ext[KEY_SIZE];
char my_username[USERNAME_SIZE];
Error error = {NOT_ERROR};
/*Specific task functions*/
void print_help()
{
    printf("Secure Chat Information:\n\n");
    printf("Secure Chat is an application for Linux terminal distributions that allows for group chat where you can send private, end-to-end encrypted messages.\n\n");
    printf("Format: ./client host port username or ./client --help or ./client -h\n\n");
    printf("Client Information:\n");
    printf("- You can connect with the username you prefer, as long as it is not already registered in the chat.\n");
    printf("- Maximum username length is 16 characters.\n\n");
    printf("Chat Information:\n");
    printf("- Messages you send will not be encrypted and will reach all users currently connected.\n");
    printf("- Maximum message length is 1024 characters.\n\n");
    printf("Private Messages:\n");
    printf("- You can send private messages only to users currently connected.\n");
    printf("- Each private message must be encrypted with RSA Asimetric Encryption.\n");
    printf("- Maximum private message length is 598 characters.\n\n");
    printf("Chat Commands:\n");
    printf("- /commands: Provides a list of available commands\n");
    printf("- /exit: Closes your session and disconnects from the chat.\n");
    printf("- /help: Provides information about the application and its usage.\n");
    printf("- /online: Provides a list of currently connected users.\n");
    printf("- /private <user> <message>: Sends an encrypted message only to the specified user.\n\n");
    printf("Author: Jorge A. Abella.\nNick: JorgeGorrito\nContact: j0rg3.4b3ll4@gmail.com or jaabella@unillanos.edu.co\nProject repository: https://github.com/JorgeGorrito/secure-chat\n");
}

void print_commands()
{
    printf("Available commands:\n"
       "/commands: Show list of available commands\n"
       "/exit: Close the session and disconnect from the chat\n"
       "/help: Show Secure chat information\n"
       "/online: Show list of online users\n"
       "/private <username> <message>: Send private end-to-end encrypted message to specified user\n"
       );
}   

void generate_RSA_keys(char* pub_key)
{
    BIGNUM *bn = BN_new();
    BUF_MEM *buf = NULL;

    pair_keys = RSA_new();
    bio = BIO_new(BIO_s_mem());

    BN_set_word(bn, RSA_F4);
    RSA_generate_key_ex(pair_keys, 5120, bn, NULL);
    
    PEM_write_bio_RSAPublicKey(bio, pair_keys);
    BIO_get_mem_ptr(bio, &buf);
    
    memcpy(pub_key, buf->data, buf->length);
    pub_key[buf->length] = '\0';
    
    BN_free(bn);
    BIO_free_all(bio);
    bio = NULL; 
}

/*Server connection functions*/
void sendMessage(int sock)
{
    struct Message message = {0, "", "", ""};
    char bufferMessage[sizeof(struct Message)] = {};
    char sendline[BUFFER_SIZE];
    while(TRUE)
    {
        cleanMessage(&message);
        fgets(sendline, BUFFER_SIZE, stdin);
        sendline[strcspn(sendline, "\n")] = '\0';

        if (sendline[0] == '/')
        {
            int command_size = 0;
            if(strstr(sendline, " "))
                command_size = (int)(strstr((sendline+1), " ") - (sendline+1));
            else
                command_size = (int)strlen(sendline+1);

            char command[command_size + 1];
            strncpy(command, (sendline+1), command_size);
            command[command_size] = '\0';

            if (!strcmp(command, "private"))
            {

                int username_size = (int)(strstr(sendline+9, " ") - (sendline+9));
                int message_size = strlen(sendline)-9-username_size;
                char username[username_size+1];
                char message_str[message_size+1];

                memcpy(username, (sendline+9), username_size);
                username[username_size] = '\0';
                memcpy(message_str, (sendline+9)+username_size+1, message_size);
                message_str[message_size] = '\0';

                if(message_size>598)
                {
                    printf("Client> Exceeded maximum number of characters allowed for private messages (598).\n");
                    continue;
                }

                setMessage(&message, PKEY_MESSAGE, "", "", username);
                packMessage(&message, bufferMessage);
                send(sock, bufferMessage, sizeof(struct Message), 0);

                while(!rsa_extern && error.type!=ERROR_PKEY);

                if(error.type==ERROR_PKEY)
                {
                    printf("private@Server> The user %s isn't online.\n", username);
                    error.type==NOT_ERROR;
                    continue;
                }

                setMessage(&message, PRIVATE_MESSAGE, "", "", username);
                if (RSA_public_encrypt(message_size, message_str, message.message, rsa_extern, RSA_PKCS1_OAEP_PADDING) == -1)
                {
                    printf("Error encrypting the message\n");
                    exit(1);   
                }
                
                RSA_free(rsa_extern);
                rsa_extern = NULL;
            }
            else if(!strcmp(command, "online"))
            {
                setMessage(&message, ONLINE_MESSAGE, "", "", "");
            }
            else if(!strcmp(command, "commands"))
            {
                printf("Client> ");
                print_commands();
            }
            else if(!strcmp(command, "help"))
            {
                printf("Client> ");
                print_help();
            }
            else if(!strcmp(command, "exit"))
            {
                exit(0);
            }
            else
            {
                printf("Client> Unknown command: please check the syntax and try again.\n");
            }
        }
        else
        {
            setMessage(&message, SPREAD_MESSAGE, sendline, "", ""); 
        }
        packMessage(&message, bufferMessage);
        send(sock, bufferMessage, sizeof(struct Message), 0);
    }
}

void receiveMessage(int sock)
{
    struct Message message = {0, "", "", ""};
    char bufferMessage[sizeof(struct Message)] = {};
    while(TRUE)
    {
        cleanMessage(&message);
        if (recv(sock, bufferMessage, sizeof(struct Message), 0) == 0)
        {
            printf("Client> The server is off.\n");
            exit(0);
        }
        
        unpackMessage(bufferMessage, &message);
        switch(message.kind)
        {
            case SPREAD_MESSAGE:
            {
                printf("chat@%s> %s\n", message.from, message.message);        
                break;
            }
            case PRIVATE_MESSAGE:
            {
                char message_str[BUFFER_SIZE] = {};
                if(RSA_private_decrypt(RSA_size(pair_keys), message.message, message_str, pair_keys, RSA_PKCS1_OAEP_PADDING) == 0)
                {
                    printf("Algo salió mal al descifrar\n");
                    exit(0);   
                }
                printf("private@%s> %s\n", message.from, message_str);
                break;
            }
            case PKEY_MESSAGE:
            {
                if(!strlen(message.message))
                {
                    error.type = ERROR_PKEY;
                    break;
                }
                bio = BIO_new_mem_buf(message.message, -1);
                rsa_extern = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
                break;
            }
            case ONLINE_MESSAGE:
            {
                printf("private@%s> users online: %s\n", message.from, message.message);
                break;
            }
            case DISCONNECTION_MESSAGE:
            {
                printf("chat@%s> %s\n", message.from, message.message);
                exit(0);
                break;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in adr;
    struct hostent *hp, *gethostbyname();
    struct Message message = {0, "","", ""};
    char bufferMessage[sizeof(struct Message)];
    pthread_t pth_send, pth_receive;
    int bytes_io = 0;

     if (argc==2)
    {
        if(!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
        {
            print_help();
            return 0;
        }
    }
   
    if(argc != 4)
    {
        fprintf(stderr, "Invalid command. Use %s <host> <port> <username> or %s --help for more information.\n", argv[0], argv[0]);
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

    strcpy(my_username, argv[3]);
    setMessage(&message, CONNECTION_MESSAGE, "", my_username, "");
    printf("Client> Generating RSA keys, please wait...\n");
    generate_RSA_keys(message.message);
    packMessage(&message, bufferMessage);
    bytes_io = send(sock, bufferMessage, sizeof(struct Message), 0);
    recv(sock, bufferMessage, sizeof(struct Message), 0);

    unpackMessage(bufferMessage, &message);
    if (message.kind == DISCONNECTION_MESSAGE)
    {
        printf("%s.\n", message.message);
        exit(0);
    }

    recv(sock, bufferMessage, sizeof(struct Message), 0);
    unpackMessage(bufferMessage, &message);
    printf("chat@%s> %s.\n", message.from, message.message);
    pthread_create(&pth_send, NULL, (void*)&sendMessage, (void*)sock);
    receiveMessage(sock);
    
    return 0;
}