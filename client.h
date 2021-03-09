#ifndef CLIENT_H
#define CLIENT_H
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
#define PERMISSOES 0600
#define cprompt_TOK_BUFSIZE 64
#define cprompt_TOK_DELIM " \t\r\n\a"  //os delimitadores dos tokens para o parser
#define MAXPROC 10 /* Número máximo de proc. por sala*/
#define CHUNK 1024

//Definição das variaveis globais
int msgid;
int msgid2;
pid_t c1pid;
pid_t c2pid;
int connect;

// Definiçao das estruturas

//Para guardar o nome dos ficheiros lidos na diretoria onde o cliente corre em memoria
typedef struct node {
    struct node * next;
    char val[];
} node_t;

//mensagem do c2 para enviar na fila 2
struct MsgStruct
{
    long type;
    char nome[256];
    char conteudo[1024];
} message2;

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


// Definiçoes de funçoes

//gerais
void signal_callback_handler(int signum); //lida com sinais

//Funçoes do C2
void list_dir(node_t * head); // le os ficheiros na diretoria
void push(node_t * head, char *val); // guarda o nome dos ficheiros em memoria
int check_file(node_t * head);// verifica a existencia do ficheiros
int read_and_copy(char *filename);//guarda o ficheiro

//Funçoes do prompt
int cprompt_execute(char **args, int status, int argcount);//executa comandos
char *cprompt_read_line(void);//parser
char **cprompt_split_line(char *line, int * argcount);//parser
void cprompt_loop(int c1pid,int c2pid);//prompt

//Funçoes do C1
void help();//mostra comandos possiveis
int ligar();//liga servidor
int pesquisar(char **args);//pesquisa noutros clientes ficheiros
void requesita(char **args);//pede ficheiros
void desligar(char **args);//desliga do servidor
void  ALARMhandler();// função de verificação periodica de envio de mensagens em ambas as filas
#endif

