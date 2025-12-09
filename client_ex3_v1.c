#include <stdio.h>      // pour printf, scanf, perror
#include <stdlib.h>     // pour exit, EXIT_FAILURE
#include <unistd.h>     // pour getpid
#include <sys/types.h>  // types systèmes
#include <sys/ipc.h>    // pour les clés IPC
#include <sys/msg.h>    // pour msgget, msgrcv, msgsnd
#include "protocole_ex3.h"  // même structure et même clé que le serveur

int main(void) {
    int msgid;
    struct message msg;

    // Accès (ou création) à la file de messages
    msgid = msgget(CLE_FILE, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Erreur msgget (client)");
        exit(EXIT_FAILURE);
    }

    // Lecture de l'opération au clavier
    printf("Client : saisir l'opération (ex : 3 + 5) : ");
    scanf("%lf %c %lf", &msg.op1, &msg.operateur, &msg.op2);

    // Remplissage des champs du message
    msg.type = 1;              // type 1 pour les requêtes client -> serveur
    msg.pid_client = getpid(); // pour identifier le client

    // Envoi de la requête au serveur
    if (msgsnd(msgid,
               &msg,
               sizeof(struct message) - sizeof(long),
               0) == -1) {
        perror("Erreur msgsnd (client)");
        exit(EXIT_FAILURE);
    }

    printf("Client (%d) : requête envoyée : %.2f %c %.2f\n",
           msg.pid_client, msg.op1, msg.operateur, msg.op2);

    // Attente de la réponse dont le type est le pid du client
    if (msgrcv(msgid,
               &msg,
               sizeof(struct message) - sizeof(long),
               msg.pid_client,  // type attendu = pid du client
               0) == -1) {
        perror("Erreur msgrcv (client)");
        exit(EXIT_FAILURE);
    }

    // Affichage du résultat ou d'un message d'erreur
    if (msg.erreur == 0) {
        printf("Client : résultat = %.2f\n", msg.resultat);
    } else if (msg.erreur == 1) {
        printf("Client : erreur, division par zéro\n");
    } else if (msg.erreur == 2) {
        printf("Client : erreur, opérateur inconnu\n");
    }

    // Ici, pas de suppression de la file :
    // le serveur et d'autres clients peuvent encore l'utiliser

    return 0;
}
