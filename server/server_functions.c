#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* return: OUT; a, b: IN */
int f0(int a, int b) {

  return a + b;
}   
double f8 (double a, double b){ 
	return a + b;
}
float f9 (float a, float b){
        return a + b;
}
long f10 (long a, long b){
        return a + b;
}


/* returns: OUT; a, b, c, d: IN */
long f1(char a, short b, int c, long d) {

  return a + b * c - d;
}

int f11(int a, int b, int c, int d) {

  return a + b + c + d;
}
int f13(int a[]) {
  int size = 0;
  int result = 0;
  while (a[size])  {
        size++;
  }
  for (int i = 0; i < size; i++)  {
        result += a[i];
  }
  printf("shit 2 %d\n", result);
  return result;
}
int f12(char a[], char b[]) { 
	FILE *fp = fopen(a, "ab+");
	if(fp == 0x0){
		return -1;
	}
        fprintf(fp, "%s", b);
	fclose(fp);
        return fp > 0x0;
        
}

/* return string is the concatenation of the integer 
   part of the float and the interger part of the double
   return: OUT string; a, b: IN */

char* f2(float a, double b) {

  float ai;
  double bi;
  char *str1;

  a = modff(a, &ai);
  b = modf(b, &bi);

  str1 = (char *)malloc(100);

  sprintf(str1, "%lld%lld", (long long)ai, (long long)bi);

  return str1;
}

int f5() { 
	return 1;
}
int f6(int a) {
	return a;
}
/* 
 * bubble sort
 * the first element in the array indicates the size of the array
 * a: INOUT array 
 */

void f3(long a[]) {

  int len = a[0];
  int i, j, k;

  for (i = 0; i < len; i++) {
    for (j = len - 1; j > i; j--) {
      if (a[j] > a[j - 1]) {
	k = a[j];
	a[j] = a[j - 1];
	a[j - 1] = k;
      }
    }
  }
}

/*
 * print file named by a 
 * a: IN array
 */

void f4(char a[]) {

  /* print file a to a printer */
}

void f7(char a[]){ 
	int size = 0;
	while (a[size]) { 
		size++;
	} 
	for (int i = 0; i < size/2; ++i) {
		char temp = a[size-1 - i];
		a[size-1-i] = a[i];
                a[i] = temp;
	}
	printf("FUNCTION: %s\n", a);
}
