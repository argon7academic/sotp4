#include <server.h>
void display_received(){
    // display the message
    printf("Data Received is type : %ld \n", message.mesg_type);
    printf("Data Received is text : %s \n", message.mesg_text);
    printf("Data Received is c1pid : %d \n", message.c1pid);
    printf("Data Received is c2pid : %d \n", message.c2pid);
    printf("________________________________________________________________\n");
}
void cleanup(){
    int shmid=10;
    int msgid=10;
    int msgid2=11;
    //cleanup before terminating
    shmdt(ptr);//detach from shared memory
    shmctl(shmid,IPC_RMID,NULL);// destroy the shared memory
    msgctl(msgid, IPC_RMID, NULL); // to destroy the message queue FILA 1
    msgctl(msgid2, IPC_RMID, NULL); // to destroy the message queue FILA 2
}
void signal_callback_handler(int signum)
{
    printf("SERVER: Caught signal %d\n",signum);
    // Cleanup and close up stuff here
    cleanup();
    // Terminate program
    exit(signum);
}
