#include <iostream>
#include "render.h"


using namespace std;

int main()
{
    cout << "Hello World!" << endl;

    Render a = Render();

    a.set_dimensions(512, 1024);
    a.set_fond(Array3d(0.2, 0.2, 0.2));
    a.set_width_ratio(4);
    a.set_omega(Vector3d(0, 0, 40));
    a.set_rmax(1);

    a.add_objet(new Sphere(Vector3d(-1, 0, -4), 0.6, BLANC, BLANC));  // doit pouvoir réfléchir la lumière du plan
    a.add_objet(new Sphere(Vector3d(1, 0, -4), 0.3, BLANC, NOIR));  // Pas réfléchissant

    a.add_objet(new Plan(Vector3d(20, 0, -4), unitaire(Vector3d(-1, 0, 0)), BLANC, NOIR));  // Le plan n'est pas réfléchissant

    a.add_source(Source(Vector3d(6, 0, -4), BLANC/2));  // Moitié d'une source

    cout << "ca demarre" << endl;
    a.render_image("ombre sur plan");

    return 0;
}


/*  // Sources "polygonales"
//a.add_source(Source(Vector3d(4, 0, -4.1), BLANC));

a.add_sources(polygonal_source(Vector3d(4, 0, -4.1), Vector3d(-1, 0, 0), BLANC /2, 0.1, 50));
a.add_sources(polygonal_source(Vector3d(4, 0, -4.1), Vector3d(-1, 0, 0), BLANC /2, 0.05, 50));

a.add_objet(new Sphere(Vector3d(-1, 0, -4), 0.6));
a.add_objet(new Sphere(Vector3d(1, 0, -4), 0.2));
*/

/*  // Plan
a.add_objet(new Plan(Vector3d(0, 0, -4), unitaire(Vector3d(1, 0, 1))));

a.add_source(Source(Vector3d(3, 0, -4), BLANC));
*/

/*  // Reflection
a.add_objet(new Sphere(Vector3d(1, 0, -4), 0.6, BLANC, BLANC));  // Blanche réfléchissante
a.add_objet(new Sphere(Vector3d(-1, 0, -4), 0.6, Array3d(1, 0, 0), NOIR));  // Rouge non réfléchissante
*/


/*  // Source plane (aucune différence avec source ponctuelle)
a.add_objet(new Sphere(Vector3d(-1, 0, -4), 0.6, BLANC, BLANC));  // doit pouvoir réfléchir la lumière du plan
a.add_objet(new Sphere(Vector3d(1, 0, -4), 0.3, BLANC, BLANC));  // Pas réfléchissant

a.add_objet(new Plan(Vector3d(20, 0, -4), unitaire(Vector3d(-1, 0, 0)), BLANC, NOIR));  // Le plan n'est pas réfléchissant

a.add_source(Source(Vector3d(6, 0, -4), BLANC/2));  // Moitié d'une source
*/

/*  // Ombre sur plan

    a.add_objet(new Sphere(Vector3d(1, 0, -4), 0.6));  // doit pouvoir réfléchir la lumière du plan
    a.add_objet(new Plan(Vector3d(-2, 0, -4), unitaire(Vector3d(-1, 0, 0)), BLANC, NOIR));  // Le plan n'est pas réfléchissant

    a.add_source(Source(Vector3d(10, 0, -4), BLANC));



*/
