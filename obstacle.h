#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Obstacle
{
public:
    Obstacle(QVector3D position, float rayon);
    void affiche(QOpenGLShaderProgram *sp, int vc);
    QVector3D position;
    float rayon;
    QMatrix4x4 modelMatrix;
    int vertexCount;
};

#endif // OBSTACLE_H
