/* SCOPO DEL PROGETTO
Progettare ed implementare in linguaggio C usando l’API del socket di Berkeley un’applicazione client-server per il trasferimento di file che impieghi il servizio di rete senza connessione (socket tipo SOCK_DGRAM, ovvero UDP come protocollo di strato di trasporto).
Il software deve permettere:
- Connessione client-server senza autenticazione;
- La visualizzazione sul client dei file disponibili sul server (comando list);
- Il download di un file dal server (comando get);
- L’upload di un file sul server (comando put);
- Il trasferimento file in modo affidabile.



La comunicazione tra client e server deve avvenire tramite un opportuno protocollo. Il protocollo di comunicazione deve prevedere lo scambio di due tipi di messaggi:
1) messaggi di comando: vengono inviati dal client al server per richiedere l’esecuzione delle diverse operazioni;
2) messaggi di risposta: vengono inviati dal server al client in risposta ad un comando con l’esito dell’operazione.

FUNZIONALITÀ DEL CLIENT
I client, di tipo concorrente, deve fornire le seguenti funzionalità:
- L’invio del messaggio list per richiedere la lista dei nomi dei file disponibili;
- L’invio del messaggio get per ottenere un file
- La ricezione di un file richiesta tramite il messaggio di get o la gestione dell’eventuale errore
- L’invio del messaggio put per effettuare l’upload di un file sul server e la ricezione del messaggio di risposta con l’esito dell’operazione.

TRASMISSIONE AFFIDABILE
Lo scambio di messaggi avviene usando un servizio di comunicazione non affidabile.
Al fine di garantire la corretta spedizione/ricezione dei messaggi e dei file sia i client che il server implementano a livello
applicativo il protocollo selective repeat con finestra di spedizione N (cfr. Kurose & Ross “Reti di Calcolatori e Internet”, 6° Edizione, sez. 3.4.4, pagg. 212-217).
Per simulare la perdita dei messaggi in rete (evento alquanto improbabile in una rete locale per non parlare di quando client e server sono eseguiti sullo stesso host), si assume che ogni messaggio sia scartato dal mittente con probabilità p */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>

#define SERV_PORT   5193 
#define MAXLINE     1024
#define ind "127.0.0.1"
/*
void chiama_server(char *comando){
	//printf("%s\n", comando);
	
	int sockfd, n;
 	struct sockaddr_in servaddr;
	char *buff;
	char *recvline;
	int len;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {  //crea il socket
	    	perror("errore in socket");
	    	exit(1);
  	}

	memset((void *)&servaddr, 0, sizeof(servaddr));      // azzera servaddr 
	servaddr.sin_family = AF_INET;       // assegna il tipo di indirizzo 
	servaddr.sin_port = htons(SERV_PORT);  // assegna la porta del server 
	
	/* assegna l'indirizzo del server prendendolo dalla riga di comando. L'indirizzo è una stringa da convertire in intero secondo network byte order */
	/*
	if (inet_pton(AF_INET, ind , &servaddr.sin_addr) <= 0) {    // inet_pton (p=presentation) vale anche per indirizzi IPv6
		fprintf(stderr, "errore in inet_pton");
		exit(1);
	}
	
	buff = malloc(sizeof(char)*strlen(comando));
	if(buff==NULL){
		perror("malloc");
		exit(1);
	}
	
	strcpy(buff, comando); //copio nel buffer il comando
	len = strlen(buff);  //calcolo la lunghezza del buff per la sendto()
	printf("lunghezza buffer %d\n", len);
	if (sendto(sockfd, buff, len, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {  // Invia al server il pacchetto di richiesta
		perror("errore in sendto");
		exit(1);
	}
	printf("avvio avvenuto\n");
/*
	n = recvfrom(sockfd, recvline, MAXLINE, 0 , NULL, NULL); 	//Legge dal socket il pacchetto di risposta
	if (n < 0) {
	    perror("errore in recvfrom");
	    exit(1);
	}
	if (n > 0) {
	    recvline[n] = 0;        /* aggiunge il carattere di terminazione */
 	    //CONTROLLARE LA STAMPA
 /*	    int dim = strlen(rcvline);
 	    int i;
 	    for(i=0; i<dim; ++i)
 	    	puts(recvline[i]);
 	    //CANCELLARE??
	    if (fputs(recvline, stdout) == EOF)   {  /* stampa recvline sullo stdout */
/*	    	fprintf(stderr, "errore in fputs");
	    	exit(1);
	    }
    	}*/
	/*
	sleep(2); //simulo la ricezine di recvfrom()    	
    	
    	FILE *fp1;
	int res;
	
	//FUNZIONA SOLO SE SONO NELLA STESSA CARTELLA, ALTRIMENTI SERVE IL PERCORSO
	fp1 = fopen("testo_prova", "rb"); //apro il file in lettura binaria con "rb"
	if(fp1 == NULL){
		fprintf(stderr, "errore nell'apertura del file da leggere\n");
		exit(EXIT_FAILURE);
	}
	printf("apertura file per upload avvenuta\n");
	
	fseek(fp1, 0, SEEK_END); //sposto il cursore alla fine
	len = ftell(fp1); //leggo la dimensione del file
	printf("dimensione file %d\n", len);
	fseek(fp1, 0, SEEK_SET); //sposto il cursore all'inizio del file
	
	free(buff); //dealloco la memoria di buff
	buff = malloc(len * sizeof(char)); //alloco memoria dinamica array (flusso)
	if(buff == NULL){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	printf("memoria allocata\n");
	
	int n1 = fread(buff, sizeof(char), len, fp1); //riempio il buffer con il flusso binario del file
	printf("lettura file effettuata\n");
	printf("numero di caratteri letti %d\n", n1); //stampo il numero di caratteri letti
	if(n1 != len){  //i caratteri letti devono essere pari a quelli del file
		perror("errore in lettura\n");
		exit(EXIT_FAILURE);
	}

	if (sendto(sockfd, buff, len, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {  // Invia al server il pacchetto di richiesta
		perror("errore in sendto");
		exit(1);
	}
	
	//GESTIRE LA RECVFROM()
	//rcvfrom() da l'ok dell'avvenuto upload
	
	//free(buff);   //dealloco memoria
	fclose(fp1); //chiudo il file letto
	
}*/

void connetti_server(char *buff, int len){
	int sockfd, n;
 	struct sockaddr_in servaddr;
	char *recvline;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {  //crea il socket
	    	perror("errore in socket");
	    	exit(1);
  	}

	memset((void *)&servaddr, 0, sizeof(servaddr));      /* azzera servaddr */
	servaddr.sin_family = AF_INET;       /* assegna il tipo di indirizzo */
	servaddr.sin_port = htons(SERV_PORT);  /* assegna la porta del server */
	
	/* assegna l'indirizzo del server prendendolo dalla riga di comando. L'indirizzo è una stringa da convertire in intero secondo network byte order */
	
	if (inet_pton(AF_INET, ind , &servaddr.sin_addr) <= 0) {    /* inet_pton (p=presentation) vale anche per indirizzi IPv6 */
		fprintf(stderr, "errore in inet_pton");
		exit(1);
	}
	
	/*
	buff = malloc(sizeof(char)*strlen(comando));
	if(buff==NULL){
		perror("malloc");
		exit(1);
	}
	
	strcpy(buff, comando); //copio nel buffer il comando
	len = strlen(buff);  //calcolo la lunghezza del buff per la sendto()
	printf("lunghezza buffer %d\n", len);
	*/
	if (sendto(sockfd, buff, len, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {  // Invia al server il pacchetto di richiesta
		perror("errore in sendto");
		exit(1);
	}
	printf("avvio avvenuto\n");
/*
	n = recvfrom(sockfd, recvline, MAXLINE, 0 , NULL, NULL); 	//Legge dal socket il pacchetto di risposta
	if (n < 0) {
	    perror("errore in recvfrom");
	    exit(1);
	}
	if (n > 0) {
	    recvline[n] = 0;        /* aggiunge il carattere di terminazione */
 	    //CONTROLLARE LA STAMPA
 /*	    int dim = strlen(rcvline);
 	    int i;
 	    for(i=0; i<dim; ++i)
 	    	puts(recvline[i]);
 	    //CANCELLARE??
	    if (fputs(recvline, stdout) == EOF)   {  /* stampa recvline sullo stdout */
/*	    	fprintf(stderr, "errore in fputs");
	    	exit(1);
	    }
    	}*/
}

void chiedi_file(char **comando, char op){ //richiede il nome del file e lo aggiunge alla lettera che identifica il comando 'g'/'p'
	printf("nome file da caricare: "); //chiedo il nome del file
	char nome[20];
	scanf("%s", nome);
	int n = strlen(nome); //calcolo la lunghezza del nome
	*comando = malloc((1+n)*sizeof(char)); //alloco memoria dinamica per "comando"
	if(malloc==NULL){
		perror("malloc");
		exit(1);
	}
	
	strcat(*comando, &op); //aggiungo "put" a comando
	strcat(*comando, nome); //aggiungo "nome file" a comando
	//printf("%s\n", comando);
}

void lista_file(){
	printf("lista_file\n");
}

void download(){
	printf("download\n");
}

void invia_operazione(char *comando){
	int sockfd, n;
 	struct sockaddr_in servaddr;
	char *buff;
	//char *recvline;
	int len;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {  //crea il socket
	    	perror("errore in socket");
	    	exit(1);
  	}

	memset((void *)&servaddr, 0, sizeof(servaddr));      // azzera servaddr 
	servaddr.sin_family = AF_INET;       // assegna il tipo di indirizzo 
	servaddr.sin_port = htons(SERV_PORT);  // assegna la porta del server 
	
	// assegna l'indirizzo del server prendendolo dalla riga di comando. L'indirizzo è una stringa da convertire in intero secondo network byte order
	
	if (inet_pton(AF_INET, ind , &servaddr.sin_addr) <= 0) {    // inet_pton (p=presentation) vale anche per indirizzi IPv6
		fprintf(stderr, "errore in inet_pton");
		exit(1);
	}
	
	buff = malloc(sizeof(char)*strlen(comando));
	if(buff==NULL){
		perror("malloc");
		exit(1);
	}
	
	strcpy(buff, comando); //copio nel buffer il comando
	
	len = strlen(buff);  //calcolo la lunghezza del buff per la sendto()
	printf("lunghezza buffer %d\n", len);
	
	if (sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {  // Invia al server il pacchetto di richiesta
		perror("errore in sendto");
		exit(1);
	}
	printf("avvio avvenuto\n");
	
	char recvline[10];
	n = recvfrom(sockfd, recvline, MAXLINE, 0 , NULL, NULL); 	//Legge dal socket il pacchetto di risposta
	if (n < 0) {
	    perror("errore in recvfrom");
	    exit(1);
	}
	if (n > 0) {
		printf("%s\n", recvline);
	    //recvline[n] = 0;        /* aggiunge il carattere di terminazione */
 	    //CONTROLLARE LA STAMPA
 	    /*int dim = strlen(rcvline);
 	    int i;
 	    for(i=0; i<dim; ++i)
 	    	puts(recvline[i]);
 	    //CANCELLARE??
	    if (fputs(recvline, stdout) == EOF)   {  // stampa recvline sullo stdout
	    	fprintf(stderr, "errore in fputs");
	    	exit(1);*/
	    }
}

void upload(){
	//COME USARE QUANTO SCRITTO PER PASSARE PUT+NOME_FILE??
	printf("upload\n");
	
	char *buff, *comando=NULL;
	int len;
	
	chiedi_file(&comando, 'p'); //chiede il nome del file per l'upload e aggiunge 'p'
	//printf("%s\n", comando);
	invia_operazione(comando);
	
	/*
	//LETTURA FLUSSO
	FILE *fp1;
	int res;
	int len;
	
	//FUNZIONA SOLO SE SONO NELLA STESSA CARTELLA, ALTRIMENTI SERVE IL PERCORSO
	fp1 = fopen("testo_prova", "rb"); //apro il file in lettura binaria con "rb"
	if(fp1 == NULL){
		fprintf(stderr, "errore nell'apertura del file da leggere\n");
		exit(EXIT_FAILURE);
	}
	printf("apertura file per upload avvenuta\n");
	
	fseek(fp1, 0, SEEK_END); //sposto il cursore alla fine
	len = ftell(fp1); //leggo la dimensione del file
	printf("dimensione file %d\n", len);
	fseek(fp1, 0, SEEK_SET); //sposto il cursore all'inizio del file
	
	char *buff = malloc(len * sizeof(char)); //alloco memoria dinamica array (flusso)
	if(buff == NULL){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	printf("memoria allocata\n");
	
	int n1 = fread(buff, sizeof(char), len, fp1); //riempio il buffer con il flusso binario del file
	printf("lettura file effettuata\n");
	printf("numero di caratteri letti %d\n", n1); //stampo il numero di caratteri letti
	if(n1 != len){  //i caratteri letti devono essere pari a quelli del file
		perror("errore in lettura\n");
		exit(EXIT_FAILURE);
	}	
	
	connetti_server(buff, len);
	*/
}

void main(int argc, char *argv[]) {
	int n;
	char *comando;
  	if (argc != 1) { /* controlla numero degli argomenti */
    		fprintf(stderr, "non vanno inseriti parametri\n");
    		exit(1);
  	}
	
	printf("lista operazioni\n 1: lista dei file nel server\n 2: download\n 3: upload\n 4: exit\n");
		//bisogna modificare i 3 if per poter inviare sia il comando che il file da scaricare o caricare dal server con l' aggiunta di una funzione prima di (chiama_server()) per gestire l' aggiunta del nome del file al comando
	while(n != 4){
		//CONTROLLO SULL'INPUT
		scanf("%d",&n); //leggo il comando inserito dall'utente
		if(n==1){ 	//lista dei file nel server
			//chiama_server("lst");
		
			lista_file();
		}
		else if(n==2){ 		//download
			/*
			printf("nome file da scaricare: "); //chiedo il nome del file
			char nome[30];
			scanf("%s", nome);
			n = strlen(nome); //calcolo la lunghezza del nome
			comando = malloc( (3+n)*sizeof(char)); //alloco memoria dinamica per "comando"
			if(malloc==NULL){
				perror("malloc");
				exit(1);
			}
			strcat(comando, "get"); //aggiungo "get" a comando
			//printf("%s\n", comando);
			strcat(comando, nome); //aggiungo "nome file" a comando
			//printf("%s\n", comando);
			chiama_server(comando); //invio il comando alla funzione
			*/
			download();
		}
			else if(n==3){ 		//upload
				/*
				printf("nome file da caricare: "); //chiedo il nome del file
				char nome[20];
				scanf("%s", nome);
				n = strlen(nome); //calcolo la lunghezza del nome
				comando = malloc((1+n)*sizeof(char)); //alloco memoria dinamica per "comando"
				if(malloc==NULL){
					perror("malloc");
					exit(1);
				}
				strcat(comando, "3"); //aggiungo "put" a comando
				//printf("%s\n", comando);
				strcat(comando, nome); //aggiungo "nome file" a comando
				//printf("%s\n", comando);
				chiama_server(comando); //invio il comando alla funzione
				*/
				upload();
			}
				else if(n==4){ 	    //exit
					break;
				}
					else fprintf(stderr, "operazione non valida\n");  //gestione dell'inserimento di un valore > 4
	}
}
