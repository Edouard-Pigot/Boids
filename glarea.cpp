// Basé sur :
// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>



GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);
    setFormat(sf);

    setEnabled(true);                   // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus);    // accepte focus
    setFocus();                         // donne le focus

    timer = new QTimer(this);
    timer->setInterval(50);           // msec
    connect (timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();
    elapsedTimer.start();

    //qDebug() << "construction";
    banc = new Banc(100, 50.0f, 30.0f, 30.0f);/*
    Obstacle o1 = Obstacle(QVector3D(0, 0, 0), 5.0f);
    Obstacle o2 = Obstacle(QVector3D(0, 0, 0), 5.0f);
    obstacles.push_back(&o1);
    obstacles.push_back(&o2);*/
}


GLArea::~GLArea()
{
    delete timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();
    tearGLObjects();
    doneCurrent();
}


void GLArea::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.5f,0.5f,1.0f,1.0f);
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();

    qDebug() << "shader aquarium";
    // shader d'aquarium
    program_aquarium = new QOpenGLShaderProgram(this);
    program_aquarium->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vsh");
    program_aquarium->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.fsh");
    if (! program_aquarium->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_aquarium->log();
    }

    qDebug() << "shader obstacle";
    // shader d'obstacle
    /*program_obstacle = new QOpenGLShaderProgram(this);
    program_obstacle->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vsh");
    program_obstacle->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.fsh");
    if (! program_obstacle->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_obstacle->log();
    }*/

    qDebug() << "shader poisson";
    // shader de poisson
    program_poisson = new QOpenGLShaderProgram(this);
    program_poisson->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/billboard.vsh");
    program_poisson->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/billboard.fsh");
    if (! program_poisson->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_poisson->log();
    }
    program_poisson->setUniformValue("texture", 0);
}


void GLArea::makeGLObjects()
{
    float x = banc->largeur/2;
    float y = banc->hauteur/2;
    float z = banc->profondeur/2;

    qDebug() << x << y << z;

    qDebug() << "création aquarium";
    // Création de l'aquarium
    GLfloat vertices_aquarium[] = {
        -x, -y, -z,
         x, -y, -z,
        -x,  y, -z,
         x,  y, -z,
        -x, -y, -z,
        -x,  y, -z,
         x, -y, -z,
         x,  y, -z,

        -x, -y,  z,
         x, -y,  z,
        -x,  y,  z,
         x,  y,  z,
        -x, -y,  z,
        -x,  y,  z,
         x, -y,  z,
         x,  y,  z,

        -x, -y, -z,
        -x, -y,  z,
        -x,  y, -z,
        -x,  y,  z,
         x, -y, -z,
         x, -y,  z,
         x,  y, -z,
         x,  y,  z
    };

    QVector<GLfloat> vertData_aquarium;
    for (int i = 0; i < 24; ++i) {
        // coordonnées sommets
        for (int j = 0; j < 3; j++)
            vertData_aquarium.append(vertices_aquarium[i*3+j]);
    }

    vbo_aquarium.create();
    vbo_aquarium.bind();
    vbo_aquarium.allocate(vertData_aquarium.constData(), vertData_aquarium.count() * int(sizeof(GLfloat)));

    qDebug() << "création obstacles";
/*
    QVector<GLfloat> vertData_obstacle;
    int couches = 40;
    int secteurs = 40;
    for(Obstacle *obstacle : obstacles){
        int cpt = 0;
        float rayon = obstacle->rayon;
        for(int i = 0; i <= couches; i++)
        {
            float lat0 = static_cast<float>(M_PI) * (float)(-0.5f + (i - 1) / (float)couches);
            float z0  = rayon*sin(lat0);
            float zr0 =  rayon*cos(lat0);

            float lat1 = static_cast<float>(M_PI) * (float)(-0.5f + i / (float)couches);
            float z1 = rayon*sin(lat1);
            float zr1 = rayon*cos(lat1);

            for(int j = 0; j <= secteurs; j++)
            {
                float lng = 2 * static_cast<float>(M_PI) * (float)(j - 1) / (float)secteurs;
                float x = cos(lng);
                float y = sin(lng);

                vertData_obstacle.append(x * zr0 + obstacle->position[0]); //X
                vertData_obstacle.append(y * zr0 + obstacle->position[1]); //Y
                vertData_obstacle.append(z0 + obstacle->position[2]);      //Z

                vertData_obstacle.append(x * zr1 + obstacle->position[0]); //X
                vertData_obstacle.append(y * zr1 + obstacle->position[1]); //Y
                vertData_obstacle.append(z1 + obstacle->position[2]);      //Z
                cpt += 2;
            }
        }
        obstacle->vertexCount = cpt;
    }

    qDebug() << vertData_obstacle.size()/3;

    vbo_obstacle.create();
    vbo_obstacle.bind();
    vbo_obstacle.allocate(vertData_obstacle.constData(), vertData_obstacle.count() * int(sizeof(GLfloat)));
*/

    qDebug() << "création poissons";
    // Création d'un poisson
    GLfloat vertices_poisson[] = {
        -0.5f, 1.75f, 0.0f,
        -0.5f,-1.75f, 0.0f,
        0.5f, 1.75f, 0.0f,
        -0.5f,-1.75f, 0.0f,
        0.5f,-1.75f, 0.0f,
        0.5f, 1.75f, 0.0f
    };

    GLfloat texCoords_poisson[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    QVector<GLfloat> vertData_poisson;
    for (int i = 0; i < 6; ++i) {
        // coordonnées sommets
        for (int j = 0; j < 3; j++)
            vertData_poisson.append(vertices_poisson[i*3+j]);
        // coordonnées texture
        for (int j = 0; j < 2; j++)
            vertData_poisson.append(texCoords_poisson[i*2+j]);
    }

    vbo_poisson.create();
    vbo_poisson.bind();
    vbo_poisson.allocate(vertData_poisson.constData(), vertData_poisson.count() * int(sizeof(GLfloat)));

    // Création de textures
    QImage image_fish(":/textures/fish.png");
    if (image_fish.isNull())
        qDebug() << "load image fish.png failed";
    textures[0] = new QOpenGLTexture(image_fish);
}


void GLArea::tearGLObjects()
{
    vbo_aquarium.destroy();
    vbo_poisson.destroy();
    //vbo_obstacle.destroy();
    for (int i = 0; i < 1; i++)
        delete textures[i];
}


void GLArea::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    windowRatio = float(w) / h;
}


void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrice de projection
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, windowRatio, 1.0f, 1000.0f);

    // Matrice de vue (caméra)
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(xPos, yPos, zPos);
    viewMatrix.rotate(xRot, 1, 0, 0);
    viewMatrix.rotate(yRot, 0, 1, 0);
    viewMatrix.rotate(zRot, 0, 0, 1);

    qDebug() << "affichage aquarium";
    // Affichage de l'aquarium
    vbo_aquarium.bind();
    program_aquarium->bind();

    QMatrix4x4 modelMatrixAquarium;
    modelMatrixAquarium.translate(0.0f, 0.0f, 0.0f);
    program_aquarium->setUniformValue("projectionMatrix", projectionMatrix);
    program_aquarium->setUniformValue("viewMatrix", viewMatrix);
    program_aquarium->setUniformValue("modelMatrix", modelMatrixAquarium);

    program_aquarium->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 3 * sizeof(GLfloat));
    program_aquarium->enableAttributeArray("in_position");

    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, 24);

    program_aquarium->disableAttributeArray("in_position");
    program_aquarium->release();


    qDebug() << "affichage obstacle";
    // Affichage d'un obstacle
    /*vbo_obstacle.bind();
    program_obstacle->bind();

    QMatrix4x4 modelMatrixObstacle;
    modelMatrixObstacle.translate(0.0f, 0.0f, 0.0f);
    program_obstacle->setUniformValue("projectionMatrix", projectionMatrix);
    program_obstacle->setUniformValue("viewMatrix", viewMatrix);

    program_obstacle->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 3 * sizeof(GLfloat));
    program_obstacle->enableAttributeArray("in_position");

    for(Obstacle *o : obstacles){
        o->affiche(program_obstacle, 10);
        o->affiche(program_obstacle, 10);
    }

    program_obstacle->disableAttributeArray("in_position");
    program_obstacle->release();

*/


    qDebug() << "affichage poisson";
    // Affichage d'un poisson
    vbo_poisson.bind();
    program_poisson->bind();

    QMatrix4x4 modelMatrixParticule;
    modelMatrixParticule.translate(0.0f, 1.0f, 0.0f);
    program_poisson->setUniformValue("projectionMatrix", projectionMatrix);
    program_poisson->setUniformValue("viewMatrix", viewMatrix);

    program_poisson->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program_poisson->setAttributeBuffer("in_uv", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    program_poisson->enableAttributeArray("in_position");
    program_poisson->enableAttributeArray("in_uv");

    textures[0]->bind();

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    banc->anime(dt);
    banc->affiche(program_poisson);
    textures[0]->release();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    program_poisson->disableAttributeArray("in_position");
    program_poisson->disableAttributeArray("in_uv");
    program_poisson->release();
}


void GLArea::keyPressEvent(QKeyEvent *ev)
{
    float da = 1.0f;

    switch(ev->key()) {
        case Qt::Key_A :
            xRot -= da;
            update();
            break;

        case Qt::Key_Q :
            xRot += da;
            update();
            break;

        case Qt::Key_Z :
            yRot -= da;
            update();
            break;

        case Qt::Key_S :
            yRot += da;
            update();
            break;

        case Qt::Key_E :
            zRot -= da;
            update();
            break;

        case Qt::Key_D :
            zRot += da;
            update();
            break;
    }
}


void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();
}


void GLArea::mousePressEvent(QMouseEvent *ev)
{
    lastPos = ev->pos();
}


void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}


void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    int dx = ev->x() - lastPos.x();
    int dy = ev->y() - lastPos.y();

    if (ev->buttons() & Qt::LeftButton) {
        xRot += dy;
        yRot += dx;
        update();
    } else if (ev->buttons() & Qt::RightButton) {
        xPos += dx/10.0f;
        yPos -= dy/10.0f;
        update();
    } else if (ev->buttons() & Qt::MidButton) {
        xPos += dx/10.0f;
        zPos += dy;
        update();
    }

    lastPos = ev->pos();
}


void GLArea::onTimeout()
{
    static qint64 old_chrono = elapsedTimer.elapsed(); // static : initialisation la première fois et conserve la dernière valeur
    qint64 chrono = elapsedTimer.elapsed();
    dt = (chrono - old_chrono) / 1000.0f;
    old_chrono = chrono;



    update();
}
