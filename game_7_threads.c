#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int server_1p_thread();


int main(int argn,char** args) {
  if(argn<=1|| strcmp(args[1],"-1p")==0){
    server_1p_thread();
  }
}

/***************LES FONCTIONS DE JEU ******************/

/**
 * fait tourner une partie pour un joueur dont le socket est passé en argument
 */
void *game_1p(void *arg) {
    //recup de la socket du client
    int sock = *((int *) arg);
    srand(time(NULL) + sock);

    // une valeur aléatoire mystere entre 0 et 65536
    unsigned short int n = rand() % (1 << 16); 
    printf("nb mystere pour partie socket %d = %d\n", sock, n);

    unsigned short int guess;  // le nb proposé par le joueur, sur 2 octets
    int taille = 0;
    int tentatives = 20;
    int gagne = 0;
    char buff_in[100];
    while ((taille = recv(sock, buff_in, 100, 0)) > 0) {
        sscanf(buff_in, "%hu", &guess);
        printf("Joueur courant a envoyé : %d\n", guess);
        char reponse[20];
        if (n < guess || n > guess) {
            tentatives--;
        }
        if (tentatives == 0)
            sprintf(reponse, "PERDU\n");
        else if (n < guess)
            sprintf(reponse, "MOINS %d\n", tentatives);
        else if (n > guess)
            sprintf(reponse, "PLUS %d\n", tentatives);
        else {
            sprintf(reponse, "GAGNE\n");
            gagne = 1;
        }
        send(sock, reponse, strlen(reponse), 0);
        if (gagne || !tentatives) {
            
        };
    }
    if(taille < 0){
        perror("recv");
        close(sock);
        free(arg);
        int *ret = malloc(sizeof(int));
        *ret = 1;
        pthread_exit(ret);
    }
    printf("Fin de partie\n");

    close(sock);
    free(arg);
    return NULL;
}

/**
 * serveur pour jeu 1 player avec 1 connexion à la fois
 */
int server_1p_thread() {
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(4243);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0) {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  r = listen(serv_sock, 0);
  if (r != 0) {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  while (1) {
    struct sockaddr_in addrclient;
    socklen_t size=sizeof(addrclient);

    int *client_sock = malloc(sizeof(int));

    *client_sock = accept(serv_sock,(struct sockaddr *) &addrclient, &size);

    if (client_sock < 0) {
      fprintf(stderr, "Échec de accept");
      exit(-1);
    } 
    else {
      printf("Connexion acceptee, nouvelle partie lancée.\n");
      pthread_t thread;
      if(pthread_create(&thread, NULL, game_1p, client_sock)){
        perror("pthread_creat");
        continue;
      }
    //    pthread_join(thread, NULL);
    }
  }
  close(serv_sock);
  return 0;
}
