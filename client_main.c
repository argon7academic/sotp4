#include <client.h>
#include <client.c>

int main(){
    signal(SIGINT, signal_callback_handler);
    signal(SIGALRM, ALARMhandler);
    alarm(1);
    int status;
    c1pid= getpid();
    c2pid=fork();

    if(!c2pid) //filho
    {
        //FILE * fp; // to open files
        //arms signal SIGUSR1
        signal(SIGUSR1, signal_callback_handler);
        // allocate a memory block to save a list of the files in the directory
        node_t * head = NULL;
        head = malloc(sizeof(node_t));
        if (head == NULL) {
            printf("error\n");
            return 1;
        }
        //reads directory and saves a list of the files names to memory(in a linked list)
        list_dir(head);

        key_t key,key2;
        int r;
        key=10;
        key2=11;
        do{// connects to message queque
            // and returns identifier
            msgid = msgget(key, 0);
            msgid2 = msgget(key2, 0);
        }while(msgid==-1 || msgid2==-1);
        while (1){
            //
            // msgrcv to receive message
            r=msgrcv(msgid, &message, sizeof(message), getpid(), IPC_NOWAIT);
            if(!(r==-1))
            {
                printf("Message received!\n");
                if(message.reply==0){
                    printf("Pedido de pesquisa recebido!\n");
                    if(!check_file(head))
                    {
                        printf("PESQUISA---File HIT!\n");
                        // message resposta a confirmar que tem pedido
                        message.mesg_type = c1pid;
                        //strcpy(message.mesg_text,"Resposta");
                        message.c1pid=c1pid;
                        message.c2pid=getpid();
                        message.reply=0;
                        msgsnd(msgid, &message, sizeof(message), 0);
                        printf("Resposta enviada %s %d!\n", message.mesg_text,message.c2pid);
                    }
                    else printf("PESQUISA---File MISS!\n");
                }
                if(message.reply==1){
                    printf("Pedido de requesição recebido!\n");
                    if(!check_file(head))
                    {
                        printf("REQUISICAO---File HIT!\n");

                        if(read_and_copy(message.mesg_text)== 0){
                            printf("FILE WAS READ AND COPIED \n");
                        }
                        //serve o pedido na fila 2
                        message2.type = message.c1pid;
                        strcpy(message2.nome,message.mesg_text);
                        // msgsnd to send message
                        if(msgsnd(msgid2, &message2, sizeof(message2), 0)!=-1){

                            printf("Requesição enviada para PID=%ld..\n", message2.type);
                        }
                        else{
                            printf("ERRO A ENVIAR FICHEIRO\n");
                        }

                    }
                    else printf("REQUISICAO---File MISS!\n");
                }
            }
        };
    }
    else//pai
    {

        cprompt_loop(c1pid,c2pid);
        kill(c2pid,SIGUSR1); //sends SIGUSR1 signal
        //wait(&status): waits for any process to end and stores status information in the variable
        // to which it points(int status); wait() itself returns the process id of the child once it terminates
        // or -1 if it fails (there is no child)
        while(wait(&status)>0) //while return value isnt -1 (indicating there are no childs) runs following code
        {
            if (WIFEXITED(status)) //Verifies if child process terminated normally
            {
                //if SIGUSR1 is receveid by the handler
                printf("PARENT: CHILD TERMINATED NORMALLY\n");
            }
            else if (WIFSIGNALED(status)) // if it didnt terminate normally verifies for a signal
            {
                //if SIGUSR1 is NOT receveid by the handler
                psignal(WTERMSIG(status), "PARENT:EXIT SIGNAL FROM CHILD "); // prints the signal number that caused the child to terminate
            }
        }
    }
    printf("Parent Terminating\n");
    return (0);
}
