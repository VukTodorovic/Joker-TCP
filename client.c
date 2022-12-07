/* 
    ********************************************************************
    Description:    TCP/IP client
    Platform:       Raspberry Pi 2 - Model B
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


void generateRandom(int*);
void printNums(int*);


int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[64] = "";
    char server_message[DEFAULT_BUFLEN];
    int read_size;
    int numbers[5];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("[!]Could not create socket");
        close(sock);
        return -1;
    }
    puts("[*]Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("[!]Connect failed. Error");
        close(sock);
        return 1;
    }

    puts("[*]Connected");

    printf("------------------------------------------------\n");

    // Start game
    read_size = recv(sock , server_message , DEFAULT_BUFLEN , 0);
    server_message[read_size] = '\0';
    printf("[*]%s\n", server_message);

    if(strcmp(server_message, "Start game") != 0){
        puts("[!]Game didn't start because of an error");
        close(sock);
        return 1;
    }

    // Play option
    int play_option = 0;
    printf("[*]Choose play option: \n");
    printf("1) Input joker numbers\n");
    printf("2) Generate random\n");
    scanf("%d", &play_option);

    switch (play_option)
    {
    case 1:
        printf("[*]Input 5 numbers separated by space:\n");
        scanf("%d %d %d %d %d", numbers, numbers+1, numbers+2, numbers+3, numbers+4);
        break;
    case 2:
        generateRandom(numbers);
        break;
    default:
        puts("[!]Wrong input");
        close(sock);
        return 1;
    }

    printf("[*]Choosen numbers to play: ");
    printNums(numbers);

    // Sending numbers to server
    for(int i=0; i<5; i++){
        message[i] = numbers[i];
    }
    message[5] = '\0';

    if( send(sock , message , strlen(message), 0) < 0)
    {
        puts("[!]Send failed");
        return 1;
    }


    // Server informs the client if he won the lottery
    read_size = recv(sock , server_message , DEFAULT_BUFLEN , 0);
    server_message[read_size] = '\0';
    printf("[*]%s\n", server_message);
    
    close(sock);
    printf("[*]Socket closed\n");

    return 0;
}

// Generates 5 random numbers 0-9
void generateRandom(int* niz){
    //test
    for(int i=0; i<5; i++){
        niz[i] = i+1;
    }
}

// Prints array of numbers
void printNums(int* niz){
    for(int i=0; i<5; i++){
        printf("[%d]", niz[i]);
    }
    printf("\n");
}


