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
    char arg1[100] ="",arg2[100] = "";

    // Prefix a slash to specify the root directory
    //  when executing a command
    
    arg1[0] = '/';

    //Get first command token for arg1
    strtok(buf,' ',arg1+1,&ptr);
    
    //Set arg2 to the remaining string
    // after the first space
    int curPtr = ptr;
    while(ptr < strlen(buf)){
        arg2[ptr-curPtr] = buf[ptr];
        ptr = ptr+1;
    }
    arg2[ptr-curPtr] = '\0';
    //Replace ending \n with endOfString char
    if(arg1[strlen(arg1)-1]=='\n')
        arg1[strlen(arg1)-1] = '\0';
    //if(arg2[strlen(arg2)-1]=='\n')
    //    arg2[strlen(arg2)-1] = '\0';


    //if is currently a test for pipe
    if (strcmp(arg1,"/tacocat") == 0) {
        while(arg2[0] != '\0'){
            strtok(buf,' ',arg2,&ptr);
            if(arg2[0] != '\0' && arg2[0] != ' '){
                printf(1,"%s ",arg2);
            }
        }
    //else execute the command specified
    }else {
        args[0] = arg1;
        //Provide a dot to ls instead of a blank param
        // to avoid fstat issues in subdirs
        if(strcmp(arg1,"/ls") == 0 && arg2[0] == '\0'){
        
	    arg2[0] = '.';
        arg2[1] = '\0';
    }
            args[1] = arg2;
        //printf(1,"args[1]|%s|\n",args[1]);

        exec(args[0],args);
        printf(1,"exec failed\n");
    }
}
int main(int argc,char* argv[])
{
    //Print the initial greeting
    char greeting[] = "Initializing new Shell (nsh)";
    printf(1,"%s\n", greeting);
 
    
    char buffer[100];
    int nbuf = 100;
    _Bool exitCommand = 0;
    printf(1,"@");
    gets(buffer,nbuf);
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
        printf(1,"@");
        memset(buffer,0,nbuf);
        gets(buffer,100);
        exitCommand = buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't';
        exitCommand = exitCommand && (buffer[5] == '\0' || buffer[5] == '\n' || buffer[5] == ' ');
    }while(exitCommand == 0);
 
    exit();
}
