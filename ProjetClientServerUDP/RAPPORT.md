/**
 * @author Amine Amzal
 * @author Alassane BA
 * 
 * @create date begin : 2021-03-04
 * @end date : 2021-05-14
 * 
 */

# Sujet : Crash Cars (Photothèque de voitures up-to-date)

Ce programme vous permettra de visualiser la voiture de vos rêves.
Nous disposons dans cette photothèque des voitures les plus demandées sur le marché.
Une fois le programme lancé, vous aurez à saisir la marque suivie du modèle puis la couleur et enfin l'année de fabrication.

0. Pourquoi UDP ?
    - pour servir un grand nombre de clients 
    - compte tenu du grand nombre de clients, l'UDP permet d'accepter tous les clients sans échange de ACK et SEQ

1. Explication détaillée du code client 

On commence par créer la structure du serveur. On crée trois variables port_serveur de type short qui sera par la suite initialisée au port passé en paramètre lors de l'exécution, ip_serveur de type char* et sock de type int initialisée à 0.

On teste si lors de l'exécution, l'ip du serveur et le port ont été bien passés en paramètres dans cet ordre. Et dans le cas contraire, on affiche un message d'erreur sur la sortie standard d'erreurs. 
On initialise le socket avec la fonction initialiser_socket qui prend en paramètre l'ip_serveur, le port_serveur et l'adresse du serveur.

On affiche une fenêtre d'accueil.
On affiche la liste de voitures disponibles après avoir fermé la fenêtre d'accueil. 

Appel à la fonction effectuer_recherche(..) :
    On demande au client de saisir la marque, le modèle, la couleur et l'année de fabrication qui seront envoyés l'une après l'autre au serveur avec la fontion sendto(...). Le serveur effectue la recherche et renvoie le nom de l'image (avec l'extension .bmp) à afficher avec la fonction afficher_image(...) si la voiture est disponible.
    Dans le cas contraire, il affiche un message d'erreur (Désolé, Je ne dispose pas d'une image de cette voiture).

    - Fonction afficher_image
    On utilise la librairie SDL du langage C pour afficher les images. 
    On crée une fenêtre, un renderer, une texture et une surface où on va afficher l'image. Pour ce faire, on utilise la fonction SDL_LoadBMP qui prend en paramètre le chemin vers l'image. 
    On fait des tests d'allocations de mémoire et on libère tout à la fin.

2. Explication détaillée du code serveur

On commence par déclarer la variable port initialiée à 0. Ensuite, on vérifie que le nombre d'arguments est bien égal à 2 (le port sur lequel va se connecter le client). On initialise le port avec celui passé en paramètre et la variable sock qui est globale avec la fontion initialiser_socket qui prend en paramètre le port. On utilise une varible de boucle inter (globale) initialisée à 1. 
On alloue de la mémoire pour le pointeur *arg et on lance un thread qui exécute la fonction effectuer_recherche avec comme argument arg (qui contient la valeur de sock). 

Appel à la fonction effectuer_recherche(..) :
    On recupère la valeur de sock qui était dans arg après l'avoir casté.
    Le serveur reçoit les données saisies par le client, les concatène l'une après l'autre reliées avec le caractère ("_"). Ce dernier vérifie si la voiture demandée est bien disponible dans la photothèque (cf tableau char * voiture[]). 
    Si la recherche aboutie, il ajoute l'extension .bmp à la réponse et la renvoie au client avec la fontion sendto(...). 
    si tout se passe bien, on arrête le thread avec la fonction pthread_exit(..).

3. Explication du GraceFul Shutdown 
    - On commence par récupérer un signal SIGINT (CTRL+C) dans le main avec la fonction signal(...). Cette dernière nous permettra d'exécuter la fonction thread_over(...) au lieu d'arrêter le programme brutalement. 
    - thread_over(...) : Cette fonction nous permettra d'envoyer un signal SIGUSR1 au thread lancé dans le main (la référence vers le thread "th" est une variable globale) et la variable "inter" passe à 0.
    Lorsque le thread reçoit le signal SIGUSR1, au lieu de s'arrêter brutalement causant une erreur, il exécute la fonction quitter(...) qui nous permet d'arrêter le thread proprement avec pthread_exit(..). 
    
4. Lors de la fermeture du thread, on fusionne ce dernier avec le thread principal. Et on libère l'espace mémoire alloué à la variable "arg". 


Ce fut un travail d'équipe. 
