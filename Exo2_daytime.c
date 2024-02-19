#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 13  //numero de port du service daytime
#define ADDR "192.168.70.237"  //adresse du serveur lampe avec host lampe 

int main(){

  char buf[20];
  int fdsocket = socket(PF_INET, SOCK_STREAM, 0);
  //*** creation de la socket ***
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
    exit(2);
  }
  

  //lecture du resu renvoyé == la date 
  r = recv(fdsocket,buf, sizeof(buf), 0);
  if(r == -1){
    perror("Erreur lors de la réception des données du serveur");
    exit(EXIT_FAILURE);
  }else if(r == 0){
    perror("Connextion terminé");
    exit(EXIT_FAILURE);
  }else{
    buf[r] = '\0'; // Ajout du caractère de fin de chaîne
    printf("Heure renvoyée par le serveur : %s\n", buf);
  }

   //*** fermeture de la socket ***
   close(fdsocket);
      
  return 0;
}
