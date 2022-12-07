/* 
    ********************************************************************
    Description:    TCP/IP server
    Platform:       Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>     //printf
#include<string.h>    //strlen
#include<sys/socket.h>//socket
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>   //threads
#include<time.h>      //for seed
#include<stdbool.h>   //boolean

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015


void* serveClient(void*);
void genJoker(int*);
bool checkResult(char*, int*);


typedef struct thread_arguments { // struct used to pass multiple arguments to the thread function
            int client_sock;
            int* jokerNums;
} thread_args;


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    pthread_t t1, t2;
    int trenutniThread = 1;
    int jokerNums[5];
   
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("[!]Could not create socket");
    }
    puts("[*]Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("[!]bind failed. Error");
        return 1;
    }
    puts("[*]Bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("[*]Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Connecting with 2 clients
    while(trenutniThread < 3){
        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("[!]Accept failed");
            return 1;
        }
        fflush(stdout);
        printf("[*]Connection accepted with client %d\n", client_sock); // client socket
        printf("Client port: %d\n", client.sin_port); // client port
        char ip_addr[20];
        inet_ntop(AF_INET, &(client.sin_addr.s_addr), ip_addr, c);
        /**
        *   const char *inet_ntop(int af, const void *restrict src, char *restrict dst, socklen_t size);
        *   AF_INET - it is an IPv4 adress
        *   client.sin_addr.s_addr - IPv4 adress in bytes (unsigned int32)
        *   ip_addr - destination / return value for "stringyfied" version of ip adress
        *   c - socklen (it should represent length of buffer);
        **/
        printf("Client addr: %s\n", ip_addr); // client ip address
        fflush(stdout);

        // Generating new thread to serve the client
        pthread_t t = (trenutniThread == 1) ? t1 : t2;
        thread_args t_args;
        t_args.client_sock = client_sock;
        t_args.jokerNums = jokerNums;
        pthread_create(&t, NULL, serveClient, (void*) &t_args);

        trenutniThread++;
    }

    // Server prints lottery numbers
    genJoker(jokerNums);

    // End
    getchar();
    // getchar();
    close(socket_desc);
    puts("[*]Socket closed\n");

    return 0;
}


// Function that serves single client on separate thread
void* serveClient(void* arguments){
    thread_args* args = arguments;
    int client_sock = args->client_sock;
    int* jokerNums = args->jokerNums;

    char client_message[DEFAULT_BUFLEN];
    char message_to_send[DEFAULT_BUFLEN];
    int read_size;

    // Server sends "Start game" to the client
    strcpy(message_to_send, "Start game");
    if( send(client_sock , message_to_send , strlen(message_to_send), 0) < 0)
    {
        puts("[!]Send failed");
        return 1;
    }

    // Server gets choosen numbers from client and checks if it's a winning combination
    read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0);
    client_message[read_size] = '\0';

    printf("[*]Client %d combination: ", client_sock);
    for(int i=0; i<5; i++){
        printf("[%d]", client_message[i]);
    }
    printf("\n");

    bool result = checkResult(client_message, jokerNums);
    if(result){
        printf("[*]Client %d won the game!\n", client_sock);
    }
    else{
        printf("[*]Client %d didn't win!\n", client_sock);
    }
    
    // Server sends result to the client
    if(result){
        strcpy(message_to_send, "You won the lottery!");
    }
    else {
        strcpy(message_to_send, "You didn't win!");
    }
    
    if( send(client_sock , message_to_send , strlen(message_to_send), 0) < 0)
    {
        puts("[!]Send failed");
        return 1;
    }
}

// Checks if the player has a winning combination
bool checkResult(char* clientCombination, int* jokerNums){
    bool result = true;

    for(int i=0; i<5; i++){
        bool numberFound = false;

        for(int j=0; j<5; j++){
            if(clientCombination[i] == jokerNums[j]){
                numberFound = true;
            }
        }
        if(numberFound == false){
            result = false;
        }
    }

    return result;
}

// Generater Loto and Joker numbers and prints them to console
void genJoker(int* niz){
    int loto[7];
    srand(time(NULL));

    // Loto
    printf("[*]Loto numbers: ");
    for(int i=0; i<7; i++){
        loto[i] = rand()%40;
        printf("[%d]", loto[i]);
    }
    printf("\n");

    // Joker
    printf("[*]Joker numbers: ");
    for(int i=0; i<5; i++) {
        if(loto[i]<10) {
            niz[i] = loto[i];
        }
        else {
            niz[i] = loto[i]%10;
        }
        printf("[%d]", niz[i]);
    }
    printf("\n");
}










//Receive a message from client
    // while( (read_size = recv(clientSocket , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    // {
    //     fflush(stdout);
    //     client_message[read_size] = '\0';
    //     printf("Client %d: %s\n", clientSocket, client_message);
    // }

    // if(read_size == 0)
    // {
    //     printf("Client %d disconnected\n", clientSocket);
    //     fflush(stdout);
    // }
    // else if(read_size == -1)
    // {
    //     perror("recv failed");
    // }

