#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <math.h>

void myFunction(char* in_string, char* out_string) {
  printf("Start_String=%s\n", in_string);
  int in_size = strlen(in_string); //TODO Left off here
  printf("%d\n", in_size);
  int i = 0;
  for(i = 0; i <= in_size-ceil(in_size/2); i++) {
    //printf("%d\n", i);
    out_string[i] = in_string[i];
  }
  out_string[i] = '\0'; //null terminate always
}

void tester(char* test) {
  int i = 0;
  printf("%c\n", test[i]);
  char* temp = test[i];
  
}

int main(int arg) {
  char* myChar = "a";
  tester(myChar);
}
