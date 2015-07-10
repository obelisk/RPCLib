#include "server_functions.h"
#include <stdio.h>
#include <string.h>

int f0_Skel(int *argTypes, void **args) {

  *(int *)args[0] = f0(*(int *)args[1], *(int *)args[2]);
  printf("Params %d, %d\n", *(int *)args[1], *(int *)args[2]);
  return 0;
}
int f8_Skel(int *argTypes, void **args){
   printf ("%lf\n", *(double *)args[1]); 
  *(double *)args[0] = f8(*(double *)args[1], *(double*)args[2]);
   printf("Params %f, %f\n", *(double *)args[1], *(double *)args[2]);
   printf ("%lf\n", *(double *)args[0]);

   return 0;

}

int f9_Skel(int *argTypes, void **args){
//   printf ("%lf\n", *(double *)args[1]);
  *(float *)args[0] = f9(*(float *)args[1], *(float*)args[2]);
 //  printf("Params %f, %f\n", *(double *)args[1], *(double *)args[2]);
 //  printf ("%lf\n", *(double *)args[0]);

   return 0;

}

int f10_Skel(int *argTypes, void **args){
 //  printf ("%lf\n", *(double *)args[1]);
  *(long *)args[0] = f10(*(long *)args[1], *(long*)args[2]);
//   printf("Params %f, %f\n", *(double *)args[1], *(double *)args[2]);
 //  printf ("%lf\n", *(double *)args[0]);

   return 0;

}
 
int f1_Skel(int *argTypes, void **args) {

  *((long *)*args) = f1( *((char *)(*(args + 1))), 
		        *((short *)(*(args + 2))),
		        *((int *)(*(args + 3))),
		        *((long *)(*(args + 4))) );

  return 0;
}

int f11_Skel(int *argTypes, void **args) {

  *((int *)*args) = f11( *((int *)(*(args + 1))),
                        *((int *)(*(args + 2))),
                        *((int *)(*(args + 3))),
                        *((int *)(*(args + 4))) );

  return 0;
}


int f2_Skel(int *argTypes, void **args) {

  /* (char *)*args = f2( *((float *)(*(args + 1))), *((double *)(*(args + 2))) ); */
  *args = f2( *((float *)(*(args + 1))), *((double *)(*(args + 2))) );

  return 0;
}

int f3_Skel(int *argTypes, void **args) {

  f3((long *)(*args));
  return 0;
}
int f7_Skel(int *argTypes, void **args) {
	printf("shittttt  %d\n", *(char *)args[0]);  
	f7((char *)(*args));
	return 0;
}
/* 
 * this skeleton doesn't do anything except returns
 * a negative value to mimic an error during the 
 * server function execution, i.e. file not exist
 */
int f4_Skel(int *argTypes, void **args) {

  return -1; /* can not print the file */
}

int f5_Skel(int *argTypes, void **args) { 
	*(int *)args[0] = f5();
	return 0;
}
int f6_Skel(int *argTypes, void **args) { 
	*(int *)args[0] = f6(*(int *)args[1]);
	return 0;
}
