#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

//client with nc 


char *server = "\033[34m[server]\033[m";
char *client = "\033[32m[client]\033[m";
#define BUF_SIZE 20
#define SUP 65535
#define N_ATTEMPTS 20

void play(int sock_client, int server_number){
    //handeling th eclient requests
    if(sock_client < 0 ) return ;
    int attemps = N_ATTEMPTS;
    char buf[BUF_SIZE];
    while(attemps > 0 ) {

        int r = recv(sock_client, buf,sizeof(buf), 0);
        if(r <= 0) break;

        printf("%s %s",client, buf);
        int number = atoi(buf);
        memset(buf,'\0', sizeof(buf));
        //comparison
        attemps -= 1;
        if(attemps == 0){
            send(sock_client, "PERDU\n",6, 0);
            break;
        }
        if(number < server_number){
            sprintf(buf,"PLUS %d\n",attemps);
        }else if(number > server_number){
            sprintf(buf,"MOINS %d\n",attemps);
        }else{
            sprintf(buf,"GAGNE\n");
        }
        send(sock_client, buf, strlen(buf), 0);
        if(number == server_number) break;
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
    //put the loop
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
        // printf("try to guess a nmber between 0 and %d\n", SUP);

        // send(sock_client, "try to guess a number between 0 and 65535\n", strlen("try to guess a number between 0 and 65535\n"), 0);

        //choose randome number 
        int server_number = rand() % (SUP + 1);
        printf("%s random number generated %d\n", server,server_number);
        play(sock_client, server_number);
    }

    //*** close the server socket ***
    close(fdsocket);
    return 0;
}
