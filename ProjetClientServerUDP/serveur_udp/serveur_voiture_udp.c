/**
 * @author Amine Amzal
 * @author Alassane BA
 * 
 * @create date begin : 2021-03-04
 * @end date : 2021-05-14
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define NB_VOITURES 15

int sock = 0; // socket UDP
int inter = 1; // variable de boucle
pthread_t th; // référence au thread

/* Tableau contenant les noms de voitures disponibles */
char * voitures[] = {
    "audi_a1_blanche_2019",
    "audi_q5_grise_2021",
    "audi_r8_bleue_2019",
    "audi_rs6_noire_2020",
    "audi_serie1_blanche_2020",
    "bmw_i8_grise_2020",
    "bmw_m3_blanche_2021",
    "bmw_m4_noire_2020",
	"bmw_m5_rouge_2020",
    "bmw_serie8cabriolet_noire_2019",
    "ford_bronco_grise_2021",
    "ford_focus_noire_2020",
    "ford_fusion_grise_2019",
    "ford_mustang_bleue_2019",
    "ford_ranger_bleue_2020"
};

/**
 * Cette fonction envoie un signal SIGUSR1 au thread de référence "th"
 * et fait passer la variable "inter" à 0
 * 
 * @param signal est un entier et représente la valeur du signal 
 * 
 */ 
void thread_over(int signal) {
	printf("Je termine normalement. Cookies bien mérités 😁 😅 \n");
	pthread_kill(th,SIGUSR1 );
	inter = 0;
}

/**
 * Cette fontion permet d'arrêter le thread courant avec pthread_exit(..)
 * 
 * @param signal est un entier et représente la valeur du signal 
 * 
 */ 
void quitter(int signal){
	pthread_exit(NULL);
}

/**
 * Cette fonction concatène les données reçues sur le sock et vérifie la disponibilité de la demande 
 * et renvoie le chemin vers l'image correspondantes
 * 
 * @param arg est un pointeur qui contient la valeur du socket UDP
 * 
 * @requires arg != NULL
 * 
 */ 
void *effectuer_recherche(void *arg){
	int sock = *(int*)arg;
	struct sockaddr_in client;
	size_t client_len = sizeof(client);
	char marque[10], modele[20], couleur[15], str[5], nom_voiture[70], reponse[70] = "../voitures/";
	int annee_fabrication, i;
	char * erreur_msg = "Je ne dispose pas d'une image de cette voiture";
	
	signal(SIGUSR1 ,quitter);
	
	if (recvfrom(sock, &marque, sizeof(marque), 0, (struct sockaddr *)&client, (socklen_t *)&client_len) <= 0)
		perror("Erreur recvfrom marque");
	strcat(reponse,marque); strcat(reponse,"_");
	
    if (recvfrom(sock, &modele, sizeof(modele), 0, (struct sockaddr *)&client, (socklen_t *)&client_len) <= 0)
		perror("Erreur recvfrom modele");
    strcat(reponse,modele); strcat(reponse,"_");

    if (recvfrom(sock, &couleur, sizeof(couleur), 0, (struct sockaddr *)&client, (socklen_t *)&client_len) <= 0)
		perror("Erreur recvfrom couleur");
	strcat(reponse,couleur); strcat(reponse,"_");

    if (recvfrom(sock, &annee_fabrication, sizeof(annee_fabrication), 0, (struct sockaddr *)&client, (socklen_t *)&client_len) <= 0)
		perror("Erreur recvfrom annee_fabrication");
	sprintf(str, "%d", annee_fabrication); strcat(reponse,str);

	strcat(nom_voiture,marque); strcat(nom_voiture,"_"); strcat(nom_voiture,modele); strcat(nom_voiture,"_");
	strcat(nom_voiture,couleur); strcat(nom_voiture,"_"); strcat(nom_voiture,str);

	for(i=0; i<NB_VOITURES; i++){
		if(strcmp(nom_voiture,voitures[i]) == 0){
			strcat(reponse,".bmp");
			printf("%s\n", reponse);
			break;
		}
	}

	if(sendto(sock, reponse, sizeof(reponse),0,
			(struct sockaddr *)&client, sizeof(client)) < 0)
		perror("Erreur sendto");
	pthread_exit(NULL);
}

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec toutes les adresses ips possibles, sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur){
  	serveur->sin_family = AF_INET;
  	serveur->sin_port = htons(port_serveur);
  	serveur->sin_addr.s_addr = htonl(INADDR_ANY);
}

/**
   Initialise une socket permettant d'accepter des communications en UDP depuis n'importe
   qu'elle adresse IP.   
   @param port_serveur est le port auquel la socket sera liée.
   @return une socket prête à l'emploi si aucune erreur de survient, quitte le programme sinon.
*/
int initialiser_socket(unsigned short port_serveur){
	struct sockaddr_in serveur;
	int sock;
	init_sockaddr_in(&serveur, port_serveur);
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Erreur lors de la création de socket UDP");
		exit(EXIT_FAILURE);
	}  
	if(bind(sock, (struct sockaddr *) &serveur, sizeof(struct sockaddr_in)) < 0){
		perror("Erreur lors de l'appel à la fonction bind");
		exit(EXIT_FAILURE);
	}
  	return sock;
}


/**
   Ce programme crée un serveur dédié qui rečoit le nom de la voiture demandé par le client, vérifie si elle est disponible
   via un thread et dans le cas échéant, renvoie le chemin vers l'image.
   Le programme prend en paramètre le port sur lequel les communications seront effectuées.
*/
int main(int argc, char** argv) {

	short port = 0;
	
	if (argc!=2) {
		fprintf(stderr,"usage: %s port\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	port = atoi(argv[1]);

	sock = initialiser_socket(port);
	
	signal(SIGINT , thread_over);

	while (inter==1) {
		int *arg = malloc(sizeof(int));
		*arg = sock;
		pthread_create(&th ,NULL ,  effectuer_recherche,arg);						
		pthread_join(th,NULL);
		free(arg);	
	}
	return EXIT_SUCCESS;
}
