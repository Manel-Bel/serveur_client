#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 7  //numero de port du service echo
#define ADDR "192.168.70.237"  //adresse du serveur lampe
#define MSG "Hello"

int main(){
    char bufsend[20];
    char bufrecv[20];

    //*** creation de la socket ***
    int fdsocket = socket(PF_INET, SOCK_STREAM, 0);
    if( fdsocket == -1){
        perror("creation socket");
        exit(1);
    }
    //*** creation de l'adresse du destinataire (serveur) ***
    struct sockaddr_in addsock; 
    memset( &addsock , 0 , sizeof(addsock));
    addsock.sin_family= AF_INET;
    addsock.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDR, &addsock.sin_addr);


    //*** demande de connexion au serveur ***
    int r = connect(fdsocket,(struct sockaddr *) &addsock, sizeof(addsock)) ; 
    if(r == -1){
    perror("echec de la connexion");
    close(fdsocket);
    exit(2);
    }

    /* envoie du message en boucle */
    for(int i = 0; i< 5;i++){
        sprintf(bufsend, "%s%d", MSG,i);
        //envoie du msg avec send or write 
        int r = send(fdsocket, bufsend, strlen(bufsend), 0);
        // write(fdsocket, bufsend, strlen(bufsend));
        if(r == -1){
            perror("erreur ecriture");
            close(fdsocket);
            exit(3);
        }
        memset(bufrecv, 0, sizeof(bufrecv));
        //attendre la rep du serveur 
        if((r = recv(fdsocket, bufrecv, r * sizeof(char), 0)) == -1){
            perror("Erreur lors de la réception de la réponse");
            close(fdsocket);
            exit(1);
        }

        printf("Rep du serveur: %s\n", bufrecv);

        memset(bufrecv,0,sizeof(bufrecv));//vider le buffer
        memset(bufsend,0,sizeof(bufsend));//vider le buffer

    }

    //*** fermeture de la socket ***
    close(fdsocket);
        
    return 0;
}
