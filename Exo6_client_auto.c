#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

//client with nc 


char *server = "\033[34m[server]\033[m";
char *client = "\033[32m[client]\033[m";

#define SERVER_ADDR "127.0.0.1" 
#define BUF_SIZE 20
#define SUP 65535


void play(int sock_client){
    //handeling th eclient requests
    
    int guess;
    char buf[BUF_SIZE];
    int end = SUP;
    int start = 0;
    int loose = false;

    while(!loose) {
        memset(buf,'\0',sizeof(buf));
        guess = (start + end) / 2; 
        printf("%s the guessed number %d\n",client,guess);

        //send the guess to the server
        sprintf(buf, "%d\n", guess);
        if(send(sock_client, buf, strlen(buf), 0) <= 0){
            perror("Send failed");
            close(sock_client);
            exit(EXIT_FAILURE);
        }
        //*** read the serveur response 
        memset(buf,0,sizeof(buf));
        int r; 
        if ((r = recv(sock_client, buf, sizeof(buf), 0)) == -1) {
            perror("Recieve failed");
            close(sock_client);
            exit(EXIT_FAILURE);
        }
        buf[r] = '\0';
        printf("%s response f the server: %s",client, buf);
        buf[4] = '\0';

        //comparison
        if((strcmp(buf, "GAGN")) == 0){
            printf("Congratulation I win !\n");
           break;

        }else if((strcmp(buf, "MOIN")) == 0){
            end = guess;
    
        }else if((strcmp(buf, "PLUS")) == 0){
            start = guess + 1;
        }
        else if((strcmp(buf, "PERD")) == 0){
            loose = true;
            printf("\nVous avez perdu !\n");
        }
    }   
}


int main(int argc, char *argv[]){
   //the port number  that we want to connect to
    if(argc < 2){
        printf("Usage: %s server_port\n", argv[0]);
        exit(1);
    }

    int fdsock;
    if((fdsock= socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("creation socket");
        exit(1);
    }
    //*** creation de l'adresse du serveur ***
    struct sockaddr_in address_sock;
    memset(&address_sock, 0,sizeof(address_sock));
    address_sock.sin_family = AF_INET;
    address_sock.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, SERVER_ADDR, &address_sock.sin_addr);

    //*** demande de connexion au serveur ***
    if(connect(fdsock, (struct sockaddr *)&address_sock, sizeof(address_sock)) == -1){
        perror("echec de la connexion");
        close(fdsock);
        exit(2);
    }

    printf("Connexion établie avec le serveur\n");
    play(fdsock);
    close(fdsock);
    return 0;
}