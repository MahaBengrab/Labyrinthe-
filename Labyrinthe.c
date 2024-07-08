#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#define LIGNES 15
#define COLONNES 15
#define MUR 0
#define CASE_LIBRE 1
#define ENTREE 2
#define SORTIE 3
#define CASE_VISITEE -1

// Prototypes de fonctions
int** setAllocationCarre();
void initMatrice(int **m);
void afficheMatrice(int **m);
void lireFichier(int **m);
void desallocationMatrice(int **m);
void trouverEntree(int **m, int* entreeXY);
void trouverSortieSansThread(int **m, int x, int y);

//Allocation de la matrice
int** setAllocationCarre(){
    int **m = malloc(LIGNES * sizeof(int*));
    for(int i = 0; i < LIGNES; ++i){
        m[i] = malloc(COLONNES * sizeof(int));
    }
    return m;
}
//Initialisation de la matrice
void initMatrice(int **m){
    for(int i = 0; i < LIGNES; i++){
        for(int j = 0; j < COLONNES; j++){
            m[i][j] = 0;
        }
    }
}
//Affichage de la matrice
void afficheMatrice(int **m){
    for (int i = 0; i < LIGNES; ++i){
        for (int j = 0; j < COLONNES; ++j){
            printf("%4d", m[i][j]);
        }
        printf("\n");
    }
}
//Methode pour lire le fichier qui contient le labyrinthe
//Au lieu d'avoir une matrice qui contient des elements char ils deviennent des entiers.
void lireFichier(int **m){
    FILE* fichier = NULL;
    char caractere;
    fichier = fopen("C:\\Users\\Mehdi\\OneDrive\\Bureau\\LabyrintheCLion\\fichierLabyrinthe.txt","r");
    if(fichier == NULL){
        printf("Le fichier est encore fermee\n");
    }
    while(! feof(fichier)){
        for (int i = 0; i < LIGNES; i++){
            for (int j = 0; j < COLONNES; j++){
                //Fonction getc pour lire le fichier caratere par caractere.
                caractere = fgetc(fichier);
                if(caractere == '0'){
                    m[i][j] = MUR;
                } else if(caractere == '1'){
                    m[i][j] = CASE_LIBRE;
                } else if(caractere == '2'){
                    m[i][j] = ENTREE;
                } else if(caractere == '3'){
                    m[i][j] = SORTIE;
                }
            }
            fgetc(fichier);
        }
    }
    fclose(fichier);
}
//Desallocation de la matrice.
void desallocationMatrice(int **m){
    for (int i = 0; i < LIGNES; ++i){
        free(m[i]);
    }
    free(m);
}
//Methode pour trouver l entrée du labyrinthe
void trouverEntree(int **m, int* entreeXY) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (m[i][j] == ENTREE) {
                printf("%d %d\n", i, j);
                entreeXY[0] = i;
                entreeXY[1] = j;
                return;
            }
        }
    }
}
//Parcours du labyrinthe d'une maniere récurisve pour trouver la sortie.
void trouverSortieSansThread(int **m, int x, int y) {
    // Si on est sur la sortie, on a trouvé la solution
    if (m[x][y] == SORTIE) {
        printf("Sortie trouvee");
    }
    // Marquer la position courante comme visitée
    m[x][y] = CASE_VISITEE;

    // Explorer les positions adjacentes non visitées
    //En Haut
    if (x > 0 && (m[x-1][y] == CASE_LIBRE || m[x-1][y] == SORTIE) && m[x-1][y] != CASE_VISITEE) {
        (trouverSortieSansThread(m, x-1, y));
    }
    //En Bas
    if (x < LIGNES-1 && (m[x+1][y] == CASE_LIBRE || m[x+1][y] == SORTIE) && m[x+1][y] != CASE_VISITEE) {
        (trouverSortieSansThread(m, x+1, y));
    }
    //En Gauche
    if (y > 0 && (m[x][y-1] == CASE_LIBRE || m[x][y-1] == SORTIE) && m[x][y-1] != CASE_VISITEE) {
        (trouverSortieSansThread(m, x, y-1));
    }
    //A Droite
    if (y < COLONNES-1 && (m[x][y+1] == CASE_LIBRE || m[x][y+1] == SORTIE) && m[x][y+1] != CASE_VISITEE) {
        (trouverSortieSansThread(m, x, y+1));
    }
}

struct Coordonnees{
    int x1;
    int y1;
    int **m;
};
//Parcours du labyrinthe d'une maniere récurisve pour trouver la sortie avec des threads.
void *trouverSortieMultiThread(void *params) {
        //A l'aide du cast on converti le pointeur params (qui est en void) en pointeur de type Coordonnees
        struct Coordonnees *coord = (struct Coordonnees*) params;
        int x = coord->x1;
        int y = coord->y1;
        int **m = coord->m;

        //Si on rencontre 3 alors on est sur la sortie et le programme a trouvé la solution
        if (m[x][y] == SORTIE) {
            printf("Bravo vous avez trouvé la sortie !!\n");
            return NULL;
        }

        //On marque la case courante comme visitée.
        m[x][y] = CASE_VISITEE;

        // On vérifie les cases adjacentes non visitées
        //Creation des pthread
        pthread_t threads[4];
        int nombrethreads = 0;
        //La Ligne d avant
        if (x > 0 && (m[x-1][y] == CASE_LIBRE || m[x-1][y] == SORTIE) && m[x-1][y] != CASE_VISITEE) {
            struct Coordonnees coordHaut = {x-1, y, m};
            pthread_create(&threads[nombrethreads], NULL, trouverSortieMultiThread, &coordHaut);
            nombrethreads++;
        }
        //La Ligne d apres
        if (x < LIGNES-1 && (m[x+1][y] == CASE_LIBRE || m[x+1][y] == SORTIE) && m[x+1][y] != CASE_VISITEE) {
            struct Coordonnees coordBas = {x+1, y, m};
            pthread_create(&threads[nombrethreads], NULL, trouverSortieMultiThread, &coordBas);
            nombrethreads++;
        }
        //La colonne d avant
        if (y > 0 && (m[x][y-1] == CASE_LIBRE || m[x][y-1] == SORTIE) && m[x][y-1] != CASE_VISITEE) {
            struct Coordonnees coordGauche = {x, y-1, m};
            pthread_create(&threads[nombrethreads], NULL, trouverSortieMultiThread, &coordGauche);
            nombrethreads++;
        }
        //La colonne d apres
        if (y < COLONNES-1 && (m[x][y+1] == CASE_LIBRE || m[x][y+1] == SORTIE) && m[x][y+1] != CASE_VISITEE) {
            struct Coordonnees coordDroite = {x, y+1, m};
            pthread_create(&threads[nombrethreads], NULL, trouverSortieMultiThread, &coordDroite);
            nombrethreads++;
        }
        //On attends la fin des threads
        for (int i = 0; i < nombrethreads; i++) {
            pthread_join(threads[i], NULL);
        }
}
int main(){
    int **matrice;
    // Creation d'un thread pour exécuter la fonction trouverSortieMultiThread
    pthread_t thread;

    //Allocation de la matrice
    matrice = setAllocationCarre();

    //Initialisation de la matrice
    initMatrice(matrice);

    //Lecture du fichier où se trouve le labyrinthe
    lireFichier(matrice);

    //Affichage de la matrice
    afficheMatrice(matrice);

    int entreeXY[2];
    //Les coordonnées de depart du labyrinthe
    trouverEntree(matrice, entreeXY);

    //trouverSortieSansThread(matrice, entreeXY[0],entreeXY[1]);


    // Creation de la structure Coordonnees avec les coordonnées de départ (case qui contient le numero 2) et la matrice du labyrinthe
    struct Coordonnees coord = {entreeXY[0], entreeXY[1], matrice};
    pthread_create(&thread, NULL, trouverSortieMultiThread, &coord);

    // Attendre la fin de l'exécution du thread
    pthread_join(thread, NULL);

    //Desallocation de la matrice
    desallocationMatrice(matrice);
    return 0;
}





