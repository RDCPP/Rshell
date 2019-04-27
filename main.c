#include "hdr.h"

int main(void){
    void (*hand) (int);

    hand = signal(SIGINT,SIG_IGN);/* ignore Ctrl+C */
    char *args[MAX_LEN / 2 + 1]; /* command line arguments */
    int should_run = 1;          /* flag to determine when to exit program */
    int background = 0;
    int execmode = 0;
    int hisnum;
    
    char *input;
    int status;
    char *home = getenv("HOME");
    chdir(home);

    int fd = open("./.rshellhistory",O_CREAT|O_RDWR|O_EXCL,0644);
    if(fd == -1){
        fd = open("./.rshellhistory",O_RDWR,0644);
        char hbuf[50];
        char* hn;
        read(fd,hbuf,sizeof(hbuf));
        hn = strtok(hbuf,"\n");
        hisnum = atoi(hn+9);
        lseek(fd,0,SEEK_END);
    }
    else{
        char *hisinfo = "hisnum : 1                 \n--------------------\n";
        hisnum = 1;
        write(fd,hisinfo,strlen(hisinfo));
    }

    while(should_run){
        char *username = (char *)malloc(sizeof(char)*50);
        char *hostname = (char *)malloc(sizeof(char)*50);
        char *path = (char *)malloc(sizeof(char)*255);
        cuserid(username);
        gethostname(hostname,49);
        getcwd(path,255);

        printf(CYN "[%s@%s" YEL " %s" CYN "]" RESET " $ ",username,hostname,path);
        fflush(stdout);

        input = (char *)malloc(MAX_LEN*sizeof(char));
        memset(input,0,MAX_LEN / 2 + 1);
        fgets(input, MAX_LEN, stdin);

        if(*input != '\n'){
            char num[30];
            char *his =(char *)malloc((MAX_LEN*2)*sizeof(char));
            sprintf(num,"%d",hisnum++);
            strcpy(his,num);
            strcat(his,"  ");
            strcat(his,input);
            write(fd,his,strlen(his));
        }

        input[strlen(input) - 1] = '\0';

        if(*input == '\0'){
            free(username);
            free(hostname);
            free(path);
            free(input);
            continue;
        }

        int i = 0;

        args[i] = strtok(input," ");
        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL," ");
        }

        if (strcmp(args[i-1], "&") == 0) {
            args[i - 1] = NULL;
            background = 1;
        }

        if(!strcmp(args[0],"cd")){
            if(args[1] == NULL) chdir(home);
            else cd(args);
        }

        else if(!strcmp(args[0],"clear")){
            system("clear");
        }

        else if(!strcmp(args[0],"history")){
            off_t cur = lseek(fd,0,SEEK_CUR);
            lseek(fd,0,SEEK_SET);
            off_t size = lseek(fd,0,SEEK_END);
            lseek(fd,0,SEEK_SET);
            char *hisbuf = (char *)malloc(sizeof(char) * size);
            read(fd,hisbuf,(size_t)size);
            char *start = strstr(hisbuf,"1  ");
            printf("%s\n",start);
            lseek(fd,cur,SEEK_SET);
        }

        else if(!strcmp(args[0],"exit")){
            lseek(fd,9,SEEK_SET);
            char num[30];
            sprintf(num,"%d",hisnum);
            write(fd,num,strlen(num));
            close(fd);
            exit(0);
        }

        else{
            pid_t pid = fork();

            if (pid < 0) {
                perror("Fork error");
                exit(0);
            }

            if (pid == 0) {
                execvp(args[0],args);
            }

            if (pid > 0){
                if(!background){
                    waitpid(pid,&status,0);
                }
                else{
                    background = 0;
                }
            }
        }
    }
} 