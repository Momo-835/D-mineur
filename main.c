#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "GfxLib.h"

#define LargeurFenetre 800
#define HauteurFenetre 600

typedef struct {
    int largeur;
    int hauteur;
    int mines;
    int **grille;
} GrilleDemineur;

GrilleDemineur grille;

void gestionEvenement(EvenementGfx evenement);
void lancerJeu();
void bouttonsDesNiveaux();
void commence(int x, int y, bool *fond, bool *jeuStrat, int *niveau, bool *enJeu);
void afficherGrille(int niveau);
void afficherPageJeu(int niveau);
void initialiserGrille(int largeur, int hauteur, int mines);
void afficherGrilleDemineur(int largeur, int hauteur);
void afficherBoutonRetour();

int main(int argc, char **argv)
{
    initialiseGfx(argc, argv);
    prepareFenetreGraphique("OpenGL", LargeurFenetre, HauteurFenetre);
    lanceBoucleEvenements();
    return 0;
}

void gestionEvenement(EvenementGfx evenement)
{
    static bool fond = false;
    static bool jeuStrat = false;
    static int niveau = 0;
    static bool enJeu = false;

    switch(evenement)
    {
    case Initialisation:
        demandeTemporisation(20);
        srand(time(NULL));
        break;

    case Affichage:
        if (!enJeu)
        {
            if (fond)
            {
                effaceFenetre(255, 255, 0); // Fond jaune
            }
            else
            {
                effaceFenetre(0, 0, 0); // Fond noir
            }

            if (jeuStrat)
            {
                bouttonsDesNiveaux();
            }
            else
            {
                lancerJeu();
            }
        }
        else
        {
            afficherPageJeu(niveau);
        }
        break;

    case Clavier:
        printf("%c : ASCII %d\n", caractereClavier(), caractereClavier());
        switch (caractereClavier())
        {
        case 'Q':
        case 'q':
            termineBoucleEvenements();
            break;

        case 'F':
        case 'f':
            modePleinEcran();
            break;

        case 'L':
        case 'l':
            demandeTemporisation(100);
            break;

        case 'S':
        case 's':
            demandeTemporisation(-1);
            break;
        }
        break;

    case BoutonSouris:
        switch (etatBoutonSouris())
        {
        case GaucheAppuye:
            if (!enJeu) {
                commence(abscisseSouris(), ordonneeSouris(), &fond, &jeuStrat, &niveau, &enJeu);
                printf("Bouton gauche appuyé en : (%d, %d)\n", abscisseSouris(), ordonneeSouris());
            } else {
                if (abscisseSouris() >= 10 && abscisseSouris() <= 110 &&
                    ordonneeSouris() >= 10 && ordonneeSouris() <= 40) {
                    // Clic sur le bouton de retour
                    enJeu = false; // Retour à la page d'accueil
                }
            }
            rafraichisFenetre();
            break;
        }
        break;

    case Souris:
        break;

    case Inactivite:
        break;

    case Redimensionnement:
        rafraichisFenetre();
        break;
    }
}

void lancerJeu()
{
    int largeur = largeurFenetre();
    int hauteur = hauteurFenetre();

    float xHG = largeur / 3.0f;
    float yHG = hauteur * 2 / 3.0f;
    float xBD = largeur * 2 / 3.0f;
    float yBG = hauteur / 3.0f;

    couleurCourante(250, 100, 0);
    rectangle(xHG, yHG, xBD, yBG);

    couleurCourante(100, 150, 50);
    char ch[100] = "Commencer le jeu";
    int taillePolice = 20;

    float taille = tailleChaine(ch, taillePolice);
    epaisseurDeTrait(2);
    afficheChaine(ch, taillePolice, (largeur - taille) / 2.0f, (hauteur - taillePolice) / 2.0f);
}

void bouttonsDesNiveaux()
{
    int largeur = largeurFenetre();
    int hauteur = hauteurFenetre();

    float hauteurBoutton = hauteur / 5.0f;

    for (int i = 1; i < 4; ++i)
    {
        float xHG = largeur / 3.0f;
        float yHG = hauteur - i * hauteurBoutton;
        float xBD = largeur * 2 / 3.0f;
        float yBG = hauteur - (i + 1) * hauteurBoutton;

        couleurCourante(0, 100, 50);
        rectangle(xHG, yHG, xBD, yBG);

        couleurCourante(0, 0, 0);
        char ch[100];
        sprintf(ch, "Niveau %d", i);
        int taillePolice = 20;
        float taille = tailleChaine(ch, taillePolice);
        epaisseurDeTrait(2);
        afficheChaine(ch, taillePolice, (xHG + xBD - taille) / 2.0f, (yHG + yBG - taillePolice) / 2.0f);

        epaisseurDeTrait(5);
        if (i < 4)
        {
            ligne(xHG, yBG, xBD, yBG);
        }

    }
}

void commence(int x, int y, bool *fond, bool *jeuStrat, int *niveau, bool *enJeu)
{
    int largeur = largeurFenetre();
    int hauteur = hauteurFenetre();
    int hauteurBoutton = hauteurFenetre() / 5.0f;

    float xHG = largeur / 3.0f;
    float yHG = hauteur * 2 / 3.0f;
    float xBD = largeur * 2 / 3.0f;
    float yBG = hauteur / 3.0f;

    // Inverser y pour le repère GfxLib (origine en bas à gauche)
    y = hauteur - y;

     printf("Coordonnées du clic après inversion y : (%d, %d)\n", x, y);

    if (!*jeuStrat)
    {
        if (x >= xHG && x <= xBD && y >= yBG && y <= yHG)
        {
            printf("Clic dans le rectangle !\n");
            *fond = !*fond;
            *jeuStrat = true;
        }
    }
    else
    {
        if (x >= xHG && x <= xBD)
        {
            if (y <= (hauteur - hauteurBoutton) && y >= (hauteur - hauteurBoutton * 2))
            {
                printf("Clic sur le Niveau 3 !!\n");
                *niveau = 3;
            }
            if (y <= (hauteur - hauteurBoutton * 2) && y >= (hauteur - hauteurBoutton * 3))
            {
                printf("Clic sur le Niveau 2 !!\n");
                *niveau = 2;
            }
            if (y <= (hauteur - hauteurBoutton * 3) && y >= (hauteur - hauteurBoutton * 4))
            {
                printf("Clic sur le Niveau 1 !!\n");
                *niveau = 1;
            }

            if (*niveau > 0) {
                int largeurGrille, hauteurGrille, mines;
                switch (*niveau)
                {
                case 1:
                    largeurGrille = 15;
                    hauteurGrille = 10;
                    mines = 18;
                    break;
                case 2:
                    largeurGrille = 20;
                    hauteurGrille = 15;
                    mines = 45;
                    break;
                case 3:
                    largeurGrille = 25;
                    hauteurGrille = 20;
                    mines = 100;
                    break;
                }
                initialiserGrille(largeurGrille, hauteurGrille, mines);
                *enJeu = true;
            }
        }
    }
}

void initialiserGrille(int largeur, int hauteur, int mines)
{
    grille.largeur = largeur;
    grille.hauteur = hauteur;
    grille.mines = mines;
    grille.grille = (int **)malloc(hauteur * sizeof(int *));
    for (int i = 0; i < hauteur; ++i)
    {
        grille.grille[i] = (int *)malloc(largeur * sizeof(int));
        for (int j = 0; j < largeur; ++j)
        {
            grille.grille[i][j] = 0;
        }
    }

    int minesPlacees = 0;
    while (minesPlacees < mines)
    {
        int x = rand() % largeur;
        int y = rand() % hauteur;
        if (grille.grille[y][x] == 0)
        {
            grille.grille[y][x] = 1;
            minesPlacees++;
        }
    }
}

void afficherPageJeu(int niveau)
{
    int largeur = largeurFenetre();
    int hauteur = hauteurFenetre();

    effaceFenetre(255, 255, 255); // Fond blanc

    // Affichage du niveau
    couleurCourante(0, 0, 0);
    char ch[100];
    sprintf(ch, "Niveau %d", niveau);
    int taillePolice = 20;
    afficheChaine(ch, taillePolice, 10, hauteur - 30);

    // Bouton de retour
    afficherBoutonRetour();

    // Affichage de la grille de démineur
    afficherGrilleDemineur(grille.largeur, grille.hauteur);
}

void afficherBoutonRetour()
{
    int largeur = largeurFenetre();
    int hauteur = hauteurFenetre();

    couleurCourante(100, 100, 100); // Couleur du bouton de retour
    rectangle(10, 10, 110, 40); // Rectangle du bouton de retour

    couleurCourante(255, 255, 255); // Couleur du texte
    char ch[100] = "Retour";
    int taillePolice = 14;                                                                                                               
    float taille = tailleChaine(ch, taillePolice);
    afficheChaine(ch, taillePolice, (100 - taille) / 2.0f, 25 - taillePolice / 2.0f); // Texte du bouton de retour
}

void afficherGrilleDemineur(int largeur, int hauteur)
{
    int largeurFen = largeurFenetre();
    int hauteurFen = hauteurFenetre();

    int tailleCase = (largeurFen / largeur < hauteurFen / hauteur) ? largeurFen / largeur : hauteurFen / hauteur;

    int margeX = (largeurFen - largeur * tailleCase) / 2;
    int margeY = (hauteurFen - hauteur * tailleCase) / 2;

    couleurCourante(0, 0, 255);
    epaisseurDeTrait(2);

    for (int i = 0; i <= largeur; ++i)
    {
        ligne(margeX + i * tailleCase, margeY, margeX + i * tailleCase, margeY + hauteur * tailleCase);
    }

    for (int j = 0; j <= hauteur; ++j)
    {
        ligne(margeX, margeY + j * tailleCase, margeX + largeur * tailleCase, margeY + j * tailleCase);
    }
}

   
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
