#include <stdio.h>

int main(){
    char text[200];
    printf("Type the message: \n");
    fgets(text, 200, stdin);
    printf("The message is: %s",text);

    return 0;
}