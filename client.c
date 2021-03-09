#include <client.h>

int check_file(node_t * head) {
    int check=1;
    node_t * current = head;
    while (current != NULL) {
        if(strcmp (message.mesg_text, current->val) == 0){check=0;}
        current = current->next;
    }
    return(check);
}

void push(node_t * head, char *val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(node_t));
    strcpy(current->next->val,val);
    current->next->next = NULL;
}

void list_dir(node_t * head)
{
    int i=0;
    char *dir=".";
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            if(strcmp(".",entry->d_name) == 0 ||
                    strcmp("..",entry->d_name) == 0)
                continue;
        }
        else if(i<1){
            strcpy(head->val,entry->d_name);
            head->next = NULL;
        }
        else if(i<=20) {
            push(head,entry->d_name);
        }
        i++;
    }
    closedir(dp);
}

void signal_callback_handler(int signum)
{
    char *argst[]={"desligar","desligar"};
    if(getpid()==c1pid){
        printf("PARENT ACTIVATED HANDLER: Caught signal %d\n",signum);
        if(connect==2)desligar(argst);
        else printf("Not connected...not informing server\n");
    }
    else{
        printf("CHILD ACTIVATED HANDLER: Caught signal %d\n",signum);
    }
    exit(signum);
}

//shell
int cprompt_execute(char **args, int status, int argcount)
{
    if ( argcount == 3 && (strcmp ("requesita", args[0]) == 0))
    {
        if(status==2){
            requesita(args);
        }
        else{
            printf("NOT Connected!\n");
        }
    }
    else if ( argcount == 2 && (strcmp ("pesquisar", args[0]) == 0))
    {
        if(status==2){

            //status += pesquisar(args);
            pesquisar(args);
        }
        else{
            printf("NOT Connected!\n");
        }
    }
    else if ( argcount == 1 && (strcmp ("ligar", args[0]) == 0))
    {
        if(status==1){
            status += ligar();
        }
        else{
            printf("Already Connected!\n");
        }
    }
    else if ( argcount == 1 && (strcmp ("desligar", args[0]) == 0))
    {
        if(status==2){
            printf("informing server of exit...!\n");
            desligar(args);
        }
        else{
            printf("No need to inform server...!\n");
        }
        status=0;
    }
    else if ( argcount == 1 && (strcmp ("help", args[0]) == 0))
    {
        help();
    }
    else if ( argcount == 1 && (strcmp ("clear", args[0]) == 0))
    {
        system("clear");
    }
    else if (args[0] == NULL) {
        printf("no command entered!\n");
    }
    else
    {
        printf("unknown command ! try <help> \n");
    }
    return(status);
}

char *cprompt_read_line(void)
{
    char *line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

char **cprompt_split_line(char *line, int * argcount)
{
    int bufsize = cprompt_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;

    if (!tokens) {
        fprintf(stderr, "cprompt: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, cprompt_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += cprompt_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "cprompt: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, cprompt_TOK_DELIM);
    }
    tokens[position] = NULL;
    *argcount=position;
    return tokens;
}

void cprompt_loop(int c1pid, int c2pid)
{
    char *line;
    char **args;
    int status=1;
    int argcount=0;
    connect=status;
    do {
        printf("(C1=%d)(C2=%d) - client > ",c1pid,c2pid);
        line = cprompt_read_line();
        args = cprompt_split_line(line,&argcount);
        status=cprompt_execute(args,status,argcount);
        free(line);
        free(args);
        connect=status;
    } while (status==1 || status==2);
}

void help(){
    char prompt[] = "------------------------Help-------------------------------------------\n“ligar”                 	   |  liga-se ao servidor\n “desligar”                     |  desliga-se do servidor e termina o programa \n“pesquisar <conteudo>”         |  pesquisa por ficheiros noutros clientes\n “requisita <conteudo> <PID> ”  |  pede ficheiros de outros clientes\n “help”                         |  descricçao dos comandos\n “clear”                        |  limpa ecra \n__________________________________________________________________________\n";
    printf("%s",prompt);
}

int ligar(){
    int r=-1;
    int loader=0;
    //variaveis fila 1
    key_t key;
    key=10;
    msgid = msgget(key, 0); // msgget conecta-se a uma fila ja criada e retorna identificador
    if(msgid==-1){
        printf("Cant Connect... Server offline?\n");
        return(0);
    }
    msgid2 = msgget(11, 0);
    if(msgid2==-1){
        printf("Cant Connect... Server offline?\n");
        return(0);
    }
    else{
        message.mesg_type = 1;
        strcpy(message.mesg_text,"ligar");
        message.c1pid=c1pid;
        message.c2pid=c2pid;
        // msgsnd envia msg
        msgsnd(msgid, &message, sizeof(message), 0);

        // mostra msg
        printf("Connecting...\n");
        printf("Data send type : %ld \n", message.mesg_type);
        printf("Data send text : %s \n", message.mesg_text);
        printf("Data send c1pid : %d \n", message.c1pid);
        printf("Data send c2pid : %d \n", message.c2pid);

        //espera que o servidor permita conectar
        while(loader<5){
            r=msgrcv(msgid, &message, sizeof(message), getpid(), IPC_NOWAIT);
            if(r==-1){
                printf("Trying to connect...\n");
            }
            else{
                if(message.reply==3){
                    printf("connected Successfully...\n");
                    return 1;
                }
                else{
                    printf("ERROR TRYING TO CONNECT...\n");
                    return 0;
                }
            }
            loader++;
            sleep(1);
        }
        printf("\nUNABLE TO CONNECT - SERVER MIGHT BE FULL\n");
        return 0;
    }
}


int pesquisar(char **args){
    key_t key_memory = 10;
    int shmid = shmget(key_memory,1024,0);//smhflag == 0 -> apenas para conectar, shmget devolve identificador
    if(shmid==-1){
        printf("Cant connect... is the server offline?\n");
        printf("you have been disconnected...you need to 'ligar' again...\n");
        return(shmid);
    }
    else{
        /* acoplamento */
        ptr= (struct s_shm *) shmat(shmid,NULL,0);

        int sent=0;
        strcpy(message.mesg_text,args[1]);
        message.c1pid=c1pid;
        message.c2pid=c2pid;
        message.reply=0;
        for(int i=0;i<MAXPROC;i++){
            if (ptr->tabela[i]!= c2pid && ptr->tabela[i]!= 0)
            {
                message.mesg_type = ptr->tabela[i] ;
                // msgsnd envia mensagem
                msgsnd(msgid, &message, sizeof(message), 0);
                // mostra mensagem
                printf("A pesquisar...\n");
                printf("Data send type : %ld \n", message.mesg_type);
                printf("Data send text : %s \n", message.mesg_text);
                printf("Data send c1pid : %d \n", message.c1pid);
                printf("Data send c2pid : %d \n", message.c2pid);
                sent++;
            }
        }
        if(sent==0){
            printf("Nothing was sent...  seems there are no other clients online?\n");
        }
        else{
            printf("Sent message to a total of %d clients\n",sent);
        }
        shmdt(ptr);//liberta-se da shared memory
    }
    return(0);
}

void requesita(char **args){
    message.mesg_type = atoi(args[2]);
    strcpy(message.mesg_text,args[1]);
    message.c1pid=c1pid;
    message.c2pid=atoi(args[2]);
    message.reply=1;
    // msgsnd to send message
    msgsnd(msgid, &message, sizeof(message), 0);

    // display the message
    printf("A requesitar...\n");
    printf("Data send type : %ld \n", message.mesg_type);
    printf("Data send text : %s \n", message.mesg_text);
    printf("Data send c1pid : %d \n", message.c1pid);
    printf("Data send c2pid : %d \n", message.c2pid);

}

// definimos "desligar" quando c1pid =0;
void desligar(char **args){
    message.mesg_type = 1;
    strcpy(message.mesg_text,args[0]);
    message.c1pid=0;
    message.c2pid=c2pid;

    // msgsnd envia mensagem
    msgsnd(msgid, &message, sizeof(message), 0);

    // mostra mensagem
    printf("A desligar...\n");
    printf("Data send type : %ld \n", message.mesg_type);
    printf("Data send text : %s \n", message.mesg_text);
    printf("Data send c1pid : %d \n", message.c1pid);
    printf("Data send c2pid : %d \n", message.c2pid);

}

int read_and_copy(char *filename){
    FILE *pFile;
    long lSize;
    char *buffer;
    size_t result;
    //abre ficheiro
    pFile = fopen(filename, "r");
    if (pFile==NULL){
        fputs("File error\n", stderr);
        return 1;
    }
    // obtem tamanho do ficheiro
    fseek(pFile, 0, SEEK_END); // procura o fim do ficheiro
    lSize = ftell(pFile);  // obtem o ponteiro atual para o ficheiro
    rewind(pFile);  // volta para o inicio do ficheiro
    //se o ficheiro é demasiado grande , diminuimos lsize para estar de acordo com o tamanho maximo do conteudo da mensagem
    if(lSize>CHUNK){lSize=CHUNK;}
    // aloca memoria para guardar o ficheiro
    // adiciona-mos mais um byte por causa do char NULL para terminar
    buffer = (char *)malloc(sizeof(char)*lSize + 1);
    if (buffer == NULL) {
        fputs("Memory error", stderr);
        return 1;
    }
    // colocamos a memoria a zero antes de copiar.
    //  lSize + 1 byte = 0 == NULL '\0'
    // limpamos a memoria e adicionamos o NULL ao mesmo tempo
    memset(buffer, 0, sizeof(char)*lSize + 1);
    // copiamos para o buffer:
    result = fread(buffer, sizeof(char), lSize, pFile);
    if ((unsigned)result != lSize) {
        fputs("Reading error", stderr);
        return 1;
    }
    // copiamos para o buffer para o conteudo.
    strcpy(message2.conteudo,buffer);//copy ...
    //libertamos ponteiros e memoria alocada
    fclose(pFile);
    free(buffer);
    return 0;
}

void  ALARMhandler(){
    for(int i=0; i<MAXPROC;i++){
        int r=msgrcv(msgid, &message, sizeof(message), getpid(), IPC_NOWAIT);
        if(!(r==-1)){printf("\nRESPOSTA %s %d\n",message.mesg_text,message.c2pid);}
        int r2=msgrcv(msgid2, &message2, sizeof(message2), getpid(), IPC_NOWAIT);
        if(!(r2==-1)){
            printf("\n\nFicheiro recebido:\n");
            printf("\n%s\n",message2.conteudo);
        }
    }
    alarm(30);
}


