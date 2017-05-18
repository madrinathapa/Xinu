#include <xinu.h>
#include <stdio.h>


shellcmd xsh_hello(int count, char *word[]){

    if (count == 2) {
        printf("Hello %s, Welcome to the world of Xinu!!", word[1]);
    }

    else if(count >= 3){
        printf("Too many arguments");
    }

    else{
        printf("Too few arguments");
    }

    printf("\n");

    return 0;
}
