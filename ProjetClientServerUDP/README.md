# Pour pouvoir utiliser le programme comme il faut,

veuillez suivre scrupuleusement les étapes suivantes :

D'abord assurez-vous de disposer de la lilbrairie SDL sur votre machine.

Pour ce faire, si vous êtes sous linux, tapez la commande suivante :

    sudo apt-get install libsdl2-dev libsdl2-image-dev  libsdl2-ttf-dev

Pour macOS, pas besoin de l'installer car on a joint les entêtes 
(cf répertoire include) et lilbrairies (cf répertoire lib) nécessaires ;
Mais au cas où ça ne marcherait pas vous pourrez tapez la commande suivante :
    brew install sdl2

# Et si vous ne disposez pas du gestionnaire de paquets pour macOS brew (homebrew), 
# copiez et collez la commande suivante sur votre terminal :
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
# Et une fois l'installation de homebrew , utilisez :
    brew install sdl2

Et sous windows, utilisez le lien suivant :

    https://zestedesavoir.com/tutoriels/461/utiliser-une-bibliotheque-sous-windows-avec-mingw-ou-code-blocks-en-langage-c/

# Liens utiles en cas de problème d'installation de la librairie SDL :

Installation :
    https://zestedesavoir.com/tutoriels/1014/utiliser-la-sdl-en-langage-c/la-sdl/#2-configurer-un-projet-avec-la-sdl

Homebrew :
    https://brew.sh/index_fr

Après l'installation de la SDL, on peut maintenant compiler notre programme;

Pour ce faire, veuillez ouvrir deux fenêtres du terminal pour le serveur et pour le client :

Sous macOS (terminal iTerm2), on peut utiliser le raccourci clavier command+D
et sous linux (Temux) : CTRL+B puis %

# Compiler le serveur :

    cd serveur_udp
    make build (ou juste make)

# Compiler le client sous macOs:

    cd client_udp
    make build (ou juste make)
    
# Sous linux : pour compiler le client, utilisez la commande :
    gcc client_voiture_udp.c -o client -lSDL2

Aprés la compilation, vous verrez apparaître normalement les fichiers serveur et client
dans les répertoires serveur_udp (respectivement client_udp)

# Exécuter (ou lancer) le serveur puis le client avec la commande :

    make run

Si tout se passe bien, une fois que vous aurez lancé le client vous verrez une fenêtre apparaître affichant une image : (cf voitures/accueil.bmp)
Pour continuer, il faudra d'abord fermer cette fenêtre. Ensuite vous aurez à choisir une voiture parmi la liste proposée en saisissant la marque, le modéle, la couleur et l'année de fabrication.

On a utilisé le protocole UDP donc vous pouvez lancer plusieurs clients en même temps.

# Pour supprimer les exécutables, vous pouvez utiliser la commande : 
    make clean.
