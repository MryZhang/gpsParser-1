#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include <string.h>

typedef enum { false, true } bool;

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
        newChar[m] = s[i];
        m++;
    }
    return atof(newChar);
}
double getSign(char sign_char) {
    switch(sign_char){
      case 'N':
        return 1.0;
      case 'S':
        return -1.0;
      case 'W':
        return -1.0;
      case 'E':
        return 1.0;
      default:
        printf("%s\n", "BROKEN");
      ;
    }
}

void parseGpsMsg(char* in_string, char* out_string) {
  char out_buffer[] = "";
  double lat_value = 0.0;
  double lon_value = 0.0;
  double temp_lon = 0.0;
  double temp_lat = 0.0;
  int in_size = strlen(in_string);
  int i = 0;
  bool reading = false;
  int value_count = 0;
  int value_start = 0;
  int value_seconds = 0;
  int value_end = 0;
  int minute_start = 0;
  double sign = 0.0;
  for(i = 0; i <= in_size; i++) {
    if(value_count < 2) {
      switch(in_string[i]){
        case ',':
          if(!reading) {
            reading = true;
            value_start = i+1;
          }
          else {
            reading = false;
            value_end = i;
            double sign = getSign(in_string[i+1]);
            if(value_count == 0) {
              temp_lat += toDoubleTenths(in_string, value_seconds, i-1);
              lat_value += temp_lat/60.0;
              lat_value *= sign;
            }
            else {
              temp_lon += toDoubleTenths(in_string, value_seconds, i-1);
              lon_value += temp_lon/60.0;
              lon_value *= sign;
            }
            value_count++;
          }
          break;
        case '.':
          if(reading) {
            if(value_count == 0) {
              lat_value += toDouble(in_string, value_start, i-3);
              temp_lat += toDouble(in_string, i-2, i-1);
            }
            else {
              lon_value += toDouble(in_string, value_start, i-3);
              temp_lon += toDouble(in_string, i-2, i-1);
            }
            value_seconds = i+1;
          }
          break;
        default :
            ;
      }
    }
  }
  char message[in_size];
  sprintf(message, "LAT=%f LON=%f", lat_value, lon_value);
  for(i = 0; i <= strlen(message); i++) {
    out_string[i] = message[i];
  }
}

int main(int arg) {
  char* in_string = "LL,4014.84954,N,11138.89767,W,162408.00,A,A*7A";
  char* out_string = malloc(strlen(in_string)+1);
  printf("In_String=%s\n", in_string);
  parseGpsMsg(in_string, out_string);
  printf("Out_String=%s\n", out_string);
  free(out_string);
}
