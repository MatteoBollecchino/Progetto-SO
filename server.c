#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "definizioniComuni.h"

#define PASSWORD "PROGETTOSO"
// #define dimBuffer 2048 // dimensione buffer per la comunicazione tramite
// socket

int loginUtente(int clientSocket);
void logoutUtente(int clientSocket);

/*
void user_loop(int clientSocket);

void search_record_procedure(int clientSocket, recordRub entries[], intentriesCount); 
void add_new_record_procedure(int clientSocket, recordRub entries[], int* entriesCount); 
void delete_record_procedure(int clientSocket,recordRub entries[], int entriesCount); 
void edit_record_procedure(int clientSocket, recordRub entries[], int entriesCount);

int search_records(recordRub entries[], int entriesCount, recordRub query,
recordRub queryResults[]); int add_new_record(recordRub entries[], int*
entriesCount, recordRub newDataEntry); int delete_record(recordRub entries[],
int entriesCount, recordRub entryToDelete); int edit_record(recordRub entries[],
int entriesCount, recordRub entryToEdit, recordRub editedEntry);

int search_record_position(recordRub entries[], int entriesCount, recordRub
query); void send_entries(int clientSocket, recordRub entries[], int
entriesCount); void update_runtime_database(recordRub newRuntimeDatabase[], int*
newRuntimeEntriesCount);

void handle_sigint(int sig);
void handle_death_signal_from_admin(int sig);
void handle_death_signal_from_user(int sig);

void handle_errno(int errorCode, char* errorMessage);
*/

int main() {

  int serverSocket, clientSocket;
  struct sockaddr_in indirizzo;
  int lunghezzaIndirizzo = sizeof(indirizzo);

  /*
    char input[dimBuffer] = {0};  // buffer utilizzato per le richieste dei
    client char output[dimBuffer] = {0}; // buffer utilizzato per le risposte
    del server
  */

  // Creazione del socket
  serverSocket = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
  if (serverSocket < 0) {
    generazioneErrore("Creazione socket fallita \n");
  }

  // Configurazione di "indirizzo" per poter effettuare il binding correttamente
  indirizzo.sin_family = AF_INET;
  indirizzo.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  indirizzo.sin_port = htons(PORTA);

  // Binding del socket alla porta specificata in "indirizzo"
  if (bind(serverSocket, (struct sockaddr *)&indirizzo, lunghezzaIndirizzo) < 0) {
    generazioneErrore("Errore durante il binding \n");
  }
  listen(serverSocket, 10);

  printf("Menù delle operazione che possono essere richieste dal client: \n"
            "1) Visualizzazzione tutti i record della rubrica \n"
            "2) Ricerca record tramite cognome \n"
            "3) Ricerca record tramite coppia nome-cognome \n"
            "4) Aggiunta Record \n"
            "5) Eliminazione Record \n"
            "6) Modifica Numero di Telefono \n"
            "7) Modifica Indirizzo \n");

  while (1) {

    // Ascolto delle connessioni in entrata (10 è il numero massimo di richieste
    // pendenti)
    

    printf("Server in ascolto sulla porta %d\n", PORTA);
      
    // Accetta una connessione in entrata
    if ((clientSocket = accept(serverSocket, (struct sockaddr *)&indirizzo,
                              (socklen_t *)&lunghezzaIndirizzo)) < 0) {
      generazioneErrore("Errore durante l'accettazione di una richiesta \n");
    }

    printf("Accettata connessione da un client\n");

    // Fase di Sospensione fino a segnale mandato dal client

    /*if(kill(getpid(),SIGSTOP) < 0){
      generazioneErrore("Errore durante la sospensione del server \n");
    }
    sleep(10);
    kill(getpid(),SIGCONT);*/

    // Gestione richiesta tramite fork
    int pid = fork();

    if (pid == 0) {
      printf("Sono un figlio generato per gestire la richiesta appena arrivata\n");

      // Login utente
      int richiesta = 0;
      // si leggono 4 byte = dimensione intero
      int risultato = recv(clientSocket, &richiesta, sizeof(richiesta),0);
      if (risultato < 1) {
        generazioneErrore("Lettura fallita\n");
      }

      printf("Lettura effettuata: %d\n", richiesta);

      switch (richiesta) {

      case VISUALIZZA_OGNI_RECORD:
        printf("Richiesta 1\n");
        break;

      case RICERCA_RECORD_CON_COGNOME:
        printf("Richiesta 2\n");
        break;

      case RICERCA_RECORD_CON_NOME_COGNOME:
        printf("Richiesta 3\n");
        break;

      case AGGIUGI_RECORD:
        printf("Richiesta 4\n");
        break;

      case RIMUOVI_RECORD:
        printf("Richiesta 5\n");
        break;

      case MODIFICA_TELEFONO:
        printf("Richiesta 6\n");
        break;

      case MODIFICA_INDIRIZZO:
        printf("Richiesta 7\n");
        break;

      default:
        generazioneErrore("Richiesta non valida\n");
        break;
      }

          /*
      // Ricezione della richiesta dal client
      int read_val = read(clientSocket, input, dimBuffer);
      if (read_val < 0) {
          perror("read");
          close(clientSocket);
          continue;
      }
      input[read_val] = '\0';

      printf("Richiesta ricevuta: %s\n", input);

      // Parsing della richiesta e gestione delle operazioni
      char* token = strtok(input, " ");
      if (token != NULL && strcmp(token, "ADD") == 0) {
          char* name = strtok(NULL, " ");
          char* phone = strtok(NULL, " ");
          if (name && phone) {
              addContact(name, phone);
              snprintf(output, sizeof(output), "Contatto %s aggiunto
      con numero %s\n", name, phone);
          }
          else {
              snprintf(output, sizeof(output), "Formato richiesta non
      valido\n");
          }
      }
      else if (token != NULL && strcmp(token, "SEARCH") == 0) {
          char* name = strtok(NULL, " ");
          if (name) {
              char* phone = searchContact(name);
              if (phone) {
                  snprintf(output, sizeof(output), "Numero di %s:
      %s\n", name, phone);
              }
              else {
                  snprintf(output, sizeof(output), "Contatto %s non
      trovato\n", name);
              }
          }
          else {
              snprintf(output, sizeof(output), "Formato richiesta non
      valido\n");
          }
      }
      else {
          snprintf(output, sizeof(output), "Operazione non
      riconosciuta\n");
      }

      // Invio della risposta al client
      send(clientSocket, output, strlen(output), 0);
      printf("Risposta inviata: %s\n", output);

      exit(0);*/
    }
    else {
      printf("Sono il processo che continua ad accettare richieste\n");
    }

    // Chiudi la connessione con il client
    logoutUtente(clientSocket);

    printf("La connessione si è conclusa\n");
  }

  return 0;
}

int loginUtente(int clientSocket){ 
    return 0; 
}

void logoutUtente(int clientSocket){
}