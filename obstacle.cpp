#include "obstacle.h"

Obstacle::Obstacle(QVector3D position, float rayon){
    this->position = position;
    this->rayon = rayon;
}

void Obstacle::affiche(QOpenGLShaderProgram *sp, int vc){
    modelMatrix.setToIdentity();
    modelMatrix.translate(position);

    sp->setUniformValue("modelMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
