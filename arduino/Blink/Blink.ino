#include <SoftwareSerial.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

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

double toDouble(String s, int start_bit, int stop_bit) {
    int m = 1;
    int i = 0;
    double ret = 0;
    for (i = stop_bit; i >= start_bit; i--) {
        ret += (s[i] - '0') * m;
        m *= 10;
    }
    return ret;
}
double toDoubleTenths(String s, int start_bit, int stop_bit) {
    int m = 1;
    int i = 0;
    char newChar[] = ".";
    for (i = start_bit; i <= stop_bit; i++) {
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

String parseGpsMsg(String in_string, int str_length) {
  String out_string = "";
  double lat_value = 0.0;
  double lon_value = 0.0;
  double temp_lon = 0.0;
  double temp_lat = 0.0;
  Serial.println("In_String=" + in_string);
  Serial.println(str_length);
  int i = 0;
  boolean reading = false;
  int value_count = 0;
  int value_start = 0;
  int value_seconds = 0;
  int value_end = 0;
  int minute_start = 0;
  int valid_comma_count = 0;
  double sign = 0.0;
  for(i = 0; i <= str_length; i++) {
    if(value_count < 2) {
      Serial.println(in_string[i]);
      switch(in_string[i]){
        case ',':
          if(!reading) {
            reading = true;
            Serial.println("Start of value");
            value_start = i+1;
          }
          else {
            reading = false;
            Serial.println("End of value");
            delay(1000);
            value_end = i;
            double sign = getSign(in_string[i+1]);
            Serial.println("Sign=");
            Serial.println(sign);
            delay(1000);
            if(value_count == 0) {
              temp_lat += toDoubleTenths(in_string, value_seconds, i-1);
              Serial.println("temp_lat=");
              Serial.println(temp_lat);
              delay(1000);
              lat_value += temp_lat/60.0;
              lat_value *= sign;
            }
            else {
              temp_lon += toDoubleTenths(in_string, value_seconds, i-1);
              Serial.println("temp_lon=");
              Serial.println(temp_lon);
              delay(1000);
              lon_value += temp_lon/60.0;
              lon_value *= sign;
            }
            Serial.println("LAT=");
            Serial.println("LON=");
            Serial.println(lat_value);
            Serial.println(lon_value);
            delay(1000);
            value_count++;
          }
          break;
        case '.':
          Serial.println("Found Decimal");
          if(reading) {
            if(value_count == 0) {
              lat_value += toDouble(in_string, value_start, i-3);
              temp_lat += toDouble(in_string, i-2, i-1);
              Serial.println("temp_lat=");
              Serial.println(temp_lat);
              delay(1000);
            }
            else {
              lon_value += toDouble(in_string, value_start, i-3);
              temp_lon += toDouble(in_string, i-2, i-1);
              Serial.println("temp_lon=");
              Serial.println(temp_lon);
              delay(1000);
            }
            Serial.println("LAT=");
            Serial.println("LON=");
            Serial.println(lat_value);
            Serial.println(lon_value);
            delay(1000);
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
              Serial.println("DATA VALID");
              delay(1000);
              char lat_char[MAX_NUMBER_STRING_SIZE];
              char lon_char[MAX_NUMBER_STRING_SIZE];
              dtoa(lat_char, lat_value);
              dtoa(lon_char, lon_value);
              out_string = "LAT="+(String)lat_char+"LON="+(String)lon_char;
            }
            else if(in_string[i+1] == 'V'){
              Serial.println("INVALID\n");
              delay(1000);
              out_string = "INVALID";
            }
            else {
              Serial.println("unsure: ");
              Serial.println(in_string[i+1]);
              delay(1000);
              out_string = "INVALID ID";
            }
          }
          valid_comma_count++;
          break;
        default:
        ;
      }
    }
  }
  return out_string;
}

String mytest(String in) {
  Serial.println(in);
  return "Did it";
}

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  int bufIndex = 46;
  String in_string = "LL,4014.84954,N,11138.89767,W,162408.00,A,A*7A";
  String finished = "";
  //double tenths = toDoubleTenths(in_string, 8, 11);
  finished = parseGpsMsg(in_string, bufIndex);
  Serial.println("DONE!!!!!");
  Serial.println(finished);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("On");
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  Serial.println("Off");
  delay(1000);               // wait for a second
}
