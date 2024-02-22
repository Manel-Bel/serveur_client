# server_client
Implementing a server - client communication in C 
~~ if the version of the server don't have a written client , use nc command (nc -4 <host> <port_number>)

# Exercice 2 : Un client TCP pour daytime
      Écrire un client qui se connecte au service daytime de la machine lampe et affiche l’heure renvoyée. On peut utiliser “en dur” l’adresse IPv4 de lampe. Ne pas oublier de fermer la connexion.
# Exercice 3 : Un client TCP pour time
      Même question pour le service time de la machine monjetas. Au lieu de renvoyer une chaîne comme daytime, un serveur time renvoie un entier de 32 bits non-signé, représentant le nombre de secondes écoulées depuis le 1er janvier 1900. C’est différent d’un time_t Unix qui, lui, compte les secondes depuis 19701. Plusieurs conversions sont à faire :– depuis le network order au host order,– puis en time_t en retirant le nombre de secondes entre les premiers janvier 1900 et 1970,
      soit 2208988800L– enfin en chaîne pour affichage, avec ctime.
# Exercice 4 : Un client TCP pour echo
      Faire un client qui se connecte au service echo de la machine lampe de l’ufr et qui en boucle lui envoie un message et affiche la réponse du service (par exemple le client enverra Hello1, Hello2, Hello3,...,Hello10 au service et attendra la réponse du service entre chaque envoi).

# Exercice 5 : Serveur echo IPv4
 Écrire un serveur TCP implémentant le service echo (pour tout client se connectant en IPv4, le
 serveur répète au client tout ce que celui-ci lui envoie). On pourra écrire une première version
 où le serveur ne traitera qu’un seul client et s’arrêtera, et une deuxième où le serveur pourra
 traiter un nombre quelconque de clients à la suite (on verra plus tard comment traiter plusieurs
 clients en parallèle avec les threads). Attention à bien distinguer la socket associée au serveur
 des sockets associées aux clients. On verifiera que l’on ferme bien les sockets des clients à la fin
 des interactions.
 Tester le serveur en utilisant nc comme client (utilisez localhost comme nom d’hôte) 
# Exercice 6 : Jeu
 On souhaite implémenter un serveur qui exécute le jeu décrit ci-après. Lorsqu’un client se
 connecte, le serveur choisit au hasard un nombre n entre 0 et 65535. Le client peut alors faire
 jusqu’à 20 tentatives pour deviner n en envoyant des requêtes de la forme "k\n" au serveur,
 où k est un entier compris entre 0 et 65535 représenté comme chaîne de caractères. À chaque
 requête, le serveur répond :– "PLUS␣r\n" si k inférieur à n, où r est le nombre de tentatives restantes;– "MOINS␣r\n" si k est supérieur à n, où r est le nombre de tentatives restantes;– "GAGNE\n" si k est égal à n. Le serveur clôt alors la communication;– "PERDU\n" si k est différent de n et qu’il ne reste plus de tentative au client. Le serveur
 clôt alors la communication.
 4. On décide de changer le protocole afin que les messages entre le client et le serveur soient de
 taille fixe (de cette manière, nous n’avons pas besoin de chercher le caractère "\n" pour dé
tecter la fin d’un message). Au lieu d’envoyer et de recevoir des nombres comme des séquences
 de caractères 0,...,9, changer le protocole et le code de façon à ce que :– le client envoie un entier au serveur sous la forme de deux octets correspondant à son
 codage en big-endian, et– le serveur réponde également sur deux octets : un entier sur un octet indiquant le
 nombre d’essais restant (0 si la partie est terminée), suivi de ...
# Exercice 7 : Adaptation du jeu 6 avec threads et fork 
       On veut permettre au serveur d’accepter plusieurs connexions simultanées afin de permettre à plusieurs joueurs de jouer simultanément. Vous implémenterez ce serveur de deux façons : une où vous créez un nouveau processus par client et une autre où vous utiliserez des threads. En modifiant le code de la fonction server_1p.
      On veut permette à n joueurs de jouer une partie les uns contre les autres : le premier joueur qui trouve le nombre caché a gagné. Le jeu se déroulera de façon asynchrone (les joueurs jouent quand ils le souhaitent et le serveur traite les messages dans l’ordre dans lequel ils arrivent) et le nombre d’essais est limité à k. La partie s’arrête dès que tous les joueurs ont trouvé le nombre caché ou bien ont épuisé leurs essais. Après chaque essai d’un joueur, celui-ci reçoit une réponse consistant à donner le résultat du dernier essai, sous la forme d’une ligne ressemblant à         – "GAGNE␣r\n", si le joueur a trouvé le nombre caché. Ici r est le rang du joueur (1 pour premier à trouver le nombre, 2 pour deuxième, ...)
      – "PERDU\n", si le joueur n’a pas trouvé et qu’il ne reste plus d’essais, ou bien s’il a abandonné.– "PLUS␣r\n" ou "MOINS␣r\n", sinon, en fonction de la comparaison de la valeur de l’essai courant avec le nombre à deviner. Ici, r est le nombre d’essais restant pour ce joueur.
