#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

enum GameState {
    PERDU,
    MOINS,
    PLUS,
    GAGNE
};

char *server = "\033[34m[server]\033[m";
char *client = "\033[32m[client]\033[m";

#define BUF_SIZE 20
#define SUP 65535
#define N_ATTEMPTS 20

void play(int sock_client, int server_number){
    //handeling the client requests
    uint16_t recvnumber;
    int attemps = N_ATTEMPTS;
    while(attemps > 0 ) {

        int r = recv(sock_client, &recvnumber,sizeof(recvnumber), 0);
        if(r <= 0) break;

        // Treat the data
        recvnumber = ntohs(recvnumber);

        printf("%s the client sent %d\n",server, recvnumber);

        //comparison
        attemps -= 1;
        uint8_t rep[2];
        rep[0] = attemps; // Premier octet : number of attemps left
        if(attemps == 0){
            rep[1] = PERDU;
        }else if(recvnumber < server_number){
            rep[1] = PLUS;
        }else if(recvnumber > server_number){
            rep[1] = MOINS;
        }else{
           rep[1] = GAGNE;
        }
        send(sock_client, rep, sizeof(rep), 0);
        if(rep[1] == PERDU || rep[1] == GAGNE ) break;
    }

    close(sock_client);
    printf("Client disconnected.\n");
}

//to creat the client : nc -4 localhost 4242
//execute withe the same port

int main(int argc, char *argv[]){
   //the port number  that we want to connect to
    if(argc < 2){
        printf("Usage: %s server_port\n", argv[0]);
        exit(1);
    }

    //*** creat the socket
    int fdsocket;
    if((fdsocket = socket(PF_INET, SOCK_STREAM, 0)) == -1 ){
        perror("creation of the socket failed");
        exit(1);
    }
    //creat the server adress
    struct sockaddr_in serv_addr, cli_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1])) ; 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //*** bind the socket to the port ***
    if(bind(fdsocket, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1){
        close(fdsocket);
        perror("Binding failed");
        exit(1);
    }


    //*** the server is ready to listen to the cnx on the port ***
    if(listen(fdsocket,0) == -1){
        close(fdsocket);
        perror("Listen failed");
        exit(1);
    }

    printf("Server is waiting for a connexions...\n");
    srand(time(NULL));

    while(1){
        //*** creat the socket communication for the client ***
        socklen_t cli_len = sizeof(cli_addr);
        int sock_client= accept(fdsocket, (struct sockaddr *) &cli_addr, &cli_len);

        if(sock_client==-1){
            close(fdsocket);
            perror("Accept failed");
            exit(1);
        }
        printf("Client connected\n");
        printf("try to guess a nmber between 0 and %d\n", SUP);

        //choose randome number 
        int server_number = rand() % (SUP + 1);
        printf("%s random number generated %d\n", server,server_number);
        play(sock_client, server_number);
    }

    //*** close the server socket ***
    close(fdsocket);
    return 0;
}