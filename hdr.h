#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_LEN 100 /* The maximum length command */

/* print color */
#define YEL   "\x1B[33m" 
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

void cd(char *args[]);
void make_path(const char *path);