#include "types.h"
#include "fs.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Tokenize the the string
void strtok(char*str,char delim,char* tok, int* ptr){
    //Set the token initial char to endOfString
    tok[0] = '\0';
    int initVal = *ptr;
    //While the string is not at the delimiter
    // and the end of string is not reached,
    // set the token to the string's characters
    while(str[*ptr] != delim && str[*ptr] != '\0'){
        tok[*ptr-initVal] = str[*ptr];
        if(str[*ptr] != '\0'){
            *ptr = *ptr + 1;
        }
    }
    // Add endOfString to the token
    // and if string is not at the end, increment the pointer
    tok[*ptr-initVal] = '\0';
    if(str[*ptr] != '\0')
        *ptr = *ptr + 1;
    return;
}

void runCMD(char* buf){

    char * args[10];
    int ptr = 0;
    char arg0[100] ="",arg1[100] = "";

    // Prefix a slash to specify the root directory
    //  when executing a command
    
    arg0[0] = '/';

    //Get first command token for arg0
    strtok(buf,' ',arg0+1,&ptr);
    
    //Skip whitespace
    while(buf[ptr] == ' ')
        ptr++;
    
    //Set arg1 to the remaining string
    // after the first space
    int curPtr = ptr;
    while(ptr < strlen(buf)){
        arg1[ptr-curPtr] = buf[ptr];
        ptr = ptr+1;
    }
    arg1[ptr-curPtr] = '\0';
    //Replace ending \n with endOfString char
    if(arg0[strlen(arg0)-1]=='\n')
        arg0[strlen(arg0)-1] = '\0';
    //if(arg1[strlen(arg1)-1]=='\n')
    //    arg1[strlen(arg1)-1] = '\0';

    ptr = 0;
    //if is currently a test for pipe
    if (strchr(buf,'>') != 0) {
        char temp[100]="",file[100]="";
        int fd;
        
        strtok(buf,'>',temp,&ptr);
        if(buf[ptr] == '>')
            ptr++;
        while(buf[ptr] == ' ')
            ptr++;
        strtok(buf,'>',file,&ptr);
        strcpy(buf,temp);
        
        if(fork() >= 0){
            close(1);
            if((fd = open(file,O_RDWR|O_CREATE)) < 0){
                printf(2,"Error opening %s for R/W",file);
                exit();
            }
            dup(fd);
            runCMD(buf);
        }
        wait();
    //else execute the command specified
    }else if (strchr(buf,'<') != 0) {
        char temp[100]="",file[100]="";
        int fd;
        
        strtok(buf,'<',temp,&ptr);
        while(buf[ptr] == ' ')
            ptr++;
        strtok(buf,'<',file,&ptr);
        strcpy(buf,temp);
        if(fork() >= 0){
            close(1);
            if((fd = open(file,O_RDONLY)) < 0){
                printf(2,"Error opening %s for READ",file);
                exit();
            }
            dup(fd);
            runCMD(buf);
        }
        wait();
    } else {
        args[0] = arg0;
        //Provide a dot to ls instead of a blank param
        // to avoid fstat issues in subdirs
        if(strcmp(arg0,"/ls") == 0 && arg1[0] == '\0')
            strcpy(arg1,".");
        
        args[1] = arg1;
        //printf(2,"args[1]|%s|\n",args[1]);

        exec(args[0],args);
        printf(2,"exec failed\n");
       
    }
     exit();
}

int
getcmd(char *buf, int nbuf)
{
  printf(2, "@ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}
int main(int argc,char* argv[])
{
    //Print the initial greeting
    char greeting[] = "Initializing new Shell (nsh)";
    printf(1,"%s\n", greeting);
 
    static char buffer[100]; 
    
  /*  // ----- START FROM sh.c -----
      int fd;

    // Ensure that three file descriptors are open.
    while((fd = open("console", O_RDWR)) >= 0){
        if(fd >= 3){
        close(fd);
        break;
        }
    }
    // ------ End FROM sh,c ------
    */
    
    _Bool exitCommand = 0;
    getcmd(buffer,sizeof(buffer));
    do{
        //rtrim \n 
        buffer[strlen(buffer)-1] = '\0';
       //Change dir on cd to the following string
        if(buffer[0] == 'c' && buffer[1] == 'd' && buffer[2] == ' '){
            if(chdir(buffer+3) !=0) 
                printf(1,"Unable to change directory to %s\n",buffer+3);
        }
        else //otherwise fork process and exec commands
            if(fork()==0 && *buffer != 0)
                runCMD(buffer);
        wait();
        strcpy(buffer,0);
        getcmd(buffer,sizeof(buffer));
        exitCommand = buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't';
        exitCommand = exitCommand && (buffer[5] == '\0' || buffer[5] == '\n' || buffer[5] == ' ');
    }while(exitCommand == 0);
 
    exit();
}
