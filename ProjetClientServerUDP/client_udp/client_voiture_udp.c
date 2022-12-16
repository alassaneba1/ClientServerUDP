/**
 * @author Amine Amzal
 * @author Alassane BA
 * 
 * 
 * @create date begin : 2021-03-04
 * @end date : 2021-05-14
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <unistd.h>
#include <limits.h>
#include "SDL.h"

/**
 * Cette fonction permet de fermer la fenétre SDL 
 */
void closeWindow(){
    bool isquit = false;
        SDL_Event event;
        while (!isquit) 
            if (SDL_PollEvent( & event)) 
                if (event.type == SDL_QUIT) 
                    isquit = true;
}

/**
 * Cette fontion utilise la librairie SDL pour afficher l'image dont le chemin est passé en paramétre
 * 
 * @param chemin est le chemin vers l'image à afficher
 * 
 * @requires chemin != NULL
 */
void afficher_image(char * chemin_fichier){
        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;
        SDL_Texture *texture = NULL;
        int statut = EXIT_FAILURE;
        SDL_Color blanc = {200, 200, 200, 0};
        
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0 ){
            fprintf(stderr,"Érreur SDL_Init : %s", SDL_GetError());
            goto Quit;
        }

        if(SDL_CreateWindowAndRenderer(840, 680, SDL_WINDOW_SHOWN, &window, &renderer) != 0){
            fprintf(stderr, "Erreur SDL_CreateWindowRenderer : %s", SDL_GetError());
            goto Quit;
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 200, 200);
        if(texture == NULL){
            fprintf(stderr, "Erreur SDL_CreateTexture : %s", SDL_GetError());
            goto Quit;
        } 

        SDL_SetRenderDrawColor(renderer, blanc.r, blanc.g, blanc.b, blanc.a);
        SDL_RenderClear(renderer);

        SDL_Surface *tmp = NULL;
        tmp = SDL_LoadBMP(chemin_fichier);
        if(tmp == NULL){
            fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
            goto Quit;
        }
        texture = SDL_CreateTextureFromSurface(renderer, tmp);

        if(texture == NULL){
            fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
            goto Quit;
        }

        SDL_Rect dstrect = { 0, 0, 840, 680};
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);

        closeWindow();

Quit:
        if(tmp != NULL)
            SDL_FreeSurface(tmp);
        if(texture != NULL)
            SDL_DestroyTexture(texture);
        if(renderer != NULL)
            SDL_DestroyRenderer(renderer);
        if(window != NULL)
            SDL_DestroyWindow(window);

        SDL_Quit();
}

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec le serveur d'adresse 'ip_serveur', sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @require serveur != NULL && ip_serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur, char * ip_serveur){
	serveur->sin_family = AF_INET;
	serveur->sin_port = htons(port_serveur);
	inet_aton(ip_serveur, &serveur->sin_addr);
}

/**
   Initialise une socket UDP, et les structures `serveur` et `moi`,
   qui contienne des informations sur l'ip et le port utilisés par les deux points d'accès.
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param serveur est un pointeur vers la structure représentant les informations du serveur
   @require serveur != NULL && ip_serveur != NULL
   @return la socket initialisée, quitte le programme en cas d'échec.
*/
int initialiser_socket(char * ip_serveur, unsigned short port_serveur, struct sockaddr_in * serveur){
	int sock;
	init_sockaddr_in(serveur, port_serveur, ip_serveur);
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Erreur lors de la création de socket UDP");
		exit(EXIT_FAILURE);
	}
	return sock;
}

/**
 * Cette fonction permet au client de saisir les caractéristiques de la voiture, envoie les données
 * au serveur et attend la réponse puis affiche l'image dans le cas échéant sinon un message d'erreur
 * 
 * @param sock est la socket qui est utilisée pour communiquer
 * @param serveur est un pointeur vers la structure représentant les informations du serveur
 * 
 * @requires sock et serveur doivent être déjà initialisés 
 */ 
void effectuer_recherche(int sock, struct sockaddr_in serveur){
	char marque[10], modele[20], couleur[15], reponse[50];
	int annee_fabrication;
	size_t serveur_len = sizeof(serveur);
    char * erreur_msg = "\nDésolé, Je ne dispose pas d'une image de cette voiture";

	printf("Saisissez une marque de voitures : ");
	scanf("%s", marque);
    sendto(sock, &marque, 10*sizeof(char), 0, (struct sockaddr *)&serveur, serveur_len);

	printf("Saisissez un modele de voitures : ");
	scanf("%s", modele);
    sendto(sock, &modele, 20*sizeof(char), 0, (struct sockaddr *)&serveur, serveur_len);

	printf("Saisissez une couleur de voitures : ");
	scanf("%s", couleur);
    sendto(sock, &couleur, 15*sizeof(char), 0, (struct sockaddr *)&serveur, serveur_len);

	printf("Saisissez l'année de fabrication de la voiture : ");
	scanf("%d", &annee_fabrication);
    sendto(sock, &annee_fabrication, sizeof(int), 0, (struct sockaddr *)&serveur, serveur_len);

	if(recvfrom(sock, &reponse, sizeof(reponse), 0,
			(struct sockaddr *)&serveur, (socklen_t *)&serveur_len) < 0 )
		perror("Erreur recvfrom");
	else if(strstr(reponse,".bmp") == NULL)
            printf("%s\n", erreur_msg);
        else
	        afficher_image(reponse);	
}

/**
    Dans ce programme, à l'aide du protocole UDP, un client envoie au serveur
    les caractéristiques de la voiture qu'il cherche via un socket UDP,
    attend sa réponse (chemin vers l'image) et l'affiche.
    Le programme prend en paramètre l'ip et le port du serveur.
*/
int main(int argc, char** argv) {
	struct sockaddr_in serveur; 
	short port_serveur = 0;
	char * ip_serveur;
	int sock = 0; /* socket de communication */

	if (argc != 3) {
		fprintf(stderr,"usage: %s host port_serveur\n", argv[0]);
		return EXIT_FAILURE;
	}
	port_serveur = atoi(argv[2]);
	ip_serveur = argv[1];

	sock = initialiser_socket(ip_serveur, port_serveur, &serveur);
	
    printf("\nVeuillez fermer la fenêtre d'accueil pour pouvoir commencer votre recherche\n");
    afficher_image("../voitures/accueil.bmp");

    printf("\nLes lignes suivantes sont contituées comme suit :\n");
    printf("\n\t\t marque_modele_couleur_anneedefabrication\n");
    printf("\n\tAUDI :\n");
        printf("\t\t - audi_a1_blanche_2019\n");
        printf("\t\t - audi_q5_grise_2021\n");
        printf("\t\t - audi_r8_bleue_2019\n");
        printf("\t\t - audi_rs6_noire_2020\n");
        printf("\t\t - audi_serie1_blanche_2020\n");
    printf("\tBMW :\n");
        printf("\t\t - bmw_i8_grise_2020\n");
        printf("\t\t - bmw_m3_blanche_2021\n");
        printf("\t\t - bmw_m4_noire_2020\n");
        printf("\t\t - bmw_m5_rouge_2020\n");
        printf("\t\t - bmw_serie8cabriolet_noire_2019\n");
    printf("\tFORD :\n");
        printf("\t\t - ford_bronco_grise_2021\n");
        printf("\t\t - ford_focus_noire_2020\n");
        printf("\t\t - ford_fusion_grise_2019\n");
        printf("\t\t - ford_mustang_bleue_2019\n");
        printf("\t\t - ford_ranger_bleue_2020\n\n");

	effectuer_recherche(sock, serveur);

    close(sock);
	
	return EXIT_SUCCESS;
}
