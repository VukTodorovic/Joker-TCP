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
        if(trenutniThread > 2)
        {   
            //break;
        }
        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }
        fflush(stdout);
        printf("Connection accepted with client %d\n", client_sock);

        pthread_t t = (trenutniThread == 1) ? t1 : t2;

        if(trenutniThread==2){ // Ako su se oba konektovala
            //generate joker nums
            genJoker(jokerNums);
            printf("Joker brojevi: ");
            printJoker(jokerNums);
        }
        
        pthread_create(&t, NULL, serveClient, (void*) client_sock);
        pthread_join(&t, NULL);
        trenutniThread++;
    }

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
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
}

void genJoker(int* niz){
    //test
    for(int i=1; i<5; i++) {
        niz[i] = i;
    }
}

void printJoker(int* niz) {
    for(int i=1; i<5; i++) {
        printf("%d", niz[i]);
    }
    printf("\n");
}

