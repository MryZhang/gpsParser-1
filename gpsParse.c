#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

typedef enum { false, true } bool;

static double PRECISION = 0.00000000000001;
static int MAX_NUMBER_STRING_SIZE = 32;
/**
 * Double to ASCII
 */
char * dtoa(char *s, double n) {
    // handle special cases
    if (isnan(n)) {
        s = "nan";
    } else if (isinf(n)) {
        s = "inf";
    } else if (n == 0.0) {
        s = "0";
    } else {
        int digit, m, m1;
        char *c = s;
        int neg = (n < 0);
        if (neg)
            n = -n;
        // calculate magnitude
        m = log10(n);
        int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            n = n / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return s;
}

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
        printf("%s\n", "NO SIGN");
        return 0.0;
      ;
    }
}

void parseGpsMsg(char* in_string, char* out_string, int str_length) {
  char out_buffer[] = "";
  double lat_value = 0.0;
  double lon_value = 0.0;
  double temp_lon = 0.0;
  double temp_lat = 0.0;
  int in_size = str_length;
  printf("In_String=%s\n", in_string);
  printf("%d\n", in_size);
  int i = 0;
  bool reading = false;
  int value_count = 0;
  int value_start = 0;
  int value_seconds = 0;
  int value_end = 0;
  int minute_start = 0;
  int valid_comma_count = 0;
  double sign = 0.0;
  for(i = 0; i <= in_size; i++) {
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
            double sign = getSign(in_string[i+1]);
            printf("Sign=%f\n", sign);
            if(value_count == 0) {
              temp_lat += toDoubleTenths(in_string, value_seconds, i-1);
              printf("temp_lat=%f\n",temp_lat);
              lat_value += temp_lat/60.0;
              lat_value *= sign;
            }
            else {
              temp_lon += toDoubleTenths(in_string, value_seconds, i-1);
              printf("temp_lon=%f\n",temp_lon);
              lon_value += temp_lon/60.0;
              lon_value *= sign;
            }
            printf("LAT=%f LON=%f\n",lat_value, lon_value);
            value_count++;
          }
          break;
        case '.':
          printf("%s\n","Found Decimal");
          if(reading) {
            if(value_count == 0) {
              lat_value += toDouble(in_string, value_start, i-3);
              temp_lat += toDouble(in_string, i-2, i-1);
              printf("temp_lat=%f\n",temp_lat);
            }
            else {
              lon_value += toDouble(in_string, value_start, i-3);
              temp_lon += toDouble(in_string, i-2, i-1);
              printf("temp_lon=%f\n",temp_lon);
            }
            printf("LAT=%f LON=%f\n",lat_value, lon_value);
            value_seconds = i+1;
          }
          break;
        default :
            ;
      }
    }
    else if(value_count >= 2) {
      //check valid
      switch(in_string[i]) {
        case ',':
          if(valid_comma_count == 1) {
            if(in_string[i+1] == 'A'){
              printf("DATA VALID\n");
              char lat_char[MAX_NUMBER_STRING_SIZE];
              char lon_char[MAX_NUMBER_STRING_SIZE];
              dtoa(lat_char, lat_value);
              dtoa(lon_char, lon_value);
              for(i = 0; i <= 12; i++) {
                out_string[0] = 'L';
                out_string[1] = 'A';
                out_string[2] = 'T';
                out_string[3] = '=';
                out_string[i+4] = lat_char[i];
                out_string[17] = ' ';
                out_string[18] = 'L';
                out_string[19] = 'O';
                out_string[20] = 'N';
                out_string[21] = '=';
                out_string[i+22] = lon_char[i];
              }
            }
            else if(in_string[i+1] == 'V'){
              printf("INVALID\n");
              out_string[0] = 'I';
              out_string[1] = 'N';
              out_string[2] = 'V';
              out_string[3] = 'A';
              out_string[4] = 'L';
              out_string[5] = 'I';
              out_string[6] = 'D';
            }
            else {
              printf("unsure: %c\n", in_string[i+1]);
              out_string[0] = 'I';
              out_string[1] = 'N';
              out_string[2] = 'V';
              out_string[3] = 'A';
              out_string[4] = 'L';
              out_string[5] = 'I';
              out_string[6] = 'D';
              out_string[7] = ' ';
              out_string[8] = 'I';
              out_string[9] = 'D';
            }
          }
          valid_comma_count++;
          break;
        default:
        ;
      }
    }
  }
}

int main(int arg) {
  int bufIndex = 46;
  char* in_string = "$GPGLL,,,,,162404.00,V,N*4F";
  char* out_string = malloc(bufIndex+100);
  printf("In_String=%s\n", in_string);
  parseGpsMsg(in_string, out_string, bufIndex);
  printf("Out_String=%s\n", out_string);
  free(out_string);
}
