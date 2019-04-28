#include "hdr.h"

inline void cd(char *args[]){ /* make change directory function inline. */
    int c = chdir(args[1]);
    if(c < 0){
        if(errno == ENOENT){ /* ENOENT -> input directory name is not existed. */
            fprintf(stderr,"%s: wrong directory\n",args[1]);
        }
        else{ /* Other errors; it rarely exists. */
            fprintf(stderr,"cd ERROR : %s\n",strerror(errno));
        }
    }
}

int main(void){
    void (*hand) (int);

    hand = signal(SIGINT,SIG_IGN);/* ignore Ctrl+C */
    char *args[MAX_LEN / 2 + 1]; /* command line arguments */
    int should_run = 1;          /* flag to determine when to exit program */
    int background = 0;          /* background flag */
    int pipeline = 0;            /* pipeline flag */
    int redirection = 0;         /* redirection flag */
    int hisnum;                  /* using history num */
    
    char *input;                 /* input data */
    int status;                  /* status code to use waitpid */
    char *home = getenv("HOME"); /* get HOME directory */
    chdir(home);                 /* my terminal default : home directory starts. */

    // char **env_key = (char **)malloc(sizeof(char *)*100);
    // char **env_value = (char **)malloc(sizeof(char *)*100);
    // char **alias_key = (char **)malloc(sizeof(char *)*100);
    // char **alias_value = (char **)malloc(sizeof(char *)*100);

    // int valid_env[100] = {0,};
    // int e = 0;
    // int valid_alias[100] = {0,};
    // int a = 0;

    /* 원래는 환경변수와 alias를 위해 할당한 포인터들이었는데, 구현이 꼬이면서 일단 주석처리함 */

    /* for history
       1. first we open about history file to use O_EXCL option.
       2. if fd == -1, it means file already exists.
       3. we saved history number on top, and we can loads.
       4. if fd != -1, it means we created files.
       5. First time, we should archive history number part. it is else part.*/
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

    while(should_run){ /* shell should runs */
        char *username = (char *)malloc(sizeof(char)*50);
        char *hostname = (char *)malloc(sizeof(char)*50);
        char *path = (char *)malloc(sizeof(char)*255);
        /* allocate username,hostname,path */
        cuserid(username); /* get username */
        gethostname(hostname,49); /* get host name */
        getcwd(path,255); /* get current path */

        printf(CYN "[%s@%s" YEL " %s" CYN "]" RESET " $ ",username,hostname,path);
        /* username, hostname, box : CYAN color, path : YELLOW color , else : default color */
        fflush(stdout);

        input = (char *)malloc(MAX_LEN*sizeof(char));
        memset(input,0,MAX_LEN / 2 + 1);
        fgets(input, MAX_LEN, stdin);

        /* to input cammand part*/

        if(*input != '\n'){ /* to record history, except empty case */
            char num[30];
            char *his =(char *)malloc((MAX_LEN*2)*sizeof(char));
            sprintf(num,"%d",hisnum++);
            strcpy(his,num);
            strcat(his,"  ");
            strcat(his,input);
            write(fd,his,strlen(his));
            free(his);
        }

        input[strlen(input) - 1] = '\0'; /* fgets also gets \n, we change it. */

        char *cpy;

        if(strchr(input,'|')){
            pipeline = 1;
            cpy = strdup(input);
        }
        if(strchr(input,'>') || strchr(input,'<')){
            redirection = 1;
            cpy = strdup(input);
        }

        if(*input == '\0'){ /* empty case : goto end of while */
            goto fin;
        }

        int i = 0; /* using strtok, we properly execute commands */

        args[i] = strtok(input," ");
        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL," ");
        }

        if (strcmp(args[i-1], "&") == 0) { /* background cases */
            args[i - 1] = NULL;
            background = 1;
        }

        /* env variables can get, but i don't know how I can get declared myself. */

        if(*args[0] == '$'){ 
            char *envname = getenv(args[0]+1);
            if(envname != NULL)
                printf("%s\n",envname);
        }

        /* cd case : no extra arguments go home directory */

        else if(!strcmp(args[0],"cd")){
            if(args[1] == NULL) chdir(home);
            else cd(args);
        }

        /* pwd : current path gives */

        else if(!strcmp(args[0],"pwd")){
            char *p = (char *)malloc(sizeof(char)*255);
            getcwd(p,255);
            printf("%s\n",p);
            free(p);
        }

        /* clear : make our shell interface clean */

        else if(!strcmp(args[0],"clear")){
            system("clear");
        }

        /* whoami : returns username */

        else if(!strcmp(args[0],"whoami")){
            char *u = (char *)malloc(sizeof(char)*50);
            cuserid(u);
            printf("%s\n",u);
            free(u);
        }

        /* hostname : returns hostname */

        else if(!strcmp(args[0],"hostname")){
            char *h = (char *)malloc(sizeof(char)*50);
            gethostname(h,49);
            printf("%s\n",h);
            free(h);
        }

        /* 이 부분은 환경변수와 alias 할당을 위한 구현단이었음. 구현의 난항으로 인해 일단 주석처리함 */

        // else if(!strcmp(args[0],"export")){
               /* 이 부분은 export나 alias가 추가적인 args가 없으면, 현재 목록을 띄우도록 구현한 것 */
        //     if(args[1] == NULL){ 
        //         system("export");
        //         for(int i=0;i<100;i++){
        //             if(valid_env[i] == 1){
        //                 printf("%s=%s\n",env_key[i],env_value[i]);
        //             }
        //         }
        //     }
               /* 이 부분은 추가적인 args를 받았을 때의 행동. 새로이 할당하여 = 앞뒤로 잘라서 key와 value에 넣고자 함 */
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
        //             while(valid_env[e] != 0) e++; /* 다음에 할당받을 가장 작은 값을 e로 함.
        //         }
        //     }
        // }

        // else if(!strcmp(args[0],"unset")){
        //     if(args[1] != NULL){
        //         for(int i=0;i<99;i++){
        //             if(valid_env[i] == 1){
        //                 if(!strcmp(env_key[i],args[1])){ /* 존재하면 삭제 */
        //                     free(env_key[i]);
        //                     free(env_value[i]);
        //                     if(i<e) e=i; /* 할당시 행동과 맞춰주기 위해 e를 세팅한다 */
        //                     break;
        //                 }
        //             }
        //         }
        //     }
        // }
           /* alias 또한 export와 거의 동일한 할당 제거 방식을 사용 */
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

        else if(!strcmp(args[0],"hello")){ /* hello : custom command - simply make Hello World! code.*/
            int hfd = open("./hello.c",O_CREAT|O_RDWR,0644);
            char *code = "#include <stdio.h>\n\nint main(void){\n\nprintf(\"Hello World!\\n\");\nreturn 0;\n}";
            write(hfd,code,strlen(code));
            if(args[1] != NULL){ /* 2 options. -c : gcc complie. -e : execute */
                if(!strcmp(args[1],"-c") || !strcmp(args[1],"-e")){
                    system("gcc -o hello hello.c");
                }
                if(!strcmp(args[1],"-e")){
                    system("./hello");
                }
            }
            close(hfd);
        }

        else if(!strcmp(args[0],"funfun")){ /* funfun : custom command - to appreciate */
            printf("                                                            \n");
            printf(" _____  _                    _      __   __               _ \n");
            usleep(500000);
            printf("|_   _|| |                  | |     \\ \\ / /              | |\n");
            usleep(500000);
            printf("  | |  | |__    __ _  _ __  | | __   \\ V /   ___   _   _ | |\n");
            usleep(500000);
            printf("  | |  | '_ \\  / _` || '_ \\ | |/ /    \\ /   / _ \\ | | | || |\n");
            usleep(500000);
            printf("  | |  | | | || (_| || | | ||   <     | |  | (_) || |_| ||_|\n");
            usleep(500000);
            printf("  \\_/  |_| |_| \\__,_||_| |_||_|\\_\\    \\_/   \\___/  \\__,_|(_)\n");
            usleep(500000);
            printf("                                                            \n");
        }

        else if(!strcmp(args[0],"history")){ /* history command */
            off_t cur = lseek(fd,0,SEEK_CUR); /* after finished, we must return our cursor */
            lseek(fd,0,SEEK_SET);
            off_t size = lseek(fd,0,SEEK_END);
            lseek(fd,0,SEEK_SET); /*to get filesize */
            char *hisbuf = (char *)malloc(sizeof(char) * size); /* make hisbuf : buf size : size of files */
            read(fd,hisbuf,(size_t)size);
            char *start = strstr(hisbuf,"1  "); /* we only use after history number part */
            if(args[1] != NULL && !strcmp(args[1],"|") && !strcmp(args[2],"grep")){ /* this is simply implemented grep, not perfect code. */
                if(args[3] == NULL){
                    fprintf(stderr,"grep should have more agruments\n");
                    free(hisbuf);
                    goto fin;
                }
                char *cpybuf = (char *)malloc(sizeof(char) * size);
                strcpy(cpybuf,start); /* to using grep easily, copy to start point */
                char *result = strtok(cpybuf,"\n"); /* line by line reading */
                while(result != NULL){
                    if(strstr(result,args[3]) != NULL){
                        printf("%s\n",result);
                    }
                    result =strtok(NULL,"\n");
                }
                free(cpybuf);
            }
            else{ /* no grep : all print */
                printf("%s\n",start);
            }
            lseek(fd,cur,SEEK_SET);
            free(hisbuf);
        }

        else if(!strcmp(args[0],"exit")){ /* exit : close shell, only this command exit (not Ctrl + C) */
            lseek(fd,9,SEEK_SET);
            char num[30];
            sprintf(num,"%d",hisnum);
            write(fd,num,strlen(num));
            close(fd);
            exit(0);
        }

        else if(pipeline || redirection){ /* pipeline & redirection give up, just use system */
            system(cpy);
            free(cpy);
        }

        else{ /* extra command, not core */
            pid_t pid = fork(); /* main shell is still alive; make child and execute command there */

            if (pid < 0) {
                perror("Fork error");
                exit(0);
            }

            if (pid == 0) { /* fork returns 0 : child -> execvp */
                int ecode = execvp(args[0],args);
                if(ecode<0){
                    fprintf(stderr,"exec ERROR : %s\n",strerror(errno));
                }
            }

            if (pid > 0){ /* fork returns child pid : parents */
                if(!background){
                    waitpid(pid,&status,0);
                }
                else{ /* here is background case */
                    background = 0;
                    printf("Make Background Process : %d\n",pid);
                }
            }
        }
        fin: /* one cycle is finished; we free what we use by malloc */
            free(username);
            free(hostname);
            free(path);
            free(input);
    }
} 