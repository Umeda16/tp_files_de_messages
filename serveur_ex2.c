/*Boubetra Nihad - Groupe TP2*/

#include <stdio.h>      // pour printf, perror
#include <stdlib.h>     // pour exit, EXIT_FAILURE
#include <sys/types.h>  // types systèmes
#include <sys/ipc.h>    // pour les clés IPC
#include <sys/msg.h>    // pour msgget, msgrcv, msgsnd

// Clé de la file de messages (doit être la même dans le client)
#define CLE_FILE 1234

// Structure utilisée pour échanger les données
struct message {
    long type;      // type du message (obligatoirement un long)
    int nb1;        // premier entier
    int nb2;        // deuxième entier
    int resultat;   // résultat de l'addition
};

int main(void) {
    int msgid;
    struct message msg;

    // Création ou accès à la file de messages
    // 0666 : droits en lecture/écriture pour tout le monde
    msgid = msgget(CLE_FILE, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Erreur msgget (serveur)");
        exit(EXIT_FAILURE);
    }

    // Le serveur attend un message de type 1 contenant les deux entiers
    if (msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), 1, 0) == -1) {
        perror("Erreur msgrcv (serveur)");
        exit(EXIT_FAILURE);
    }

    // Affichage des valeurs reçues
    printf("Serveur : reçu %d et %d\n", msg.nb1, msg.nb2);

    // Calcul de l'addition
    msg.resultat = msg.nb1 + msg.nb2;

    // Préparation du message de réponse vers le client (type 2)
    msg.type = 2;

    // Envoi du résultat au client
    if (msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0) == -1) {
        perror("Erreur msgsnd (serveur)");
        exit(EXIT_FAILURE);
    }

    printf("Serveur : envoi du résultat %d au client\n", msg.resultat);

    // Fin du serveur
    return 0;
}
