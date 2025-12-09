/*Boubetra Nihad - Groupe TP2*/

#include <stdio.h>      // pour printf, perror
#include <stdlib.h>     // pour exit, EXIT_FAILURE
#include <string.h>     // pour strcpy, strlen
#include <unistd.h>     // pour fork, getpid
#include <sys/types.h>  // types systèmes
#include <sys/ipc.h>    // pour les files de messages
#include <sys/msg.h>    // pour msgget, msgsnd, msgrcv, msgctl
#include <sys/wait.h>   // pour wait

// Définition de la structure utilisée pour la file de messages
struct message {
    long type;          // type du message (obligatoirement un long)
    char texte[100];    // contenu du message
};

int main(void) {
    int msgid;
    pid_t pid;
    struct message msg;

    // Création de la file de messages
    // IPC_PRIVATE permet de créer une file uniquement pour ces deux processus
    msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Erreur msgget");
        exit(EXIT_FAILURE);
    }

    // Création du deuxième processus
    pid = fork();
    if (pid == -1) {
        perror("Erreur fork");
        // En cas d’erreur, suppression de la file avant de quitter
        msgctl(msgid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /********** Processus 2 (fils) **********/

        // Le processus 2 attend de recevoir le message du processus 1
        // Ici on attend un message de type 1
        if (msgrcv(msgid, &msg, sizeof(msg.texte), 1, 0) == -1) {
            perror("Erreur msgrcv (processus 2)");
            exit(EXIT_FAILURE);
        }

        // Affichage du message reçu
        printf("Processus 2 reçoit : %s\n", msg.texte);

        // Préparation du message de réponse pour le processus 1
        msg.type = 2; // type 2 pour différencier le sens du message
        strcpy(msg.texte, "je suis le processus 2");

        // Envoi du message de réponse au processus 1
        if (msgsnd(msgid, &msg, sizeof(msg.texte), 0) == -1) {
            perror("Erreur msgsnd (processus 2)");
            exit(EXIT_FAILURE);
        }

        // Fin du processus 2
        exit(EXIT_SUCCESS);

    } else {
        /********** Processus 1 (parent) **********/

        // Préparation du message destiné au processus 2
        msg.type = 1; // type 1 pour le premier envoi
        strcpy(msg.texte, "je suis le processus 1");

        // Envoi du message au processus 2
        if (msgsnd(msgid, &msg, sizeof(msg.texte), 0) == -1) {
            perror("Erreur msgsnd (processus 1)");
            // En cas d’erreur, suppression de la file avant de quitter
            msgctl(msgid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }

        // Le processus 1 attend maintenant la réponse du processus 2
        // Ici on attend un message de type 2
        if (msgrcv(msgid, &msg, sizeof(msg.texte), 2, 0) == -1) {
            perror("Erreur msgrcv (processus 1)");
            // Suppression de la file en cas d’erreur
            msgctl(msgid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }

        // Affichage du message reçu
        printf("Processus 1 reçoit : %s\n", msg.texte);

        // Attente de la fin du processus 2 pour bien nettoyer
        wait(NULL);

        // Suppression de la file de messages, car elle n’est plus utilisée
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("Erreur msgctl (suppression file)");
            exit(EXIT_FAILURE);
        }

        // Fin du processus 1
        exit(EXIT_SUCCESS);
    }
}
