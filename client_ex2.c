/*Boubetra Nihad - Groupe TP2*/

#include <stdio.h>      // pour printf, scanf, perror
#include <stdlib.h>     // pour exit, EXIT_FAILURE
#include <s./ys/types.h>  // types systèmes
#include <sys/ipc.h>    // pour les clés IPC
#include <sys/msg.h>    // pour msgget, msgrcv, msgsnd

// Même clé que dans le serveur
#define CLE_FILE 1234

// Même structure que dans le serveur
struct message {
    long type;      // type du message
    int nb1;        // premier entier
    int nb2;        // deuxième entier
    int resultat;   // résultat renvoyé par le serveur
};

int main(void) {
    int msgid;
    struct message msg;

    // Accès (ou création) à la file de messages
    msgid = msgget(CLE_FILE, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Erreur msgget (client)");
        exit(EXIT_FAILURE);
    }

    // Lecture des deux entiers au clavier
    printf("Client : saisir le premier entier : ");
    scanf("%d", &msg.nb1);

    printf("Client : saisir le deuxième entier : ");
    scanf("%d", &msg.nb2);

    // Préparation du message à envoyer au serveur
    msg.type = 1;  // type 1 pour le message client -> serveur

    // Envoi du message contenant les 2 entiers
    if (msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0) == -1) {
        perror("Erreur msgsnd (client)");
        exit(EXIT_FAILURE);
    }

    printf("Client : envoi de %d et %d au serveur\n", msg.nb1, msg.nb2);

    // Attente de la réponse du serveur (type 2)
    if (msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), 2, 0) == -1) {
        perror("Erreur msgrcv (client)");
        exit(EXIT_FAILURE);
    }

    // Affichage du résultat reçu
    printf("Client : résultat reçu = %d\n", msg.resultat);

    // Suppression de la file de messages (nettoyage)
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Erreur msgctl (client)");
        exit(EXIT_FAILURE);
    }

    return 0;
}
