#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define NCLIENTS 3
#define N_ATTEMPTS 20 
#define PORT 4242

char *server = "\033[34m[server]\033[m";

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct Param{
    int id;
    int sock;
    int *rang;
    unsigned short int server_number;
};
typedef struct Param Param;


void *game_np(void *arg) {
    //cast void *arg to our structure 
    Param *param = (Param *) arg;

    unsigned short int guess;
    int r, attempts = N_ATTEMPTS;
    int win = 0;
    char buff_in[100];

    while (attempts > 0) {
        r = recv(param->sock, buff_in, 100, 0);
        if(r <= 0) break;
        sscanf(buff_in, "%hu", &guess);
        printf("Player \033[32m[%d]\033[m has send : %d\n",param->id, guess);

        char response[20];
        attempts--;

        if (param->server_number < guess)
            sprintf(response, "MOINS %d\n", attempts);
        else if (param->server_number > guess)
            sprintf(response, "PLUS %d\n", attempts);
        else if(guess == param->server_number){
            win = 1;
            pthread_mutex_lock(&lock);
                *(param->rang) += 1;
            pthread_mutex_unlock(&lock);
            sprintf(response, "GAGNE %d\n",*(param->rang));
        }
        if(attempts == 0 && !win){
            sprintf(response, "PERDU\n");
        }
        send(param->sock, response, strlen(response), 0);
        if(win) break;
    }
    printf("End of the game for player\033[32m[%d]\033[m\n",param->id);
    close(param->sock);
    free(param);
    return NULL;
}

int server_np() {
    int serv_sock;
    if((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("creation of the socket failed");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        close(serv_sock);
        perror("Binding failed");
        exit(1);
    }

    if (listen(serv_sock, 0) == -1) {
        close(serv_sock);
        perror("listen");
        exit(-1);
    }
    printf("Server is waiting for a connexions...\n");

    pthread_t *threads = malloc(NCLIENTS * sizeof(pthread_t));
    int *clients_sock = malloc(NCLIENTS * sizeof(int));
    int *rang = malloc(sizeof(int));
    if (rang == NULL) {
        close(serv_sock);
        perror("malloc");
        exit(1);
    }
    *rang = 0;
    int count = 0;

    srand(time(NULL));

    // random value between 0 and 65536
    unsigned short int server_number ;
    server_number = rand() % (1 << 16); 

    printf("%s mystery number for this game = %hu\n",server, server_number);

    while (count < NCLIENTS) {
        printf("%s current count %d, we need %d\n", server, count, NCLIENTS);
        clients_sock[count] = accept(serv_sock, NULL, NULL);
        if (clients_sock[count] == -1) {
            close(serv_sock);
            perror("Accept failed");
            exit(1);
        } 
        else {
            printf("Client connected\n");
            count ++;
        }
    }

    printf("%s the game can start now\n",server);

    for(int i = 0; i < NCLIENTS; i++){
        printf("%s Creating thread %d\n", server, i);
        Param *par = malloc(sizeof(Param));
        par->id = i;
        par->sock = clients_sock[i];
        par->rang = rang;
        par->server_number = server_number;
        if (pthread_create(&threads[i], NULL, game_np, par)) {
            perror("pthread_create");
            continue;
        }
    }
    for(int i = 0; i < NCLIENTS ; i++)
        if(pthread_join(threads[i], NULL)== -1){
            perror("pthread_join");
            continue;
        }

    free(threads);
    free(clients_sock);
    close(serv_sock);
    free(rang);
    pthread_mutex_destroy(&lock);
    return 0;
}

int main(int argn,char** args) {
  if(argn <= 1|| strcmp(args[1],"-np")==0){
    server_np();
  }
}