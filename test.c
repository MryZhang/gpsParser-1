#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

void myFunction(char* in_string, char* out_string) {
  printf("Start_String=%s\n", in_string);
  int in_size = (int)(sizeof(in_string));
  printf("%d\n", in_size);
  int i = 0;
  for(i = 0; i <= in_size-ceil(in_size/2); i++) {
    //printf("%d\n", i);
    out_string[i] = in_string[i];
  }
}

int main(int arg) {
  char in_string[] = "LL,4014.84954";
  char out_string[] = "";
  printf("In_String=%s\n", in_string);
  myFunction(in_string, out_string);
  printf("Out_String=%s\n", out_string);
}
