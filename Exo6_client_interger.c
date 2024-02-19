#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

enum GameState {
    PERDU,
    MOINS,
    PLUS,
    GAGNE
};

char *server = "\033[34m[server]\033[m";
char *client = "\033[32m[client]\033[m";

#define SERVER_ADDR "127.0.0.1" 
#define SUP 65535

char *enumtostr(uint8_t e) {
    switch (e) {
        case PLUS : return "PLUS";
        case MOINS : return "MOINS";
        case PERDU : return "PERDU";
        case GAGNE : return "GAGNE";
        default : return "UNKNOWN";
    }
}

void play(int sock_client){
    //handeling the client requests
    
    int guess, end = SUP, start = 0;
    int loose = false;
    uint16_t fnumber;

    while(!loose) {
        guess = (start + end) / 2; 
        printf("%s the guessed number %d\n",client,guess);
        fnumber = htons(guess);

        if(send(sock_client, &fnumber, sizeof(fnumber), 0) <= 0){
            perror("Send failed");
            close(sock_client);
            exit(EXIT_FAILURE);
        }
        uint8_t rep[2];
        if (recv(sock_client, rep, sizeof(fnumber), 0) == -1) {
            perror("Recieve failed");
            close(sock_client);
            exit(EXIT_FAILURE);
        }
        printf("%s response of the server: %s %d\n",client, enumtostr(rep[1]), rep[0]);
        //comparison

        if(rep[1] == GAGNE){
            printf("Congratulation I win !\n");
           break;
        }else if(rep[1] == MOINS){
            end = guess;
    
        }else if(rep[1] == PLUS){
            start = guess + 1;
        }
        else if(rep[1] == PERDU){
            loose = true;
            printf("I lost !\n");
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

    printf("Connexion established with the server\n");
    play(fdsock);

    close(fdsock);
    return 0;
}