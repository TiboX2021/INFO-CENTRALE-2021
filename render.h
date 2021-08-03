#ifndef RENDER_H
#define RENDER_H

#include <chrono>  // Calcul du temps d'exécution
#include <string>  // nom fichier
#include <memory>  // unique_ptr

#include "render_base.h"
#include "bitmap_image.hpp"

class Render
{
    /**
     * Pour render une scène : contient les dimensions de l'image, les objets et sources
     *
     */
public:
    Render();
    ~Render();  // Destructeur : vider le vector de pointeurs


    void set_width(const int width);
    void set_height(const int height);
    void set_dimensions(const int height, const int width);

    void set_fond(const Array3d fond);

    void set_height_ratio(double m);
    void set_width_ratio(double m);

    void add_source(const Source source);  // Ne pas passer par référence ! Sinon c'est détruit
    void add_objet(Objet* objet);  // Créer un pointeur (pas const car il doit ensuite être détruit)
    void add_sources(const vector<Source> sources);  // Idem, pas par référence, plus safe

    void set_omega(const Vector3d omega);
    void set_rmax(const int rmax);

    void render_image(const string &filename);  // sauvegarde l'image

private:

    // Caractéristiques de la scène
    vector<Objet *> m_objets;
    vector<Source> m_sources;
    Array3d m_fond;  // Couleur du fond
    Vector3d m_omega;
    int m_rmax;

    // Dimensions de l'image
    int m_height;
    int m_width;
    double m_ratio;  // mètres par pixel, défini à partir de width ou height

    // Fonctions internes
    Vector3d rayon_ecran(const int i, const int j);
};

#endif // RENDER_H
