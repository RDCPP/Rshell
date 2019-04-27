#include "hdr.h"

inline void cd(char *args[]){
    int c = chdir(args[1]);
    if(c < 0){
        if(errno == ENOENT){
            fprintf(stderr,"%s: wrong directory\n",args[1]);
        }
        else{
            fprintf(stderr,"cd Error : %s\n",strerror(errno));
        }
    }
}

int main(void){
    void (*hand) (int);

    hand = signal(SIGINT,SIG_IGN);/* ignore Ctrl+C */
    char *args[MAX_LEN / 2 + 1]; /* command line arguments */
    int should_run = 1;          /* flag to determine when to exit program */
    int background = 0;
    int hisnum;
    
    char *input;
    int status;
    char *home = getenv("HOME");
    chdir(home);

    char **env_key = (char **)malloc(sizeof(char *)*100);
    char **env_value = (char **)malloc(sizeof(char *)*100);
    char **alias_key = (char **)malloc(sizeof(char *)*100);
    char **alias_value = (char **)malloc(sizeof(char *)*100);

    int valid_env[100] = {0,};
    int e = 0;
    int valid_alias[100] = {0,};
    int a = 0;

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
            free(his);
        }

        input[strlen(input) - 1] = '\0';

        if(*input == '\0'){
            goto fin;
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

        if(*args[0] == '$'){
            char *envname = getenv(args[0]+1);
            if(envname != NULL)
                printf("%s\n",envname);
        }

        else if(!strcmp(args[0],"cd")){
            if(args[1] == NULL) chdir(home);
            else cd(args);
        }

        else if(!strcmp(args[0],"pwd")){
            printf("%s\n",path);
        }

        else if(!strcmp(args[0],"clear")){
            system("clear");
        }

        else if(!strcmp(args[0],"whoami")){
            printf("%s\n",username);
        }

        else if(!strcmp(args[0],"hostname")){
            printf("%s\n",hostname);
        }

        // else if(!strcmp(args[0],"export")){
        //     if(args[1] == NULL){
        //         system("export");
        //         for(int i=0;i<100;i++){
        //             if(valid_env[i] == 1){
        //                 printf("%s=%s\n",env_key[i],env_value[i]);
        //             }
        //         }
        //     }
        //     else{
        //         if(!strlen(args[1]) - 1 == '=' && strchr(args[1],'=') != NULL && e<99){
        //             char *key = (char *)malloc(sizeof(char)*100);
        //             char *value = (char *)malloc(sizeof(char)*100);
        //             char *assign = strchr(args[1],'=');
        //             strcpy(key,assign-1);
        //             strcpy(value,assign+1);
        //             env_key[e] = key;
        //             env_value[e] = value;
        //             valid_env[e] = 1;
        //             while(valid_env[e] != 0) e++;
        //         }
        //     }
        // }

        // else if(!strcmp(args[0],"unset")){
        //     if(args[1] != NULL){
        //         for(int i=0;i<99;i++){
        //             if(valid_env[i] == 1){
        //                 if(!strcmp(env_key[i],args[1])){
        //                     free(env_key[i]);
        //                     free(env_value[i]);
        //                     if(i<e) e=i;
        //                     break;
        //                 }
        //             }
        //         }
        //     }
        // }

        // else if(!strcmp(args[0],"alias")){
        //     if(args[1] == NULL){
        //         system("alias");
        //         for(int i=0;i<100;i++){
        //             if(valid_alias[i] == 1){
        //                 printf("%s=%s\n",alias_key[i],alias_value[i]);
        //             }
        //         }
        //     }
        //     else{
        //         if(!strlen(args[1]) - 1 == '=' && strchr(args[1],'=') != NULL && a<99){
        //             char *key = (char *)malloc(sizeof(char)*100);
        //             char *value = (char *)malloc(sizeof(char)*100);
        //             char *assign = strchr(args[1],'=');
        //             strcpy(key,assign-1);
        //             strcpy(value,assign+1);
        //             alias_key[a] = key;
        //             alias_value[a] = value;
        //             valid_alias[a] = 1;
        //             while(valid_alias[a] != 0) a++;
        //         }
        //     }
        // }

        // else if(!strcmp(args[0],"unalias")){
        //     if(args[1] != NULL){
        //         for(int i=0;i<99;i++){
        //             if(valid_alias[a] == 1){
        //                 if(!strcmp(alias_key[i],args[1])){
        //                     free(alias_key[i]);
        //                     free(alias_value[i]);
        //                     if(i<a) a=i;
        //                     break;
        //                 }
        //             }
        //         }
        //     }
        // }

        else if(!strcmp(args[0],"history")){
            off_t cur = lseek(fd,0,SEEK_CUR);
            lseek(fd,0,SEEK_SET);
            off_t size = lseek(fd,0,SEEK_END);
            lseek(fd,0,SEEK_SET);
            char *hisbuf = (char *)malloc(sizeof(char) * size);
            read(fd,hisbuf,(size_t)size);
            char *start = strstr(hisbuf,"1  ");
            if(args[1] != NULL && !strcmp(args[1],"|") && !strcmp(args[2],"grep")){
                if(args[3] == NULL){
                    fprintf(stderr,"grep should have agruments\n");
                    goto fin;
                }
                char *cpybuf = (char *)malloc(sizeof(char) * size);
                strcpy(cpybuf,start);
                char *result = strtok(cpybuf,"\n");
                while(result != NULL){
                    if(strstr(result,args[3]) != NULL){
                        printf("%s\n",result);
                    }
                    result =strtok(NULL,"\n");
                }
            }
            else{
                printf("%s\n",start);
            }
            lseek(fd,cur,SEEK_SET);
            free(hisbuf);
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
                    printf("Make Background Process : %d\n",pid);
                }
            }
        }
        fin:
            free(username);
            free(hostname);
            free(path);
            free(input);
    }
} 