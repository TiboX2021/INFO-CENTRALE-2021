"""
Rendering en ray tracing.
A faire en c++ (opencv??)
"""
import numpy as np
import matplotlib.pyplot as plt
from time import time
##

# Variables globales

Objets = [ [np.array([0, 0, -3]), 0.6]]  # liste des objets
KdObj = [ np.array([1., 1, 1.])]  # liste des coeffs de diffusion des objets
KrObj = [ np.array([1., 1., 1.])]  # A REMPLIR : coeffs de réflection
Source = [ np.array([3, 0, -3])]  # liste des points source
ColSrc = [ np.array([1., 1, 1.]) ] # liste des couleurs des points sources

height, width = 512, 1024 # Dimensions de l'image (PAIR)

ratio = 1 / width  # distance correspondant à 1 côté d'un pixel
# Normalisation : pour toute définition, on prend une largeur de 1m

# Couleurs (attention, il faut copier les listes)
noir = np.array([0, 0, 0], dtype='float64')
fond = np.array([0.2, 0.2, 0.2])

##

def vec(A, B):
    """
    A, B : points : np.array de taille 3
    """
    return B - A


def ps(v1, v2):
    """
    Produit scalaire de v1 et v2
    """
    return np.inner(v1, v2)


def norme(v):
    """
    Norme d'un vecteur
    """
    return np.sqrt(ps(v, v))


def unitaire(v):
    return v / norme(v)


##

def pt(ray, t):

    assert t >= 0
    (S, u) = ray
    return S + t* u


def direction(A, B):
    return unitaire(vec(A, B))


def ra(A, B):
    return A, direction(A, B)

##

def sp(A, B):

    return A, norme(vec(A, B))


def intersection(ray, s):
    """
    Renvoie le 1er point de la sphère touché par le rayon, avec le t correspondant,
    ou None
    """

    CA = vec(s[0], ray[0])  # Vecteur CA, centre sphère -> point du rayon

    b = 2 * ps(CA, ray[1])  # Coeff b de l'équation

    c = ps(CA, CA) - (s[1])**2  # Coeff c de l'équation

    delta = b**2 - 4*c

    if delta >= 0:  # Il existe une solution

        racine = np.sqrt(delta)

        t1, t2 = (-b - racine)/2, (-b + racine)/2
        # On a t1 < t2 : on teste s'ils sont valides

        if t1 >= 0:

            return pt(ray, t1), t1

        elif t2 >= 0:

            return pt(ray, t2), t2


    return None  # Par défaut


def au_dessus(s, P, src):
    """
    Teste si un point source est visible depuis un point P d'une sphère s
    """

    SP = vec(src, P)
    CP = vec(s[0], P)

    return ps(SP, CP) <= 0


def visible(obj, j, P, src):
    """
    Idem : la sphère est obj[j]
    Cette fonction prend en compte le fait que cette sphère est potentiellement
    occultée par d'autres objets
    ATTENTION: j'ai changé l'ordre par rapport au sujet!! j'utilise interception
    """
    if au_dessus(obj[j], P, src):  # 1er test

        # Tests d'occultage
        # Calcul de toutes les intersections du rayon de la source vers P
        # avec tous les ojbets

        ray =  ra(src, P)

        """
        inter = []  # liste des intersections (les t)

        for s in obj:

            inter.append((intersection(ray, obj[j]))[1])

        # On teste si le t de l'objet j est le min
        if inter[j] == min(inter):

            return True
        """
        a = interception(ray)
        if a[1] == j:  # Si l'objet de la 1ère intersect est le bon
            return True


    return False  # Par défaut


##

def couleur_diffusee(ray, Cs, N, kd):

    costheta = ps(N, ray[1])

    return  - Cs * kd * costheta


def rayon_reflechi(s, P, src):
    """
    rayon de src vers P réfléchi, partant alors de P
    (à résoudre mathématiquement)
    renvoie le rayon (P, direction)
    """

    u = direction(src, P)  # vecteur du rayon incident

    N = direction(s[0], P)  # vecteur normal

    return P, u - 2 * ps(u, N) * N

##

def grille(i, j):
    """
    Coordonnées spatiales du pixel (i, j)
    abscisse : j
    ordonnée : i
    """

    # Coordonnées par rapport à l'origine en haut à gauche
    AE = np.array([j * ratio, -i * ratio, 0])

    # Coordonnées de ce coin par rapport au centre
    OA = np.array([ - width * ratio / 2 + ratio/2, + height * ratio / 2 - ratio/2, 0])

    return OA + AE


def rayon_ecran(omega, i, j):
    """
    omega : position de l'oeil ponctuel derrière l'écran
    """

    return  ra(omega, grille(i, j))


def interception(ray):
    """
    1er objet de Objets touché par ce rayon.
    Renvoie le point d'intersection et l'indice correspondant
    """
    t_min = 0.0001  # fix temporaire pour éviter l'interception d'un rayon
    # d'une sphère par cette même sphère


    listeP = []  # liste des intersections (P)
    listet = []  # liste des intersections (t)

    for obj in Objets:

        inter = intersection(ray, obj)

        if inter != None:
            listeP.append(inter[0])
            listet.append(inter[1])
        else:
            listeP.append(None)
            listet.append(None)

    # Calcul du t minimal s'il existe
    filt = np.array(list(filter(None, listet)))

    #FIX TEMPORAIRE : on retire les t trop petits
    filt = filt[filt > t_min]

    if len(filt) > 0:

        t = min(filt)
        i = listet.index(t)  # indice de la 1ère intersection
        return listeP[i], i

    return None  # Par défaut : aucune intersection


def couleur_diffusion(P, j):
    """
    Couleur diffusée par le point P de la sphère d'indice j
    On fait la somme de toutes les couleurs diffusées
    """
    C = np.array([0, 0, 0], dtype='float64')  # Couleur noire par défaut

    for i in range(len(Source)):

        if visible(Objets, j, P, Source[i]):

            C += couleur_diffusee(ra(Source[i], P), ColSrc[i], direction(Objets[j][0], P), KdObj[j])

    return C

##

def lancer(omega, fond):
    """
    Calcule l'image avec l'oeil en omega, couleur de fond : fond
    """

    image = np.empty((height, width, 3))  # Image à remplir

    for i in range(height):

        for j in range(width):

            ray = rayon_ecran(omega, i, j)  # pixel dont la couleur est à calculer

            inter = interception(ray)

            if inter == None:
                image[i][j] = fond
            else:
                image[i][j] = couleur_diffusion(inter[0], inter[1])

    return image


## REFLETS

def reflexions(ray, rmax):
    """
    liste des points successivements rencontrés par un rayon réfléchi
    """
    liste = []  # liste de couples (P, i) intersectés

    src = ray[0]


    while (couple := interception(ray)) is not None and len(liste) < rmax + 1:

        liste.append(couple)

        #
        ray = rayon_reflechi(Objets[couple[1]], couple[0], src)

        src = couple[0]

    return liste


def couleur_percue(ray, rmax, fond):
    """
    Fait la somme successive de toutes les couleurs diffusées
    en passant la liste des points réfléchis à l'envers

    problème de kr, KrObj[0] est toujours appelé
    """
    liste = reflexions(ray, rmax)


    if len(liste) == 0:

        return fond  # Aucune interception

    else:

        C = np.array([0, 0, 0], dtype='float64')  # default : noir

        for i in range(len(liste) - 1, -1, -1):  # Parcourt à l'envers

            C = couleur_diffusion(liste[i][0], liste[i][1]) + KrObj[liste[i][1]] * C

        return np.clip(C, 0., 1.)  # Saturation : on restreint les valeurs entre 0 et 1

def lancer_complet(omega, fond, rmax):

    t = time()
    image = np.empty((height, width, 3))  # Image à remplir

    for i in range(height):

        for j in range(width):

            ray = rayon_ecran(omega, i, j)  # pixel dont la couleur est à calculer

            image[i][j] = couleur_percue(ray, rmax, fond)

    print('temps écoulé :', int(time()-t), 's')

    return image

#TODO : avec rmax  = 1 : la rouge reflète alors qu'elle n'est pas censée le faire
#TODO : look pixellisé, bizarre


"""
Explication :
la liste des réflections a parfois 2 membres (il existe une réflection), parfois 1. Alors qu'elle devrait en avoir 1 seul. Il y a des rayons réfléchis qui interceptent bizarrement


PAR COMPARAISON :
les zones foncées ont la bonne couleur
la moitié des zones sont beaucoup plus brillantes alors qu'elles ne devraient pas l'être

PISTE : erreur qui fait que des rayons reflétés entrent en collision avec d'autres points directement adjacents appartenant à la même sphère, à cause d'erreurs minuscules...
Il faut empêcher de chercher des collisions avec la même sphère
Ou alors : avoir un t minimum pour la collision (à tester) => ça marche


dernier problème : la boule rouge réfléchit la blanche alors qu'elle est non réfléchissante
(je teste en mettant la rouge 100% réfléchissante pour voir la différence)
ce changement n'a aucun effet sur le résultat (de 0% à 100%)

avec une boule violette : c'est bien la couleur de cette boule qui est réfléchie, mais pourquoi ça ne change rien???

test avec les 2 boules non réfléchissantes : aucun reflet

la boule rouge doit utiliser le Kr de la boule blanche, à revoir
=> effectivement, c'est toujours KrObj[0] qui est utilisé,
j'utilisais le i de la boucle for au lieu du i de liste[i][1]
"""


def f():

    t = time()
    img = lancer_complet(np.array([0, 0, 2]), np.array([0.2, 0.2, 0.2]), 0)
    print(time() - t)

    plt.imshow(img)
    plt.show()
## Sources étendues


# source en forme d'hexagone
# via les racines nièmes de l'unité

def source_etendue(centre, direction, rayon, n):
    """
    centre : centre de la source
    rayon : distance du centre aux sommets
    n : nombres de sommets
    direction : direction du truc

    A voir comment je l'utilise
    """
    liste = []

    # génération d'une base locale orthonormale à partir de "direction"
    vecteur = np.array([direction[0] * 2 - 7, direction[1] * (-1) + 7, direction[2] * 4 -5])

    v1 = unitaire(np.cross(vecteur, direction))
    v2 = unitaire(np.cross(v1,direction))


    for i in range(n):
        # génération des sources sommets via les racines niemes de l'unité
        theta = 2* np.pi * i / n

        v = unitaire(v1 * np.cos(theta) + v2 * np.sin(theta))

        liste.append(centre + v * rayon)

    return liste, n +1



