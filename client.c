/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           TCP/IP klijent
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[64] = "";
    char quit_message[10] = "quit";
    char server_message[DEFAULT_BUFLEN];
    int read_size;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    while(!(strncmp(message, quit_message, 6)==0)){
        // Slanje

        scanf("%s", message); // Citamo poruku sa konzole
        fflush(stdin);

        if( send(sock , message , strlen(message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        // Primanje

        read_size = recv(sock , server_message , DEFAULT_BUFLEN , 0)

        fflush(stdout);
        server_message[read_size] = '\0';
        printf("Server %d: %s\n", sock, server_message);
        
    }

    close(sock);

    return 0;
}

