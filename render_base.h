#ifndef RENDER_BASE_H
#define RENDER_BASE_H

#include <Eigen/Dense>
#include <math.h>  // sqrt pour norme, cos et sin? M_PI : pi
#include <vector>

#include <iostream>  // Debug

#define THRESHOLD 0.00001  // Correction des incertitudes float
#define ANGLE_LIMITE 0.0000000001  // Eviter des t intersection plan qui -> infini

#define BLANC Array3d(1,1,1)
#define NOIR Array3d(0,0,0)
#define NULL_VECT Vector3d(0,0,0)

using namespace std;
using Eigen::Vector3d;
using Eigen::Array3d;


// Exception A FAIRE


// Conteneurs de données


struct Source
{
    /**
     * Source : point source m_S, couleur m_col
     */
public:
    Source(const Vector3d &S, const Array3d &col)
        : m_S(S),
          m_col(col)
    {}

    Vector3d m_S;
    Array3d m_col;
};


struct Rayon
{
    /**
     * Rayon lumineux : un point R et un vecteur unitaire direction m_u
     */
public:
    Rayon(const Vector3d &R, const Vector3d &u)
        : m_R(R),
          m_u(u)
    {}

    Vector3d m_R;
    Vector3d m_u;
};


struct Intersection
{
    /**
     * Intersection d'un rayon avec un objet
     * Point d'intersection m_P, indice de l'objet m_i
     */
public:
    Intersection(const Vector3d &P, const double t)
        : m_P(P),
          m_t(t)
    {}
    bool is_valid() const {return m_t != 0;}

    static Intersection invalid(){return Intersection(NULL_VECT, 0.);}

    Vector3d m_P;
    double m_t;
};


struct Point
{
    /**
     * Point m_P et l'indice m_i de l'objet auquel il appartient
     */
public:
    Point(const Vector3d &P, const int i)
        : m_P(P),
          m_i(i)
    {}
    bool is_valid() const {return m_i != -1;}

    static Point invalid(){return Point(NULL_VECT, -1);}

    Vector3d m_P;
    int m_i;
};


struct Objet
{
    /**
     * Classe virtuelle des objets
     */
public:
    Objet(const Array3d &kd, const Array3d &kr)
        : m_kd(kd),
          m_kr(kr)
    {}
    virtual ~Objet(){}  // J'ai précisément défini le constructeur, donc le destructeur doit l'être aussi défini
    virtual Intersection intersection(const Rayon &ray) const = 0;  // Const : ne modifie pas l'objet

    // Fonction au_dessus, voir autres : si c'est visible en intra
    virtual bool visible(const Vector3d &P, const Source &src) const = 0;

    virtual Vector3d normal(const Vector3d &P) const = 0;  // retourne le vecteur normal

    // Comportement avec la lumière
    Array3d m_kd;
    Array3d m_kr;
};


struct Sphere : public Objet
{
    /**
     * Sphère : centre m_C, rayon m_r
     */
public:
    Sphere(const Vector3d &C, const double r, const Array3d &kd = BLANC, const Array3d &kr = NOIR)
        : Objet(kd, kr),  // Constructeur de la base class
          m_C(C),
          m_r(r)
    {}
    virtual Intersection intersection(const Rayon &ray) const;  // Redéfinition de la méthode virtuelle héritée
    virtual bool visible(const Vector3d &P, const Source &src) const;
    virtual Vector3d normal(const Vector3d &P) const;
    Vector3d m_C;
    double m_r;
};


struct Plan : public Objet
{
    /**
     * Plan : un point m_P et un vecteur normal m_N
     */
public:
    Plan(const Vector3d &P, const Vector3d &N, const Array3d &kd = BLANC, const Array3d &kr = NOIR)
        : Objet(kd, kr),  // Constructeur de la base class
          m_P(P),
          m_N(N)
    {}
    virtual Intersection intersection(const Rayon &ray) const;  // Redéfinition de la méthode virtuelle héritée
    virtual bool visible(const Vector3d &P, const Source &src) const;
    virtual Vector3d normal(const Vector3d &P) const {return m_N;}  // Plan : toujours le même vecteur normal
    Vector3d m_P;
    Vector3d m_N;
};




// Fonctions utiles
double clip(const double a, const double min = 0., const double max = 1.);

Array3d clipArray3d(Array3d &array, const double min = 0., const double max = 1.);  // modifie l'array

char dt255(const double d);

Vector3d vec(const Vector3d &A, const Vector3d &B);

double ps(const Vector3d &vec1, const Vector3d &vec2);  // Relativement inutile, c'est .dot()

double norme(const Vector3d &vec);

Vector3d unitaire(const Vector3d &vec);

Vector3d pt(const Rayon &ray, const double t);

Vector3d direction(const Vector3d &A, const Vector3d &B);

Rayon ra(const Vector3d &A, const Vector3d &B);

Sphere sp(const Vector3d &A, const Vector3d &B);


//Fonctions de calcul

int get_valid_intersection(const vector<Intersection> &intersections);

bool visible(const vector<Objet*> &objets, const int i, const Vector3d &P, const Source &src);

Array3d couleur_diffusee(const Source &src, const Vector3d &P, const Objet* objet);

Rayon rayon_reflechi(const Objet* objet, const Rayon &ray, const Vector3d &P);

Point interception(const vector<Objet*> objets, const Rayon &ray);

Array3d couleur_diffusion(const vector<Objet*> &objets, const vector<Source> &sources, const Vector3d &P, const int i);

vector<Point> reflections(const vector<Objet*> &objets, Rayon &ray, const int rmax);  // Ray va être modifié

// Idem : ici, ray ne peut pas être constant car il va être modifié
Array3d couleur_percue(const vector<Objet*> &objets, const vector<Source> &sources, Rayon &ray, const Array3d &fond, const int rmax);


// Complémentaire : générer des objets et sources particulières
vector<Source> polygonal_source(const Vector3d &P, const Vector3d &N, const Array3d &colour, const double r, const int n);

vector<Source> source_circul_unif();  // A faire

#endif // RENDER_BASE_H
