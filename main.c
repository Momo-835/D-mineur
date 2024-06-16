#include <stdlib.h>

#include <stdio.h>

#include <time.h>

#include "GfxLib.h"



#define LargeurFenetre 800

#define HauteurFenetre 600

#define TailleCase 20



typedef struct {

    int largeur;

    int hauteur;

    int mines;

    int **grille;

    int **valeurs;

    bool **devoilees;

    bool **drapeaux;  // Ajouté pour gérer les drapeaux

    int drapeauxRestants;

    bool premiereAction;

    bool partiePerdue;

} GrilleDemineur;



GrilleDemineur grille;

time_t debutPartie;



void gestionEvenement(EvenementGfx evenement);

void lancerJeu();

void bouttonsDesNiveaux();

void commence(int x, int y, bool *fond, bool *jeuStrat, int *niveau, bool *enJeu);

void afficherGrille(int niveau);

void afficherPageJeu(int niveau);

void initialiserGrille(int largeur, int hauteur, int mines);

void initialiserGrilleApresPremierClic(int x, int y);

void afficherGrilleDemineur(int largeur, int hauteur);

void afficherBoutonRetour();

void afficherBoutonRecommencer();

void afficherTempsEcoule();

void afficherMinesRestantes();

void devoilerCase(int x, int y);

void devoilerCasesAdjacentes(int x, int y);

void afficherMessageDefaite();

int compterMinesAdjacentes(int x, int y);

void basculerDrapeau(int x, int y);  // Ajouté pour gérer les drapeaux



int main(int argc, char **argv) {

    initialiseGfx(argc, argv);

    prepareFenetreGraphique("OpenGL", LargeurFenetre, HauteurFenetre);

    lanceBoucleEvenements();

    return 0;

}



void gestionEvenement(EvenementGfx evenement) {

    static bool fond = false;

    static bool jeuStrat = false;

    static int niveau = 0;

    static bool enJeu = false;



    switch (evenement) {

        case Initialisation:

            demandeTemporisation(1000);

            srand(time(NULL));

            break;



        case Temporisation:

            rafraichisFenetre();

            break;



        case Affichage:

            if (!enJeu) {

                if (fond) {

                    effaceFenetre(255, 255, 0);

                } else {

                    effaceFenetre(0, 0, 0);

                }



                if (jeuStrat) {

                    bouttonsDesNiveaux();

                } else {

                    lancerJeu();

                }

            } else {

                afficherPageJeu(niveau);

                afficherTempsEcoule();

                afficherMinesRestantes();

                if (grille.partiePerdue) {

                    afficherMessageDefaite();

                }

            }

            break;



        case Clavier:

            switch (caractereClavier()) {

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

            switch (etatBoutonSouris()) {

                case GaucheAppuye:

                    if (!enJeu) {

                        commence(abscisseSouris(), ordonneeSouris(), &fond, &jeuStrat, &niveau, &enJeu);

                    } else {

                        if (abscisseSouris() >= 10 && abscisseSouris() <= 110 &&

                            ordonneeSouris() >= 10 && ordonneeSouris() <= 40) {

                            enJeu = false;

                        } else if (abscisseSouris() >= 10 && abscisseSouris() <= 150 &&

                                   ordonneeSouris() >= 50 && ordonneeSouris() <= 80) {

                            initialiserGrille(grille.largeur, grille.hauteur, grille.mines);

                            debutPartie = time(NULL);

                            grille.premiereAction = true;

                        } else {

                            int x = (abscisseSouris() - (LargeurFenetre - grille.largeur * TailleCase) / 2) / TailleCase;

                            int y = (ordonneeSouris() - (HauteurFenetre - grille.hauteur * TailleCase) / 2) / TailleCase;

                            if (x >= 0 && x < grille.largeur && y >= 0 && y < grille.hauteur) {

                                if (grille.premiereAction) {

                                    initialiserGrilleApresPremierClic(x, y);

                                    grille.premiereAction = false;

                                }

                                devoilerCase(x, y);

                            }

                        }

                    }

                    rafraichisFenetre();

                    break;



                case DroiteAppuye:

                    if (enJeu) {

                        int x = (abscisseSouris() - (LargeurFenetre - grille.largeur * TailleCase) / 2) / TailleCase;

                        int y = (ordonneeSouris() - (HauteurFenetre - grille.hauteur * TailleCase) / 2) / TailleCase;

                        if (x >= 0 && x < grille.largeur && y >= 0 && y < grille.hauteur) {

                            basculerDrapeau(x, y);

                        }

                    }

                    rafraichisFenetre();

                    break;



                default:

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



        default:

            break;

    }

}



void lancerJeu() {

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



void bouttonsDesNiveaux() {

    int largeur = largeurFenetre();

    int hauteur = hauteurFenetre();



    float hauteurBoutton = hauteur / 5.0f;



    for (int i = 1; i < 4; ++i) {

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

        if (i < 4) {

            ligne(xHG, yBG, xBD, yBG);

        }

    }

}



void commence(int x, int y, bool *fond, bool *jeuStrat, int *niveau, bool *enJeu) {

    int largeur = largeurFenetre();

    int hauteur = hauteurFenetre();

    int hauteurBoutton = hauteurFenetre() / 5.0f;



    float xHG = largeur / 3.0f;

    float yHG = hauteur * 2 / 3.0f;

    float xBD = largeur * 2 / 3.0f;

    float yBG = hauteur / 3.0f;



    y = hauteur - y;



    if (!*jeuStrat) {

        if (x >= xHG && x <= xBD && y >= yBG && y <= yHG) {

            *fond = !*fond;

            *jeuStrat = true;

        }

    } else {

        if (x >= xHG && x <= xBD) {

            for (int i = 1; i < 4; ++i) {

                int BouttonHG = hauteur - i * hauteurBoutton;

                int BouttonBG = hauteur - (i + 1) * hauteurBoutton;



                if (y >= BouttonBG && y <= BouttonHG) {

                    *enJeu = true;

                    *niveau = i;

                    switch (i) {

                        case 1:

                            initialiserGrille(10, 10, 10);

                            break;

                        case 2:

                            initialiserGrille(16, 16, 40);

                            break;

                        case 3:

                            initialiserGrille(30, 16, 99);

                            break;

                    }

                    debutPartie = time(NULL);

                }

            }

        }

    }

}



void initialiserGrille(int largeur, int hauteur, int mines) {

    grille.largeur = largeur;

    grille.hauteur = hauteur;

    grille.mines = mines;

    grille.drapeauxRestants = mines;

    grille.premiereAction = true;

    grille.partiePerdue = false;



    grille.grille = (int **)malloc(largeur * sizeof(int *));

    grille.valeurs = (int **)malloc(largeur * sizeof(int *));

    grille.devoilees = (bool **)malloc(largeur * sizeof(bool *));

    grille.drapeaux = (bool **)malloc(largeur * sizeof(bool *));  // Allocation pour les drapeaux



    for (int i = 0; i < largeur; ++i) {

        grille.grille[i] = (int *)malloc(hauteur * sizeof(int));

        grille.valeurs[i] = (int *)malloc(hauteur * sizeof(int));

        grille.devoilees[i] = (bool *)malloc(hauteur * sizeof(bool));

        grille.drapeaux[i] = (bool *)malloc(hauteur * sizeof(bool));  // Allocation pour les drapeaux



        for (int j = 0; j < hauteur; ++j) {

            grille.grille[i][j] = 0;

            grille.valeurs[i][j] = 0;

            grille.devoilees[i][j] = false;

            grille.drapeaux[i][j] = false;  // Initialisation des drapeaux

        }

    }

}



void initialiserGrilleApresPremierClic(int x, int y) {

    int minesPlacees = 0;



    while (minesPlacees < grille.mines) {

        int i = rand() % grille.largeur;

        int j = rand() % grille.hauteur;



        if (grille.grille[i][j] == 0 && (i != x || j != y) && !(i >= x - 1 && i <= x + 1 && j >= y - 1 && j <= y + 1)) {

            grille.grille[i][j] = 1;

            ++minesPlacees;

        }

    }



    for (int i = 0; i < grille.largeur; ++i) {

        for (int j = 0; j < grille.hauteur; ++j) {

            if (grille.grille[i][j] == 0) {

                grille.valeurs[i][j] = compterMinesAdjacentes(i, j);

            } else {

                grille.valeurs[i][j] = -1;

            }

        }

    }

}



void afficherPageJeu(int niveau) {

    int largeur = largeurFenetre();

    int hauteur = hauteurFenetre();

    int x0 = (largeur - grille.largeur * TailleCase) / 2;

    int y0 = (hauteur - grille.hauteur * TailleCase) / 2;



    effaceFenetre(255, 255, 255);



    for (int i = 0; i < grille.largeur; ++i) {

        for (int j = 0; j < grille.hauteur; ++j) {

            int x = x0 + i * TailleCase;

            int y = y0 + j * TailleCase;



            if (grille.devoilees[i][j]) {

                couleurCourante(200, 200, 200);

                rectangle(x, y, x + TailleCase, y + TailleCase);



                if (grille.valeurs[i][j] > 0) {

                    couleurCourante(0, 0, 0);

                    char ch[2];

                    sprintf(ch, "%d", grille.valeurs[i][j]);

                    afficheChaine(ch, 15, x + 5, y + 5);

                }

            } else {

                couleurCourante(150, 150, 150);

                rectangle(x, y, x + TailleCase, y + TailleCase);



                if (grille.drapeaux[i][j]) {

                    couleurCourante(0, 0, 255);  // Couleur des drapeaux

                    afficheChaine("F", 15, x + 5, y + 5);  // Affichage des drapeaux

                }

            }

        }

    }



    afficherBoutonRetour();

    afficherBoutonRecommencer();

}



void devoilerCase(int x, int y) {

    if (x < 0 || x >= grille.largeur || y < 0 || y >= grille.hauteur || grille.devoilees[x][y] || grille.drapeaux[x][y]) {

        return;

    }



    grille.devoilees[x][y] = true;



    if (grille.valeurs[x][y] == -1) {

        grille.partiePerdue = true;

        for (int i = 0; i < grille.largeur; ++i) {

            for (int j = 0; j < grille.hauteur; ++j) {

                if (grille.grille[i][j] == 1) {

                    grille.devoilees[i][j] = true;

                }

            }

        }

    } else if (grille.valeurs[x][y] == 0) {

        devoilerCasesAdjacentes(x, y);

    }

}



void devoilerCasesAdjacentes(int x, int y) {

    for (int i = -1; i <= 1; ++i) {

        for (int j = -1; ++j <= 1; ++j) {

            if (i != 0 || j != 0) {

                devoilerCase(x + i, y + j);

            }

        }

    }

}



int compterMinesAdjacentes(int x, int y) {

    int compteur = 0;



    for (int i = -1; i <= 1; ++i) {

        for (int j = -1; ++j <= 1; ++j) {

            int nx = x + i;

            int ny = y + j;



            if (nx >= 0 && nx < grille.largeur && ny >= 0 && ny < grille.hauteur && grille.grille[nx][ny] == 1) {

                ++compteur;

            }

        }

    }



    return compteur;

}



void basculerDrapeau(int x, int y) {

    if (grille.devoilees[x][y]) {

        return;

    }



    if (grille.drapeaux[x][y]) {

        grille.drapeaux[x][y] = false;

        ++grille.drapeauxRestants;

    } else if (grille.drapeauxRestants > 0) {

        grille.drapeaux[x][y] = true;

        --grille.drapeauxRestants;

    }

}



void afficherBoutonRetour() {

    couleurCourante(100, 100, 250);

    rectangle(10, 10, 110, 40);

    couleurCourante(0, 0, 0);

    afficheChaine("Retour", 15, 20, 20);

}



void afficherBoutonRecommencer() {

    couleurCourante(100, 250, 100);

    rectangle(10, 50, 150, 80);

    couleurCourante(0, 0, 0);

    afficheChaine("Recommencer", 15, 20, 60);

}



void afficherTempsEcoule() {

    time_t tempsActuel = time(NULL);

    double secondes = difftime(tempsActuel, debutPartie);



    int minutes = secondes / 60;

    secondes = (int)secondes % 60;



    char temps[50];

    sprintf(temps, "Temps : %02d:%02d", minutes, (int)secondes);



    couleurCourante(0, 0, 0);

    afficheChaine(temps, 20, LargeurFenetre - 200, HauteurFenetre - 30);

}



void afficherMinesRestantes() {

    char mines[50];

    sprintf(mines, "Drapeaux restants : %d", grille.drapeauxRestants);



    couleurCourante(0, 0, 0);

    afficheChaine(mines, 20, LargeurFenetre - 200, HauteurFenetre - 60);

}



void afficherMessageDefaite() {

    couleurCourante(255, 0, 0);

    afficheChaine("PERDU!", 50, LargeurFenetre / 2 - 100, HauteurFenetre / 2 - 25);

}
