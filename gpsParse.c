#include <stdio.h>

int main(int arg) {
  typedef enum { false, true } bool;
  char in_string[] = "$GPGLL,4014.84954,N,11138.89767,W,162408.00,A,A*7A";
  char out_buffer[] = "";
  double value = 0.0;
  printf("%d\n", (int)sizeof(in_string));
  int i;
  bool reading = false;
  int value_count = 0;
  int value_start = 0;
  int value_end = 0;
  int minute_start = 0;
  for(i = 0; i <= sizeof(in_string); i++) {
    printf("%c\n", in_string[i]);
    switch(in_string[i]){
      case ',':
        if(!reading) {
          reading = true;
          printf("%s\n","Start of value");
          value_start = i+1;
        }
        else {
          reading = false;
          printf("%s\n","End of value");
          value_end = i;
        }
        break;
      case '.':
        printf("%s\n","Found Decimal");
//        value += in_string[value_start]
        break;
      default :
          ;
    }
  }
  printf("%s\n",in_string);
  printf("%s\n",out_buffer);
}
