#include "render.h"

Render::Render()
{

}

Render::~Render()
{
    // Destruction des pointeurs :
    for(Objet *objet : m_objets){
        delete objet;  // Suppression de l'objet associé au pointeur
    }
}


void Render::set_width(const int width)
{
    /*
    if(width % 2 != 0){
        throw "width should be an even number";
    }
    if(width < 2){
        throw "width should be at least 2";
    }
    */
    m_width = width;
}


void Render::set_height(const int height)
{
    /*
    if(height % 2 != 0){
        throw "height should be a, even member";
    }
    if(height < 2){
        throw "height should be at least 2";
    }
    */
    m_height = height;
}


void Render::set_dimensions(const int height, const int width)
{
    set_height(height);
    set_width(width);
}


void Render::set_fond(const Array3d fond)
{
    m_fond = fond;
    clipArray3d(m_fond);
}


void Render::set_height_ratio(double m)
{
    m_ratio = m / double(m_height);
}


void Render::set_width_ratio(double m)
{
    m_ratio = m / double(m_width);
}


void Render::add_source(const Source source)
{
    m_sources.push_back(source);
}


void Render::add_objet(Objet* objet)
{
    // Attention : on doit doinc créer un new qqch lorsqu'on ajoute un objet.
    m_objets.push_back(objet);
}


void Render::add_sources(const vector<Source> sources)
{
    // ça plutôt que le truc avec les itérateurs de vecteur : être sûr que ça ne se détruit pas
    for(Source source : sources){
        m_sources.push_back(source);
    }
}


void Render::set_omega(const Vector3d omega)
{
    m_omega = omega;
}


void Render::set_rmax(const int rmax)
{
    m_rmax = rmax;
}


void Render::render_image(const string &filename)
{
    /// RENDERING #####################################################################################################
    cout << "creation image" << endl;
    bitmap_image image(m_width, m_height);  // Image vide
    cout << "image creee" << endl;
    auto start = chrono::high_resolution_clock::now();

    for(int i = 0; i < m_height; i++){

        cout << "avancement " << double(i) * 100. / double(m_height) << endl;

        for(int j = 0; j < m_width; j++){

            Vector3d rayon = rayon_ecran(i, j);

            Rayon ray(m_omega, rayon);
            Array3d col = couleur_percue(m_objets, m_sources, ray, m_fond, m_rmax);
            image.set_pixel(j, i, dt255(col[0]), dt255(col[1]), dt255(col[2]));
        }
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "temps d'execution  " << duration.count()/ double(1000000) << "  s"<< endl;

    string path("C:\\Users\\Thibaut de Saivre\\Desktop\\" + filename + ".bmp");

    image.save_image(path);
}


Vector3d Render::rayon_ecran(const int i, const int j)
{
    // vecteur directeur du rayon qui va de omega vers le point (i,j) de l'écran
    return direction(m_omega, Vector3d((j + (1 - m_width)/2)*m_ratio,((m_height-1)/2 - i)*m_ratio, 0));
}
