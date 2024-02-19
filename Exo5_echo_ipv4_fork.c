#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char *server = "\033[34m[pere]\033[m";
char *client = "\033[32m[fils]\033[m";
#define BUF_SIZE 20

void echo(int sock_client){
    if(sock_client < 0 ) return ;

    char buf[BUF_SIZE];
    memset(buf,0,sizeof(buf));
    
    int r;
    while((r = recv(sock_client,buf,sizeof(buf),0)) > 0){
        printf("%s message %.*s\n",client, r,buf);

        //*** send the received message back to the client ***
        if(send(sock_client,buf,r,0) == -1){
            perror("send failed");
            return;
        }
    }
    close(sock_client);
    if(r == -1){
        perror("reception failed");
        return;
    }else if(r == 0){
        perror("client closed connection");
        return;
    }
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
    struct sockaddr_in serv_addr;
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
    //put the loop
    struct  sockaddr_in cli_addr;
    while(1){
        //*** creat the socket communication for the client ***
        // memset(&cli_addr ,'0' , sizeof(cli_addr) );
        socklen_t cli_len = sizeof(cli_addr);
        int sock_client= accept(fdsocket, (struct sockaddr *) &cli_addr, &cli_len);

        if(sock_client==-1){
            close(fdsocket);
            perror("Accept failed");
            exit(1);
        }
        printf("Client connected\n");
        //*** creation of a new thread to treat the client ***
        if(fork() == 0){
            close(fdsocket);//close the listening socket in child process
            echo(sock_client);
            exit(EXIT_SUCCESS);
        }else{
            close(sock_client);
        }
    }

    //*** close the server socket ***
    close(fdsocket);
    return 0;
}
