#include "types.h"
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
    char arg1[100],arg2[100];
    
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
    
    //Replace ending \n with endOfString char
    if(arg1[strlen(arg1)-1]=='\n')
        arg1[strlen(arg1)-1] = '\0';
    if(arg2[strlen(arg2)-1]=='\n')
        arg2[strlen(arg2)-1] = '\0';


    //if is currently a test
    if (arg1[0] == 't' && arg1[1] =='a' && arg1[2]=='c' && arg1[3] == 'o' && arg1[4] == '\0') {
        while(arg2[0] != '\0'){
            strtok(buf,' ',arg2,&ptr);
            if(arg2[0] != '\0' && arg2[0] != ' '){
                printf(1,"%s ",arg2);
            }
        }
    //else execute the command specified
    }else {
        args[0] = arg1;
        args[1] = arg2;
       // printf(1,"args[0]|%s|\n",args[0]);
        
        exec(args[0],args);
        printf(1,"exec failed\n");
    }
}
int main(int argc,char* argv[])
{   //Print the initial greeting
    char greeting[] = "Initializing new Shell (nsh)";
    printf(1,"%s\n", greeting);
 
    
    char buffer[100];
    char quitCommand[100] = "quit";
    
    printf(1,"@");
    gets(buffer,100);
    while(*buffer != *quitCommand){
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
        gets(buffer,100);
    }
 
    exit();
}
