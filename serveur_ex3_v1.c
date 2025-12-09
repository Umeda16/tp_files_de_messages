/*Boubetra Nihad - Groupe TP2*/

#include <stdio.h>      // pour printf, perror
#include <stdlib.h>     // pour exit, EXIT_FAILURE
#include <sys/types.h>  // types systèmes
#include <sys/ipc.h>    // pour les clés IPC
#include <sys/msg.h>    // pour msgget, msgrcv, msgsnd
#include "protocole_ex3.h"  // contient la définition de struct message et CLE_FILE

int main(void) {
    int msgid;
    struct message msg;

    // Création ou accès à la file de messages
    msgid = msgget(CLE_FILE, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Erreur msgget (serveur)");
        exit(EXIT_FAILURE);
    }

    printf("Serveur : en attente de requêtes...\n");

    // Boucle infinie : le serveur traite les requêtes les unes après les autres
    while (1) {
        // Attente d'une requête de type 1 (client -> serveur)
        if (msgrcv(msgid,
                   &msg,
                   sizeof(struct message) - sizeof(long),
                   1,           // type attendu
                   0) == -1) {
            perror("Erreur msgrcv (serveur)");
            exit(EXIT_FAILURE);
        }

        printf("Serveur : requête reçue de %d : %.2f %c %.2f\n",
               msg.pid_client, msg.op1, msg.operateur, msg.op2);

        // Initialisation : pas d'erreur au départ
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
                    // Cas particulier : division par zéro
                    msg.erreur = 1;
                } else {
                    msg.resultat = msg.op1 / msg.op2;
                }
                break;
            default:
                // Opérateur non reconnu
                msg.erreur = 2;
                break;
        }

        // Le type de la réponse devient le pid du client
        // pour que chaque client reçoive uniquement son résultat
        msg.type = msg.pid_client;

        // Envoi du message de réponse au client
        if (msgsnd(msgid,
                   &msg,
                   sizeof(struct message) - sizeof(long),
                   0) == -1) {
            perror("Erreur msgsnd (serveur)");
            exit(EXIT_FAILURE);
        }

        printf("Serveur : réponse envoyée au client %d\n", msg.pid_client);
    }

    // Théoriquement jamais atteint (serveur arrêté par Ctrl+C)
    return 0;
}
