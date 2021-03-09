#include "server.h"
#include "server.c"
int main()
{
    int r,fstatus;
    signal(SIGINT, signal_callback_handler);
    key_t key,key2;
    int msgid,msgid2;
    key=10;
    key2=11;
    int status=1;
    // msgget creates a message queue FILA 1
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    //  FILA 2
    msgid2 = msgget(key2, 0666 | IPC_CREAT);

    key_t key_memory = 10;
    // shmget returns an identifier in shmid
    int shmid = shmget(key_memory,1024,0666|IPC_CREAT);
    /* acoplamento */
    ptr= (struct s_shm *) shmat(shmid,NULL,0);
    /* escrita na memória */
    for(int i=0;i<=MAXPROC;i++)
        ptr->tabela[i]=0;
    printf(" Server PID %d -  File 1 ID %d - 'q' to exit & end all connections\n ",getpid(),msgid);
    if(!fork())
    {
        char str1[20];
        while(1){
            scanf("%s", str1);
            if(strcmp ("q", str1) == 0)
            {
                return 0;
            }
        }
    }
    else
    {//"do" enquanto status deixar...
        do {//confirma a ver se o filho terminou periodicamente e se terminou fecha o programa
A:
            if (waitpid(-1, &fstatus, WNOHANG)>0) break;//se o filho nao terminou continua
            // msgrcv to receive message
            r=msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT);
            if(r==-1){//se nao ha mensagem...volta a confirmar se o filho terminou ou se ja ha mensagem
                goto A;
            }
            //no caso de haver mensagem
            //verifica se o utilizador ja esta registado, senao tiver regista-o
            if(message.mesg_type==1)
            {
                for(int i=0;i<MAXPROC;i++)
                {
                    if(ptr->tabela[i]==message.c2pid)
                    {
                        break;
                    }
                    else if(ptr->tabela[i]==0)
                    {
                        ptr->tabela[i]=message.c2pid;
                        printf("Aproved conection - sent reply to client\n");
                        //Aprova entrada do cliente
                        message.mesg_type = message.c1pid;
                        strcpy(message.mesg_text,"ligar");
                        message.reply=3;
                        // msgsnd to send message
                        msgsnd(msgid, &message, sizeof(message), 0);
                        break;
                    }
                }
            }
            // verifica se o processo quer desligar
            if (message.c1pid == 0 && (strcmp ("desligar", message.mesg_text) == 0)){
                for(int i=0;i<MAXPROC;i++)
                {
                    if(ptr->tabela[i]==message.c2pid)
                    {
                        ptr->tabela[i]=0;
                        //break;
                    }
                }

            }
            // print do que se passa atualmente
            for(int i=0;i<MAXPROC;i++)
                printf("%d ", ptr->tabela[i]);
            printf("\n");
            display_received();

        } while (status);


    }
    if (WIFEXITED(fstatus)) //Verifies if child process terminated normally
    {
        //if SIGUSR1 is receveid by the handler
        printf("SERVER: CHILD TERMINATED NORMALLY\n");
    }
    else if (WIFSIGNALED(fstatus)) // if it didnt terminate normally verifies for a signal
    {
        //if SIGUSR1 is NOT receveid by the handler
        psignal(WTERMSIG(fstatus), "PARENT:EXIT SIGNAL FROM CHILD "); // prints the signal number that caused the child to terminate
    }
    //cleanup before terminating
    shmdt(ptr);//detach from shared memory
    shmctl(shmid,IPC_RMID,NULL);// destroy the shared memory
    msgctl(msgid, IPC_RMID, NULL); // to destroy the message queue FILA 1
    msgctl(msgid2, IPC_RMID, NULL); // to destroy the message queue FILA 2

    return 0;
}
