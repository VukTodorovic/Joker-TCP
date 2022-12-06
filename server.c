/* 
    ********************************************************************
    Description:    TCP/IP server
    Platform:       Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>
#include<time.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015


void* serveClient(void*);
void genJoker(int*);


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
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Connecting with 2 clients
    while(trenutniThread < 3){
        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
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

        pthread_t t = (trenutniThread == 1) ? t1 : t2;
        pthread_create(&t, NULL, serveClient, (void*) client_sock);
        //pthread_join(t, NULL);  // doesn't work with join
        trenutniThread++;
    }

    // Server prints lottery numbers
    genJoker(jokerNums);

    // End
    puts("[*]End of main function #");
    getchar();
    getchar();
    close(socket_desc);
    puts("socket closed");

    return 0;
}


// Function that serves single client on separate thread
void* serveClient(void* client_sock){
    int clientSocket = (int) client_sock;
    char client_message[DEFAULT_BUFLEN];
    char message_to_send[DEFAULT_BUFLEN];
    int read_size;

    // Server sends "Start game" to the client
    strcpy(message_to_send, "Start game");
    if( send(clientSocket , message_to_send , strlen(message_to_send), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    // Server 

    
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

