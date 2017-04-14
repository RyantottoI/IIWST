/* SCOPO DEL PROGETTO
FUNZIONALITÀ DEL SERVER
Il server, di tipo concorrente, deve fornire le seguenti funzionalità:
-L’invio del messaggio di risposta al comando list al client richiedente;il messaggio di risposta contiene la filelist, ovvero la lista dei nomi dei file disponibili per la condivisione;
-L’invio del messaggio di risposta al comando get contenente il file richiesto, se presente, od un opportuno mex di errore;
-La ricezione di un messaggio put contenente il file da caricare sul server e l’invio di un messaggio di risposta con l’esito dell’operazione.
*/

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define SERV_PORT   5194 
#define MAXLINE     1024

void stampa_lista(){
  printf("sono stampa\n");
}

void download(){
  printf("sono il download\n");
}

void upload(int sockfd, struct sockaddr_in servaddr){
  printf("sono l'upload\n");
  
  char buff[10];
  strcpy(buff, "eccolo!");
  if (sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {  // Invia al server il pacchetto di richiesta
		perror("errore in sendto");
		exit(1);
	}
	printf("avvio avvenuto\n");
  
  
}

int main(int argc, char **argv) {
  int sockfd;
  socklen_t len;
  struct sockaddr_in addr;
  //char buff[MAXLINE];
  char *buff;
	//AGGIUNGERE DEALLOCAZIONE (SCRIVE PUTT E GETT)
  
  
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { /* crea il socket */
    perror("errore in socket");
    exit(1);
  }

  memset((void *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta pacchetti su una qualunque delle sue interfacce di rete */
  addr.sin_port = htons(SERV_PORT); /* numero di porta del server */

    //MODIFICARE IL NUMERO DI SOCKET PER PASSARE AD UN SERVER CONCORRENTE
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {   // assegna l'indirizzo al socket
    perror("errore in bind");
    exit(1);
  }

  while (1) { //while(1)=sempre vero. continua finchè non c'è errore o terminazione dell'utente
    buff=malloc(MAXLINE*sizeof(char));
    if(buff==NULL)
    {
        fprintf(stderr,"errore nell'allocazione della mnemoria\n");
        exit(EXIT_FAILURE);
    }
    int n = recvfrom(sockfd, buff, MAXLINE*sizeof(char), 0, (struct sockaddr *)&addr, &len);
    if (n < 0) {
      perror("errore in recvfrom");
      exit(1);
    }
    //CONTROLLO
    printf("numero caratteri ricevuti %d\n",n);
    char *comando;
    comando=malloc(n*sizeof(char));
    if(comando==NULL){
    	fprintf(stderr,"errore nell'allocazione della memoria\n");
    	exit(EXIT_FAILURE);
    	}
  
  int i=0;
  while(i<n){
	comando[i]=buff[i];
	i++;
	}

  //strcpy(comando, buff);
    printf("%s\n",comando);
    
    if(comando[0] == 'l'){
	stampa_lista();
	}
    if(comando[0] == 'g') {
        download();
	}
    if(comando[0] == 'p'){
        upload(sockfd, addr);
	}
/*
    free(comando);
    free(buff);
 */
  }
  exit(0);
}
