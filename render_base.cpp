#include "render_base.h"

// Fonctions utiles

double clip(const double a, const double min, const double max)
{
    if(a > max){return max;}
    else if(a < min){return min;}
    else {return a;}
}

Array3d clipArray3d(Array3d &array, const double min, const double max)  // Modifie l'array
{
    array[0] = clip(array[0], min, max);
    array[1] = clip(array[1], min, max);
    array[2] = clip(array[2], min, max);

    return array;  // En plus
}

char dt255(const double d)
{
    return char(255 * d);
}

Vector3d vec(const Vector3d &A, const Vector3d &B)
{
    return B - A;
}

double ps(const Vector3d &vec1, const Vector3d &vec2)
{
    return vec1.dot(vec2);
}

double norme(const Vector3d &vec)
{
    return sqrt(vec.dot(vec));
}

Vector3d unitaire(const Vector3d &vec)
{
    return vec / norme (vec);
}

Vector3d pt(const Rayon &ray, const double t)
{
    return ray.m_R + t * ray.m_u;
}

Vector3d direction(const Vector3d &A, const Vector3d &B)
{
    return unitaire(vec(A, B));
}

Rayon ra(const Vector3d &A, const Vector3d &B)
{
    return Rayon(A, direction(A, B));
}

Sphere sp(const Vector3d &A, const Vector3d &B)
{
    return Sphere(A, norme(vec(A, B)));
}

Intersection Sphere::intersection(const Rayon &ray) const
{
    /**
     * Renvoie l'intersection de la sphère avec le rayon
     * Si aucune intersection n'st valide, le t renvoyé vaut 0
     */
    Vector3d CA(vec(m_C, ray.m_R));

    double b(2*CA.dot(ray.m_u));

    double c(CA.dot(CA) - m_r*m_r);

    double delta(b*b - 4*c);

    if(delta >= 0){  // Il existe une solution

        double racine(sqrt(delta));

        // 2 solutions  :  t1 < t2
        double t1((-b - racine)/2);
        double t2((-b + racine)/2);

        if(t1 > THRESHOLD){
            return Intersection(pt(ray, t1), t1);
        } else if(t2 > THRESHOLD){
            return Intersection(pt(ray, t2), t2);
        }
    }  // Fin du delta >= 0
    return Intersection::invalid();  // Aucune intersection : t = 0
}


bool Sphere::visible(const Vector3d &P, const Source &src) const
{
    /**
     * Renvoie si une source est visible depuis un point d'une sphère
     * (fonction au_dessus)
     */

    //Vector3d SP(vec(src.m_S, P));
    //Vector3d CP(vec(m_C, P));

    return vec(src.m_S, P).dot(vec(m_C, P)) <= 0;
}


Vector3d Sphere::normal(const Vector3d &P) const
{
    return direction(m_C, P);
}


Intersection Plan::intersection(const Rayon &ray) const
{
    /**
     * Intersection d'un point avec un plan de normale N
     * Attention aux signes lors du calcul de t, qui est positif ou infini!
     *
     * pour ne pas prendre en compte les intersections qui vont trop loin :
     * test du produit scalaire, qui ne doit pas être trop proche de 0
     * (on utilise ANGLE_LIMITE)
     */

    //Vector3d PS(vec(ray.m_R, m_P));

    double pdt_scal(ray.m_u.dot(m_N));  // Si = 0 : pas d'intersection

    if(abs(pdt_scal) < ANGLE_LIMITE){  // Aucune intersection (attention à prendre la valeur absolue!)
        return Intersection::invalid();
    } else {

    double projection = vec(ray.m_R, m_P).dot(m_N);  // Projection sur le vecteur normal
    double t = projection / pdt_scal;


    //Vector3d point(pt(ray, t));  // Point d'intersection
    if(t > 0){  // Si le t est valide
        return Intersection(pt(ray, t), t);  // Intersection valide
    } else {
        return Intersection::invalid();
    }
    }
}

bool Plan::visible(const Vector3d &P, const Source &src) const
{
    /**
     * Si une source est "visible" par un point du plan
     * En fait, si le plan cache la source depuis le point de vue omega, alors
     * visible renvoie faux (via signes de produits scalaires)
     */
    /// Déterminer si la source est du bon côté :
    /// pas un produit scalaire avec la source...
    /// vecteur omega vers source : si ça croise le plan, c'est pas visible
    ///
    /// IMPOSSIBLE A FAIRE

    return true;
}


// Fonctions de calcul

int get_valid_intersection(const vector<Intersection> &intersections)
{
    /**
     * Renvoie l'index de l'intersection de t minimum, avec t qui n'est pas nul
     * S'il n'y en a aucune (toutes nulles), renvoie -1
     */
    /*
    if(intersections.size() == 0){
        throw "vector should contain at least one element";
    }
    */
    double min_t(0);
    int i;  // l'index à renvoyer

    for(auto it = intersections.begin(); it != intersections.end(); ++it){

        if(it->is_valid() && (min_t == 0  || it->m_t < min_t)){
            min_t = it->m_t;
            i = distance(intersections.begin(), it);  // Index correspondant
        }
    }
    if(min_t == 0){  // Aucune intersection valide trouvée
        return -1;  // valeur par défaut
    } else {
        return i;
    }
}


bool visible(const vector<Objet*> &objets, const int i, const Vector3d &P, const Source &src)
{
    // Vecteur de pointeurs pour utiliser le polymorphisme

    if(objets.at(i)->visible(P, src)){  // On vérifie la visibilité "interne"

        Rayon ray(ra(src.m_S, P));  // Rayon dont on vérifie les intersections

        vector<Intersection> intersections;

        for(Objet* objet : objets){
            // On parcourt tous les objets pour calculer les intersections
            intersections.push_back(objet->intersection(ray));
        }

        // On sait qu'il y a au moins une intersection valide : pas de test de -1
        if(int(i) == int(get_valid_intersection(intersections))){
            return true;  // la première intersection est bien avec cet objet
        }
    }
    return false;  // Par défaut
}


Array3d couleur_diffusee(const Source &src, const Vector3d &P, const Objet* objet){
    /**
     * On suppose que la source est visible
     * Renvoie la couleur de la source diffusée par le point P de l'objet objet
     *
     * Il faut connaître le vecteur normal de l'objet utilisé : encore une méthode virtuelle
     *
     * PLAN 100% DIFFUSANT : VOIR COMMENT GERER LA NORMALE POUR AVOIR TOUJOURS 100% DE DIFFUSION
     * surcharger
     */
    // calcul du cos de l'angle d'incidence:

    return src.m_col * objet->m_kd * direction(P, src.m_S).dot(objet->normal(P));
}


Rayon rayon_reflechi(const Objet* objet, const Rayon &ray, const Vector3d &P)
{
    Vector3d N(objet->normal(P));  // Vecteur normal
    return Rayon(P, ray.m_u - 2* ray.m_u.dot(N) * N);
}


Point interception(const vector<Objet*> objets, const Rayon &ray)
{
    /**
     * Renvoie le premier point rencontré par le rayon ainsi que
     * l'index de l'objet auquel il appartient
     * Si aucune collision : l'indice renvoyé est -1
     * => se teste via la méthode is_valid
     */

    vector<Intersection> intersections;

    for(Objet* objet : objets){
        intersections.push_back(objet->intersection(ray));
    }

    // On chope l'index de l'intersection valide
    int index = get_valid_intersection(intersections);

    if(index != -1){
        return Point(intersections[index].m_P, index);
    } else {
        return Point::invalid();
    }
}


Array3d couleur_diffusion(const vector<Objet*> &objets, const vector<Source> &sources, const Vector3d &P, const int i)
{
    /**
     * Renvoie la couleur diffusée par le point P appartenant à l'objet d'indice i
     */

    Array3d col(NOIR);  // Couleur par défaut

    for(Source src : sources){

        if(visible(objets, i, P, src)){
            col += couleur_diffusee(src, P, objets[i]);
        }
    }
    return col;
}


vector<Point> reflections(const vector<Objet*> &objets, Rayon &ray, const int rmax)
{
    /**
     * Renvoie la liste des points successivements rencontrés par un rayon réfléchi
     * ATTENTION : les points sont tous valides
     */

    vector<Point> liste;

    Point point(interception(objets, ray));  // Initialisation


    while(point.is_valid() && int(liste.size()) < rmax + 1){  // On n'ajoute que les points valides

        liste.push_back(point);  // c'est ici que se fait le 1er ajout

        // Pour éviter beaucoup de calculs inutiles, on est obligés de rajouter un test
        if(int(liste.size()) < rmax + 1){
            ray = rayon_reflechi(objets[point.m_i], ray, point.m_P);
            point = interception(objets, ray);
        }

    }
    return liste;
}


Array3d couleur_percue(const vector<Objet*> &objets, const vector<Source> &sources, Rayon &ray, const Array3d &fond, const int rmax)
{
    vector<Point> liste(reflections(objets, ray, rmax));

    if(liste.empty()){
        return fond;  // Aucune interception : couleur du fond
    } else {
        Array3d col(NOIR);  // Couleur par défaut

        // On parcourt la liste de points à l'envers
        for(auto it = liste.rbegin(); it != liste.rend(); ++it){
            col = couleur_diffusion(objets, sources, it->m_P, it->m_i) + col * objets[it->m_i]->m_kr;
        }

        clipArray3d(col);  // On sature à 1. pour éviter un overflow
        return col;
    }
}



// Complémentaire

vector<Source> polygonal_source(const Vector3d &P, const Vector3d &N, const Array3d &colour, const double r, const int n)
{
    /**
     * Génère une liste de sources : source centrale en P
     * Autres sources réparties sur les n sommets d'un polygône régulier de normale N.
     * Distance centre - sommet : r
     * Couleur totale : colour.
     */
    /*
    if(n < 3){
        throw "n must be at least 3 (else it looks like trash)";
    }
    */
    vector<Source> sources;
    Array3d col(colour / (n + 1));  // Couleur divisée entre toutes les sources pour éviter la saturation

    sources.push_back(Source(P, col));  // Source centrale

    // Création d'une base orthonormale à partir du vecteur normal N pour créer les n autres sources
    Vector3d u(N[0] * 3 - 8, N[1] * (-3) + 8, N[2] * 2 + 10);  // De sorte que u et N non colinéaires

    u = unitaire(N.cross(u));  // 1er vecteur

    Vector3d v(N.cross(u));  // 2ème vecteur

    double theta(0);  // Angle dans la base locale

    for(int i = 0; i<n; i++){
        theta = 2 * M_PI * i / n;

        //w = u * cos(theta) + v * sin(theta);  // Je l'ai directement mis dedans

        sources.push_back(Source(P + r * (u * cos(theta) + v * sin(theta)), col));
    }
    return sources;
}


