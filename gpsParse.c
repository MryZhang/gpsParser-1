#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

double toDouble(char* s, int start, int stop) {
    int m = 1;
    int i = 0;
    double ret = 0;
    for (i = stop; i >= start; i--) {
        ret += (s[i] - '0') * m;
        m *= 10;
    }
    return ret;
}
double toDoubleTenths(char* s, int start, int stop) {
    int m = 1;
    int i = 0;
    char newChar[] = ".";
    for (i = start; i <= stop; i++) {
        printf(" char=%c\n",s[i]);
        printf("newChar before = %s\n", newChar);
        newChar[m] = s[i];
        printf("newChar after = %s\n", newChar);
        m++;
    }
    return atof(newChar);
}

int main(int arg) {
  typedef enum { false, true } bool;
  char in_string[] = "$GPGLL,4014.84954,N,11138.89767,W,162408.00,A,A*7A";
  char out_buffer[] = "";
  double lat_value = 0.0;
  double lon_value = 0.0;
  printf("%d\n", (int)sizeof(in_string));
  int i;
  bool reading = false;
  int value_count = 0;
  int value_start = 0;
  int value_seconds = 0;
  int value_end = 0;
  int minute_start = 0;
  for(i = 0; i <= sizeof(in_string); i++) {
    if(value_count < 2) {
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
            //get seconds
            //value *= sign;//get sign
            value_count++;
          }
          break;
        case '.':
          printf("%s\n","Found Decimal");
          if(reading) {
            if(value_count == 0) {
              lat_value += toDouble(in_string, value_start, i-3);
              lat_value += toDoubleTenths(in_string, i-2, i-1);
            }
            else {
              lon_value += toDouble(in_string, value_start, i-3);
              lon_value += toDoubleTenths(in_string, i-2, i-1);
            }
            printf("LAT=%f LON=%f\n",lat_value, lon_value);
            value_seconds = i+1;
          }
          break;
        default :
            ;
      }
    }
  }
  printf("%s\n",in_string);
  printf("%s\n",out_buffer);
}
