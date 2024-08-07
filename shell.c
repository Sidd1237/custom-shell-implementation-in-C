#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SID_RL_BUFSIZE 1024
#define SID_TOK_BUFSIZE 64
#define SID_TOK_DELIM " \t\r\n\a"

int sid_cd(char **args);
int sid_help(char **args);
int sid_exit(char **args);

char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int sid_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int (*builtin_func[]) (char **) = {
  &sid_cd,
  &sid_help,
  &sid_exit
};

int sid_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "sid: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("sid");
    }
  }
  return 1;
}
int sid_help(char **args)
{
  int i;
  printf("Siddharth Gotur's Shell:\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < sid_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}
int sid_exit(char **args)
{
  return 0;
}


int sid_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid==0){
        //child process
        if(execvp(args[0],args)==-1){
            perror("sid");
        }
        exit(EXIT_FAILURE);
    }else if(pid<0){
        perror("sid")
    } else{
        dp{
            wpid = waitpid(pid,&status,WUNTRACED);
        } while(!WIFEXITED(status)&&!WIFSIGNALED(Sstatus));
    }
    return 1;
}

int sid_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < sid_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return sid_launch(args);
}
char** sid_split_line(char *line){
    int bufsize = SID_TOK_BUFSIZE, position = 0;
    char **tokens = (char**)malloc(bufsize*sizeof(char));
    char *token;

    if(!token){
        fprintf(stderr,"sid: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SID_TOK_DELIM);
    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position>=bufsize){
            bufsize+=SID_TOK_BUFSIZE;
            tokens = (char**)realloc(tokens,bufsize*sizeof(char*));
            if(!tokens){
                fprintf(stderr,"sid: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SID_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
char *sid_read_line(void){
    int bufsize = SID_RL_BUFSIZE;
    int position = 0;
    char *buffer = (char*)malloc(sizeof(char)*bufsize);
    int c;

    if(!buffer){
        fprintf(stderr, "sid: allocation error\n");
        exit(EXIT_FAILURE);
    }
    while(1){
        c = getchar();
        
        if(c == EOF || c=='\n'){
            buffer[position] = '0';
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;

        //reallocating if surpasses buffer size
        if(position>=bufsize){
            bufsize += SID_RL_BUFSIZE;
            buffer = (char*)realloc(buffer,bufsize);
            if(!buffer){
                fprintf(stderr,"sid: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
void sid_loop(void){
    char *line;
    char **args;
    int status;

    do{
        printf("> ");
        line = sid_read_line();
        args = sid_split_line(line);
        status = sid_execute(args);

        free(line);
        free(args);
    } while (status);
}
int main(int argc, char const *argv[])
{
    //looping interpreting commands
    sid_loop();
    return EXIT_SUCCESS;
}


