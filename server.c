/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP/IP server
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015




void* serveClient(void*);
void genJoker(int*);
void printJoker(int*);


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    //char client_message[DEFAULT_BUFLEN];
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

    while(1){
        printf("# TRENUTNI THREAD: %d #\n", trenutniThread);
        if(trenutniThread == 3)
        {   
            //generate joker nums
            fflush(stdout);
            genJoker(jokerNums);
            printf("Joker brojevi: ");
            printJoker(jokerNums);
            break;
        }
        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }
        fflush(stdout);
        printf("Connection accepted with client %d\n", client_sock); // client socket
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

    puts("# MAIN DOSAO DO RETURNA #");
    getchar();
    getchar();

    return 0;
}



void* serveClient(void* client_sock){
    int clientSocket = (int) client_sock;
    char client_message[DEFAULT_BUFLEN];
    int read_size;

    //Receive a message from client
    while( (read_size = recv(clientSocket , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        fflush(stdout);
        client_message[read_size] = '\0';
        printf("Client %d: %s\n", clientSocket, client_message);
    }

    if(read_size == 0)
    {
        printf("Client %d disconnected\n", clientSocket);
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
}

void genJoker(int* niz){
    //test
    for(int i=0; i<5; i++) {
        niz[i] = i+1;
    }
}

void printJoker(int* niz) {
    for(int i=0; i<5; i++) {
        printf("%d", niz[i]);
    }
    printf("\n");
}

