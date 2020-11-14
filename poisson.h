#ifndef POISSON_H
#define POISSON_H

#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QtMath>

class Poisson
{
public:
    Poisson(QVector3D position, float taille, QVector3D vitesse, float distance);
    void anime(float dt);
    bool dans_voisinage(QVector3D p);
    void affiche(QOpenGLShaderProgram *sp);
    QVector3D position;
    float taille;
    QVector3D vitesse;
    float distance;
    QMatrix4x4 modelMatrix;
    float vitesseMax;
};

#endif // POISSON_H
