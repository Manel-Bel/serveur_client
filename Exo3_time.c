#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PORT 37  //numero de port du service time avec grep time /etc/services
#define ADDR "194.254.199.95"  //adresse du serveur monjetas 

int main(){

  uint32_t timeval;
  time_t time_unix;
  char *time_string;

  //*** creation de la socket ***
  int fdsocket = socket(PF_INET, SOCK_STREAM, 0);
  if(fdsocket == -1){
      perror("creation socket");
      exit(1);
  }
  //*** creation de l'adresse du destinataire (serveur) ***
  struct sockaddr_in addsock; 
  memset(&addsock, 0, sizeof(addsock));
  addsock.sin_family= AF_INET;
  addsock.sin_port = htonl(PORT);
  inet_pton(AF_INET, ADDR, &addsock.sin_addr);


  //*** demande de connexion au serveur ***
  int r = connect(fdsocket,(struct sockaddr *)&addsock, sizeof(addsock)) ; 
  if(r == -1){
    perror("echec de la connexion");
    close(fdsocket);
    exit(2);
  }
  

  //lecture du resu renvoyé == la date 
  r = recv(fdsocket,&timeval, sizeof(timeval), 0);
  if(r == -1){
    perror("Erreur lors de la réception des données du serveur");
    close(fdsocket);
    exit(EXIT_FAILURE);
  }else if(r == 0){
    perror("Connexion terminé");
    exit(EXIT_FAILURE);
  }else{
    printf("Connexion établie\n");
    // Convertir le time value de network order à host order
    timeval = ntohl(timeval);
    printf("timeval %d\n",timeval);
    // Soustraire le décalage == conv en time_t entre 1900 et 1970
    time_unix = (timeval - 2208988800L);
    printf("time_unix %ld\n",time_unix);

    //pour l'affichage 
    time_string = ctime(&time_unix);
    printf("time_string %s\n",time_string);
  }

   //*** fermeture de la socket ***
   close(fdsocket);
      
  return 0;
}
