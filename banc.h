#ifndef BANC_H
#define BANC_H

#include "poisson.h"
#include <QVector>
#include <list>

class Banc
{
public:
    Banc(int nbPoissons, int largeur, int hauteur, int profondeur);
    int nbPoissons;
    std::list<Poisson> poissons;
    int largeur, hauteur, profondeur;
    void anime(float dt);
    void affiche(QOpenGLShaderProgram *sp);
};

#endif // BANC_H
