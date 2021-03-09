#ifndef SERVER_H
#define SERVER_H
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/shm.h>
#define exit_on_error(s,m) if (s < 0) { perror(m); exit(1); }
#define PERMISSOES 0666 /* permissões para outros utilizadores */
#define MAXPROC 10 /* Número máximo de proc. por sala*/

// Definiçoes das estruturas

//mensagens da fila 1
struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
    pid_t c1pid;
    pid_t c2pid;
    int reply;
} message;

//ponteiro para aceder ao segmento de memoria partilhada
struct s_shm *ptr;
/* Tab. de registo de utilizadores em cada sala */
int tsala[MAXPROC];
/*Organização da memória partilhada*/
struct s_shm{
    int tabela[MAXPROC];
};

//Funçoes
void signal_callback_handler(int signum);//lida com sinais
void display_received();//mostra o recebido
void cleanup();//operaçoes de limpeza de memoria, destruir filas e memorias partilhadas
#endif

