/* fichier commun possible : protocole_ex3.h */

#include <sys/types.h>

// Clé commune pour la file de messages
#define CLE_FILE 1234

// Structure des messages échangés
struct message {
    long type;          // type du message (1 pour requêtes, pid client pour réponses)
    pid_t pid_client;   // pid du client qui a envoyé la requête
    char operateur;     // '+', '-', '*', '/'
    double op1;         // premier opérande
    double op2;         // deuxième opérande
    double resultat;    // résultat du calcul
    int erreur;         // 0 = ok, 1 = division par zéro, 2 = opérateur inconnu
};
