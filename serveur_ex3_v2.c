#include <stdio.h>      // pour printf, perror
#include <stdlib.h>     // pour exit, EXIT_FAILURE
#include <unistd.h>     // pour fork, _exit
#include <signal.h>     // pour signal, SIGCHLD
#include <sys/types.h>  // types systèmes
#include <sys/ipc.h>    // pour les clés IPC
#include <sys/msg.h>    // pour msgget, msgrcv, msgsnd
#include <sys/wait.h>   // pour waitpid et WNOHANG
#include "protocole_ex3.h"  // même structure et même clé que pour la version 1

// Fonction appelée quand un fils se termine
// Permet d'éviter les processus zombies
void handler_sigchld(int sig) {
    (void)sig; // évite un warning sur le paramètre non utilisé
    // Attente non bloquante de tous les fils terminés
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        // rien à faire, nettoyage des fils
    }
}

int main(void) {
    int msgid;
    struct message msg;

    // Installation du gestionnaire pour SIGCHLD
    // Ainsi, les processus fils terminés sont automatiquement nettoyés
    signal(SIGCHLD, handler_sigchld);

    // Création ou accès à la file de messages
    msgid = msgget(CLE_FILE, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Erreur msgget (serveur)");
        exit(EXIT_FAILURE);
    }

    printf("Serveur parallèle : en attente de requêtes...\n");

    // Boucle principale : le serveur reçoit les requêtes une par une
    while (1) {
        // Attente d'un message de type 1 (requête d'un client)
        if (msgrcv(msgid,
                   &msg,
                   sizeof(struct message) - sizeof(long),
                   1,
                   0) == -1) {
            perror("Erreur msgrcv (serveur)");
            exit(EXIT_FAILURE);
        }

        printf("Serveur : requête reçue de %d : %.2f %c %.2f\n",
               msg.pid_client, msg.op1, msg.operateur, msg.op2);

        // Création d'un processus fils pour traiter cette requête
        pid_t pid = fork();
        if (pid == -1) {
            perror("Erreur fork (serveur)");
            // en cas d'erreur, le serveur continue sa boucle
            continue;
        }

        if (pid == 0) {
            /********** Processus fils : traite une requête **********/

            // Initialisation de l'indicateur d'erreur
            msg.erreur = 0;

            // Calcul selon l'opérateur demandé
            switch (msg.operateur) {
                case '+':
                    msg.resultat = msg.op1 + msg.op2;
                    break;
                case '-':
                    msg.resultat = msg.op1 - msg.op2;
                    break;
                case '*':
                    msg.resultat = msg.op1 * msg.op2;
                    break;
                case '/':
                    if (msg.op2 == 0) {
                        msg.erreur = 1;      // division par zéro
                    } else {
                        msg.resultat = msg.op1 / msg.op2;
                    }
                    break;
                default:
                    msg.erreur = 2;          // opérateur inconnu
                    break;
            }

            // Le type de la réponse est le pid du client
            msg.type = msg.pid_client;

            // Envoi de la réponse au client
            if (msgsnd(msgid,
                       &msg,
                       sizeof(struct message) - sizeof(long),
                       0) == -1) {
                perror("Erreur msgsnd (fils serveur)");
                _exit(EXIT_FAILURE);
            }

            printf("Serveur (fils %d) : réponse envoyée au client %d\n",
                   getpid(), msg.pid_client);

            // Fin du fils après traitement de la requête
            _exit(EXIT_SUCCESS);
        }

        /********** Processus père : retourne immédiatement à l'écoute **********/
        printf("Serveur (père) : requête confiée au fils %d\n", pid);
        // le père ne fait rien de plus et revient au début de la boucle
    }

    // Normalement jamais atteint
    return 0;
}
