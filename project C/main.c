#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define CODE_ADMIN 000000
#define CODE_UTILISATEUR_POUR_TEST 111111

typedef struct Utilisateur {
    char Nom_Prenom[20];
    int code_Utilisateur;
    struct Utilisateur *suivant;
} Utilisateur;

typedef struct {
    Utilisateur *debut;
    Utilisateur *fin;
    int taille;
} Liste_Utilisateur;

void initialiser_liste(Liste_Utilisateur *L) {
    L->debut = NULL;
    L->fin = NULL;
    L->taille = 0;
}

void ajouter_utilisateur_liste_vide(Liste_Utilisateur *L, Utilisateur *nouv_elem) {
    nouv_elem->suivant = NULL;
    L->debut = nouv_elem;
    L->fin = nouv_elem;
    L->taille++;
}

void ajouter_fin_liste(Liste_Utilisateur *L, Utilisateur *nouv_elem) {
    nouv_elem->suivant = NULL;
    L->fin->suivant = nouv_elem;
    L->fin = nouv_elem;
    L->taille++;
}

void suprimer_debut_liste(Liste_Utilisateur *L) {
    if (L->taille == 0) {
        printf("La liste est vide.\n");
    } else {
        Utilisateur *temp = L->debut;
        L->debut = L->debut->suivant;
        free(temp);
        L->taille--;
    }
}

void suprimer_fin_liste(Liste_Utilisateur *L) {
    if (L->taille == 0) {
        printf("La liste est vide.\n");
    } else {
        Utilisateur *temp = L->fin;
        Utilisateur *current = L->debut;
        while (current->suivant != L->fin) {
            current = current->suivant;
        }
        current->suivant = NULL;
        L->fin = current;
        free(temp);
        L->taille--;
    }
}

void detruire(Liste_Utilisateur *L) {
    while (L->taille > 0) {
        suprimer_debut_liste(L);
    }
}

void afficher_liste(Liste_Utilisateur *L) {
    if (L->taille == 0) {
        printf("\nLa liste est vide !\n");
    } else {
        Utilisateur *current = L->debut;
        while (current != NULL) {
            printf("\nNom Complet :  %s .", current->Nom_Prenom);
            printf("\nCode ID : %d .\n", current->code_Utilisateur);
            current = current->suivant;
        }
    }
}

void sauvegarder_famille(Utilisateur *courant) {
    time_t now = time(NULL);
    if (now == -1) {
        perror("Erreur lors de l'obtention de l'heure");
        return;
    }
    struct tm *current_t = localtime(&now);
    if (current_t == NULL) {
        perror("Erreur lors de la conversion du temps");
        return;
    }
    FILE *file = fopen("historique.txt", "a");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    int NB_alpha = strlen(courant->Nom_Prenom);
    if (NB_alpha < 20) {
        for (int i = NB_alpha; i < 20 - 1; i++) {
            courant->Nom_Prenom[i] = ' ';
        }
        courant->Nom_Prenom[20 - 1] = '\0';
    }
    if (courant != NULL) {
        fprintf(file, "%s | %02d/%02d/%04d | %02d:%02d:%02d\n",
                courant->Nom_Prenom,
                current_t->tm_mday,
                current_t->tm_mon + 1,
                current_t->tm_year + 1900,
                current_t->tm_hour,
                current_t->tm_min,
                current_t->tm_sec);
    }
    fclose(file);
}

int saisir_code_utilisateur() {
    int valeur;
    char buffer[10];
    printf("\nSaisir Serie de Numero (6 chiffres) : ");
    fgets(buffer, 10, stdin);
    if (sscanf(buffer, "%d", &valeur) != 1 || strlen(buffer) != 7) { // 6 chiffres + '\n'
        printf("Entrée invalide. Veuillez saisir un code utilisateur de 6 chiffres.\n");
        return -1;
    }
    return valeur;
}

void heure() {
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    printf("\t\t\t\t\t\t\t%02d/%02d/%04d\n",
           local_time->tm_mday,
           local_time->tm_mon + 1,
           local_time->tm_year + 1900);
}

void afficher_historiques() {
    FILE *file = fopen("historique.txt", "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void chercher_historique_utilisateur() {
    char nom[20];
    printf("\nSaisir le nom de l'utilisateur a chercher : ");
    fgets(nom, 20, stdin);
    nom[strcspn(nom, "\n")] = '\0';

    FILE *file = fopen("historique.txt", "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, nom)) {
            printf("%s", line);
        }
    }
    fclose(file);
}

void sauvegarder_utilisateurs(Liste_Utilisateur *L) {
    FILE *file = fopen("utilisateurs.txt", "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    Utilisateur *current = L->debut;
    while (current != NULL) {
        fprintf(file, "%s %d\n", current->Nom_Prenom, current->code_Utilisateur);
        current = current->suivant;
    }
    fclose(file);
}

void charger_utilisateurs(Liste_Utilisateur *L) {
    FILE *file = fopen("utilisateurs.txt", "r");
    if (file == NULL) {
        printf("Aucun fichier de sauvegarde trouve. Initialisation de la liste vide.\n");
        return;
    }
    while (!feof(file)) {
        Utilisateur *nouveau = (Utilisateur *)malloc(sizeof(Utilisateur));
        if (nouveau == NULL) {
            printf("Erreur d'allocation de mémoire.\n");
            break;
        }
        if (fscanf(file, "%s %d", nouveau->Nom_Prenom, &nouveau->code_Utilisateur) == 2) {
            if (L->taille == 0) {
                ajouter_utilisateur_liste_vide(L, nouveau);
            } else {
                ajouter_fin_liste(L, nouveau);
            }
        } else {
            free(nouveau);
        }
    }
    fclose(file);
}

void supprimer_utilisateur(Liste_Utilisateur *L, int code) {
    if (L->taille == 0) {
        printf("La liste est vide.\n");
        return;
    }
    Utilisateur *current = L->debut;
    Utilisateur *previous = NULL;
    while (current != NULL && current->code_Utilisateur != code) {
        previous = current;
        current = current->suivant;
    }
    if (current == NULL) {
        printf("Utilisateur non trouve.\n");
        return;
    }
    if (previous == NULL) {
        L->debut = current->suivant;
    } else {
        previous->suivant = current->suivant;
    }
    if (current == L->fin) {
        L->fin = previous;
    }
    free(current);
    L->taille--;
    sauvegarder_utilisateurs(L);
    printf("Utilisateur supprime avec succes.\n");
}

void ajouter_utilisateur(Liste_Utilisateur *L) {
    Utilisateur *nouveau = (Utilisateur *)malloc(sizeof(Utilisateur));
    if (nouveau == NULL) {
        printf("Erreur d'allocation de mémoire.\n");
        return;
    }
    printf("\nSaisir le nom complet de l'utilisateur : ");
    fgets(nouveau->Nom_Prenom, 20, stdin);
    nouveau->Nom_Prenom[strcspn(nouveau->Nom_Prenom, "\n")] = '\0';

    do {
        printf("Saisir le code utilisateur (6 chiffres) : ");
        nouveau->code_Utilisateur = saisir_code_utilisateur();
    } while (nouveau->code_Utilisateur == -1);

    if (L->taille == 0) {
        ajouter_utilisateur_liste_vide(L, nouveau);
    } else {
        ajouter_fin_liste(L, nouveau);
    }
    sauvegarder_utilisateurs(L);
    printf("Utilisateur ajoute avec succes.\n");
}

void liste_des_choix(Liste_Utilisateur *L) {
    int choix;
    do {
        system("cls");
        heure();
        printf("\n\t\t##### INTERFACE D'ADMINISTRTEUR : #####\n\n");
        printf("\n1-Afficher les Historiques des utilisateurs.");
        printf("\n2-Afficher liste des utilisateurs autoriser.");
        printf("\n3-Chercher des historiques d'un utilisateur.");
        printf("\n4-Supprimer un Utilisateur.");
        printf("\n5-Ajouter un utilisateur.");
        printf("\n6-Log Out.");
        printf("\n\nSaisir votre choix : ");
        scanf("%d", &choix);
        getchar();
        switch (choix) {
            case 1:
                afficher_historiques();
                break;
            case 2:
                afficher_liste(L);
                break;
            case 3:
                chercher_historique_utilisateur();
                break;
            case 4:
                printf("\nSaisir le code de l'utilisateur a supprimer : ");
                int code;
                scanf("%d", &code);
                supprimer_utilisateur(L, code);
                break;
            case 5:
                getchar();
                ajouter_utilisateur(L);
                break;
            case 6:
                printf("\nDeconnexion en cours...\n");
                break;
            default:
                printf("\nChoix invalide, veuillez reessayer.\n");
        }
        if (choix != 6) {
            printf("\nAppuyez sur une touche pour continuer...\n");
            getchar();
        }
    } while (choix != 6);
}

int main() {
    Liste_Utilisateur *L = (Liste_Utilisateur *)malloc(sizeof(Liste_Utilisateur));
    if (L == NULL) {
        printf("Erreur d'allocation de mémoire pour la liste.\n");
        return 1;
    }
    initialiser_liste(L);
    charger_utilisateurs(L);

    Utilisateur ADMIN = {"ADMIN", CODE_ADMIN};
    Utilisateur P1 = {"Amine Genin", 111111};  // 1 * 6
    if (L->taille == 0) {
        ajouter_utilisateur_liste_vide(L, &P1);
        sauvegarder_utilisateurs(L);
    }

    int code_administrateur = CODE_ADMIN;
    int numero;

    while (1) {
        system("cls");
        numero = saisir_code_utilisateur();
        if (numero == code_administrateur) {
            sauvegarder_famille(&ADMIN);
            liste_des_choix(L);
            sleep(1);
        } else {
            Utilisateur *courant = L->debut;
            int found = 0;
            while (courant != NULL) {
                if (numero == courant->code_Utilisateur) {
                    system("cls");
                    sauvegarder_famille(courant);
                    printf("\nValide !\n");
                    sleep(1);
                    found = 1;
                    break;
                }
                courant = courant->suivant;
            }
            if (!found) {
                system("cls");
                printf("\nInvalide !\n");
                sleep(1);
            }
        }
    }

    detruire(L);
    free(L);
    return 0;
}
