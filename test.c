#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

double toDouble(char* s, int start, int stop) {
    long int m = 1;
    int i = 0;
    double ret = 0;
    for (i = stop; i >= start; i--) {
        printf(" char=%c\n",s[i]);
        printf("ret before = %f\n", ret);
        ret += (s[i] - '0') * m;
        printf("ret after = %f\n", ret);
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
  char in_string[] = "4014.84954";
  printf("VALUE = %f\n",toDouble(in_string, 0, 1));
  printf("VALUE = %f\n",toDoubleTenths(in_string, 2, 3));
}
