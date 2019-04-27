#include "hdr.h"

void cd(char *args[]){
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