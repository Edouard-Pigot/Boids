#include "poisson.h"
#include <QDebug>

Poisson::Poisson(QVector3D position, float taille, QVector3D vitesse, float distance){
    this->position = position;
    this->taille = taille;
    this->vitesse = vitesse;
    this->distance = distance;
    this->vitesseMax = 5;
}

void Poisson::anime(float dt){
    this->position += this->vitesse*(dt);
}

bool Poisson::dans_voisinage(QVector3D p){
    return std::abs(this->position.distanceToPoint(p)) <= distance;
}

void Poisson::affiche(QOpenGLShaderProgram *sp){
    modelMatrix.setToIdentity();
    modelMatrix.translate(position);

    if(vitesse.x() != 0.0f || vitesse.z() != 0.0f){
       int signe = 1;
       if(vitesse.z() > 0) signe = -1;
       float angleY = signe * std::acos(vitesse.x()/std::sqrt(std::pow(vitesse.x(), 2.0f) + std::pow(vitesse.z(), 2.0f))) * 180.0f / static_cast<float>(M_PI) + 180.0f;
       modelMatrix.rotate(angleY,0,1,0);
   }
   if(vitesse.length() != 0.0f){
       float angleZ = std::acos(vitesse.y()/vitesse.length())*180.0f / static_cast<float>(M_PI);
       modelMatrix.rotate(angleZ,0,0,1);
   }

    sp->setUniformValue("modelMatrix", modelMatrix);
    sp->setUniformValue("particleSize", 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
