#include "banc.h"

Banc::Banc(int nbPoissons, int largeur, int hauteur, int profondeur){
    this->nbPoissons = nbPoissons;
    this->largeur = largeur;
    this->hauteur = hauteur;
    this->profondeur = profondeur;
    poissons = std::list<Poisson>();
    for(int i = 0; i < nbPoissons; i++) {
        QVector3D position = QVector3D(rand() % largeur - largeur/2, rand() % hauteur - hauteur/2, rand() % profondeur - profondeur/2);
        QVector3D vitesse = QVector3D(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5);
        poissons.push_back(Poisson(position, 1.0f, vitesse, 20.0f));
    }
}

void Banc::anime(float dt){
    for(Poisson &poisson : poissons) {
        std::list<Poisson> voisins = std::list<Poisson>();
        QVector3D positionMoyenne = QVector3D();
        QVector3D vitesseMoyenne = QVector3D();
        foreach (Poisson poissonVoisin, poissons) {
            if(poisson.position == poissonVoisin.position) continue;
            if(poisson.dans_voisinage(poissonVoisin.position)){
                voisins.push_back(poissonVoisin);
                positionMoyenne += poissonVoisin.position;
                vitesseMoyenne += poissonVoisin.vitesse;
            }
        }
        if(voisins.size() > 0){
            QVector3D separation = poisson.position - (positionMoyenne/voisins.size());
            QVector3D alignement = poisson.vitesse - (vitesseMoyenne/voisins.size());
            QVector3D cohesion = - poisson.position - (positionMoyenne/voisins.size());

            QVector3D vitesse = 1.5 * separation + 1.0 * alignement + 1.0 * cohesion;
            float L = 0.01;
            QVector3D vitessePonderee = (1-L) * poisson.vitesse + L * vitesse;

            if(vitessePonderee.length() > poisson.vitesseMax){
                vitessePonderee.normalize();
                vitessePonderee = vitessePonderee * poisson.vitesseMax;
            }
            poisson.vitesse = vitessePonderee;
        }

        //proximity to aquarium bound
        QVector3D aquariumSize = QVector3D(largeur/2, hauteur/2, profondeur/2);
        for(int i = 0; i < 3; i++){
            if(aquariumSize[i] - poisson.position[i] < 1.0 && poisson.position[i] > 0.0){ //+
                float L = abs(aquariumSize[i] - poisson.position[i]);
                poisson.vitesse[i] = (1-L) * poisson.vitesse[i] + L * -(poisson.vitesse[i]);
            }else if(-aquariumSize[i] - poisson.position[i] > -1.0 && poisson.position[i] < 0.0){ //-
                float L = abs(-aquariumSize[i] - poisson.position[i]);
                poisson.vitesse[i] = (1-L) * poisson.vitesse[i] + L * -(poisson.vitesse[i]);
            }

            if(poisson.position[i] >= aquariumSize[i]){
                poisson.position[i] = aquariumSize[i] - 1.0;
            }else if(poisson.position[i] <= -aquariumSize[i]){
                poisson.position[i] = -aquariumSize[i] + 1.0;
            }
        }

        poisson.anime(dt);
    }
}

void Banc::affiche(QOpenGLShaderProgram *sp){
    std::list<Poisson>::iterator i;
    i = poissons.begin();
    while(i != poissons.end()){
        i->affiche(sp);
        i++;
    }
}
