/*:
 * client.c
 *
 * This file is the client program,
 * which prepares the arguments, calls "rpcCall", and checks the returns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpc.h"

#define CHAR_ARRAY_LENGTH 100

int main(int argc, char **argv) {

	int a0 = 5;
	int b0 = 10;
	int count0 = 3;
	int return0;
	int argTypes0[count0 + 1];
	void **args0;
	argTypes0[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes0[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes0[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes0[3] = 0;
	args0 = (void **)malloc(count0 * sizeof(void *));
	args0[0] = (void *)&return0;
	args0[1] = (void *)&a0;
	args0[2] = (void *)&b0;

	char a1 = 'a';
	short b1 = 100;
	int c1 = 1000;
	long d1 = 10000;
	int count1 = 5;
	long return1;
	int argTypes1[count1 + 1];
	void **args1;
	argTypes1[0] = (1 << ARG_OUTPUT) | (ARG_LONG << 16);
	argTypes1[1] = (1 << ARG_INPUT) | (ARG_CHAR << 16);
	argTypes1[2] = (1 << ARG_INPUT) | (ARG_SHORT << 16);
	argTypes1[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes1[4] = (1 << ARG_INPUT) | (ARG_LONG << 16);
	argTypes1[5] = 0;
	args1 = (void **)malloc(count1 * sizeof(void *));
	args1[0] = (void *)&return1;
	args1[1] = (void *)&a1;
	args1[2] = (void *)&b1;
	args1[3] = (void *)&c1;
	args1[4] = (void *)&d1;

	float a2 = 3.14159;
	double b2 = 1234.1001;
	int count2 = 3;
	char *return2 = (char *)malloc(CHAR_ARRAY_LENGTH * sizeof(char));
	int argTypes2[count2 + 1];
	void **args2;
	argTypes2[0] = (1 << ARG_OUTPUT) | (ARG_CHAR << 16) | CHAR_ARRAY_LENGTH;
	argTypes2[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes2[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes2[3] = 0;
	args2 = (void **)malloc(count2 * sizeof(void *));
	args2[0] = (void *)return2;
	args2[1] = (void *)&a2;
	args2[2] = (void *)&b2;

	long a3[11] = {11, 109, 107, 105, 103, 101, 102, 104, 106, 108, 110};
	int count3 = 1;
	int argTypes3[count3 + 1];
	void **args3;
	argTypes3[0] = (1 << ARG_OUTPUT) | (1 << ARG_INPUT) | (ARG_LONG << 16) | 11;
	argTypes3[1] = 0;
	args3 = (void **)malloc(count3 * sizeof(void *));
	args3[0] = (void *)a3;

	char *a4 = "non_exist_file_to_be_printed";
	int count4 = 1;
	int argTypes4[count4 + 1];
	void **args4;
	argTypes4[0] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | strlen(a4);
	argTypes4[1] = 0;
	args4 = (void **)malloc(count4 * sizeof(void *));
	args4[0] = (void *)a4;

	int argTypes5[2];
	void **args5;
	int return5;
	argTypes5[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes5[1] = 0;
	args5 = (void **)malloc(1 * sizeof(void *));
	args5[0] = (void *)&return5;

	int argTypes6[3];
	void **args6;
	int return6;
	int a6 = 5;
	argTypes6[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes6[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes6[2] = 0;
	args6 = (void **)malloc(2 * sizeof(void *));
	args6[0] = (void *)&return6;
	args6[1] = (void *)&a6;

	int argTypes7[2];
	void **args7;
	argTypes7[0] = (1 << ARG_OUTPUT) | (1 << ARG_INPUT) | (ARG_CHAR << 16) | 2;
	argTypes7[1] = 0;
	args7 = (void **)malloc(1 * sizeof(void *));
	char a7[2] = {'a', 'b'};
	args7[0] = (void *)a7;

	int argTypes8[4];
	void **args8;
	double a8 = 10;
	double b8 = 10;
	double return8;
	argTypes8[0] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16);
	argTypes8[1] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes8[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes8[3] = 0;
	args8 = (void **)malloc(3 * sizeof(void *));
	args8[0] = (void *)&return8;
	args8[1] = (void *)&a8;
	args8[2] = (void *)&b8;

	int argTypes9[4];
	void **args9;
	float a9 = 15;
	float b9 = 10;
	double return9;
	argTypes9[0] = (1 << ARG_OUTPUT) | (ARG_FLOAT << 16);
	argTypes9[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes9[2] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes9[3] = 0;
	args9 = (void **)malloc(3 * sizeof(void *));
	args9[0] = (void *)&return9;
	args9[1] = (void *)&a9;
	args9[2] = (void *)&b9;

    long a10 = 0;
    long b10 = 0;
    int count10 = 3;
    long return10;
    int argTypes10[count10 + 1];
    void **args10;
	argTypes10[0] = (1 << ARG_OUTPUT) | (ARG_LONG << 16);
	argTypes10[1] = (1 << ARG_INPUT) | (ARG_LONG << 16);
	argTypes10[2] = (1 << ARG_INPUT) | (ARG_LONG << 16);
	argTypes10[3] = 0;
	args10 = (void **)malloc(count10 * sizeof(void *));
	args10[0] = (void *)&return10;
	args10[1] = (void *)&a10;
	args10[2] = (void *)&b10;

	int a11 = 100;
	int b11 = 100;
	int c11 = 100;
	int d11 = 100;
	int count11 = 5;
	int return11;
	int argTypes11[count11 + 1];
	void **args11;
	argTypes11[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes11[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[4] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[5] = 0;
	args11 = (void **)malloc(count11 * sizeof(void *));
	args11[0] = (void *)&return11;
	args11[1] = (void *)&a11;
	args11[2] = (void *)&b11;
	args11[3] = (void *)&c11;
	args11[4] = (void *)&d11;

	int count12 = 3;
	int return12;
	int argTypes12[count12 + 1];
	void **args12;
	char a12[8] = {'/', 't', 'm', 'p', '/', 'a', 'b'};
	char b12[2] = {'c', 'd'};
	argTypes12[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes12[1] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | 7;
	argTypes12[2] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | 2;
	argTypes12[3] = 0;
	args12 = (void **)malloc(count12 * sizeof(void *));
	args12[0] = (void *)&return12;
	args12[1] = (void *)a12;
	args12[2] = (void *)b12;


    
  int count13 = 2;
  int return13;
  int argTypes13[count13 + 1];
  void **args13;
  int b13[2] = {2,3};
  argTypes13[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
  argTypes13[1] = (1 << ARG_INPUT) | (ARG_INT << 16) | 2;
  argTypes13[2] = 0;

  args13 = (void **)malloc(count13 * sizeof(void *));
  args13[0] = (void *)&return13;
  args13[1] = (void *)b13;
    // Declare Function Names
    char f0_sum_scale[] = "f0-sum-scale";
    char f1_multi_math[] = "f1-multi-math";
    char f2_num_concat[] = "f2-num-concat";
    char f3_bubble_sort[] = "f3-bubble-sort";
    char f4_fail_test[] = "f4-fail_test";
    char f5_success_test[] = "f5-success_test";
    char f6_int_echo[] = "f5-int_echo";
    char f7_reverse_string[] = "f7-reverse_string";
    char f8_double_sum[] = "f8-double_sum";
    char f9_float_sum[] = "f9-float_sum";
    char f10_long_sum[] = "f10-long_sum";
    char f11_four_int_sum[] = "f11-four_int_sum";
    char f12_write_file[] = "f12-write_file";
    char f13_sum_int_arr[] = "f13-sum_int_arr";
	// rpcCalls
	int s0 = rpcCall(f0_sum_scale, argTypes0, args0);
	printf("\nEXPECTED return of f0 is: %d\n", a0 + b0);
	if (s0 >= 0) {
		printf("ACTUAL return of f0 is: %d\n", *((int *)(args0[0])));
	} else {
		printf("Error: %d\n", s0);
	}

    int s1 = rpcCall(f1_multi_math, argTypes1, args1);
    // test the return of f1
    printf("\nEXPECTED return of f1 is: %ld\n", a1 + b1 * c1 - d1);
    if (s1 >= 0) {
        printf("ACTUAL return of f1 is: %ld\n", *((long *)(args1[0])));
    } else {
        printf("Error: %d\n", s1);
    }

    int s2 = rpcCall(f2_num_concat, argTypes2, args2);
    // test the return of f2
    printf("\nEXPECTED return of f2 is: 31234\n");
    if (s2 >= 0) {
        printf("ACTUAL return of f2 is: %s\n", (char *)args2[0]);
    } else {
        printf("Error: %d\n", s2);
    }

    int s3 = rpcCall(f3_bubble_sort, argTypes3, args3);
    // test the return of f3
    printf("\nEXPECTED return of f3 is: 110 109 108 107 106 105 104 103 102 101 11\n");

    if (s3 >= 0) {
        printf("ACTUAL return of f3 is: ");
        int i;
        for (i = 0; i < 11; i++) {
            printf(" %ld", *(((long *)args3[0]) + i));
        }
        printf("\n");
    } else {
        printf("Error: %d\n", s3);
    }

    int s4 = rpcCall(f4_fail_test, argTypes4, args4);
    // test the return of f4
    printf("\ncalling f4 to print an non existed file on the server");
    printf("\nEXPECTED return of f4: some integer other than 0");
    printf("\nACTUAL return of f4: %d\n", s4);

	int s5 = rpcCall(f5_success_test, argTypes5, args5);
	printf("\nEXPECTED return of f5 is: %d\n", 1);
	if (s5 >= 0) {
		printf("ACTUAL return of f5 is: %d\n", *((int *)(args5[0])));
	} else {
		printf("Error: %d\n", s5);
	}

	int s6 = rpcCall(f6_int_echo, argTypes6, args6);
	printf("\nEXPECTED return of f6 is: %d\n", 5);
	if (s6 >= 0) {
		printf("ACTUAL return of f6 is: %d\n", *((int *)(args6[0])));
	} else {
		printf("Error: %d\n", s6);
	}

	int s7 = rpcCall(f7_reverse_string, argTypes7, args7);
	// test the return of f3
	printf("\nEXPECTED return of f7 is: ba\n");

	if (s7 >= 0) {
		printf("ACTUAL return of f7 is: %c %c\n", *(((char *)args7[0]) + 0), *(((char *)args7[0]) + 1));
	} else {
		printf("Error: %d\n", s7);
	}

	int s8 = rpcCall(f8_double_sum, argTypes8, args8);
	printf("\nEXPECTED return of f8 is: %d\n", 20);
	if (s8 >= 0) {
		printf("ACTUAL return of f8 is: %lf\n", *((double *)(args8[0])));
	} else {
		printf("Error: %d\n", s8);
	}

	int s9 = rpcCall(f9_float_sum, argTypes9, args9);
	printf("\nEXPECTED return of f9 is: %d\n", 25);
	if (s9 >= 0) {
		printf("ACTUAL return of f9 is: %f\n", *((float *)(args9[0])));
	} else {
		printf("Error: %d\n", s9);
	}

	int s10 = rpcCall(f10_long_sum, argTypes10, args10);
	printf("\nEXPECTED return of f10 is: %d\n", 0);
	if (s10 >= 0) {
		printf("ACTUAL return of f10 is: %f\n", *((float *)(args10[0])));
	} else {
		printf("Error: %d\n", s10);
	}

	int s11 = rpcCall(f11_four_int_sum, argTypes11, args11);
	// test the return of f1
	printf("\nEXPECTED return of f11 is: %d\n", a11 + b11 + c11 + d11);
	if (s11 >= 0) {
		printf("ACTUAL return of f11 is: %d\n", *((int *)(args11[0])));
	} else {
		printf("Error: %d\n", s11);
	}

    int s12 = rpcCall(f12_write_file, argTypes12, args12);
    // test the return of f4
    //  printf("\ncalling f12 to print an non existed file on the server");
    printf("\nEXPECTED return of f12: some integer other than 0");
    printf("\nACTUAL return of f12: %d\n", (*(int *)(args12[0])));

             int s13 = rpcCall(f13_sum_int_arr, argTypes13, args13);
  printf("\nEXPECTED return of f13: 5\n");
  if (s13 >= 0) {
    printf("ACTUAL return of f13 is: %d\n", *((int *)(args13[0])));
  }
  else {
    printf("Error: %d\n", s13);
  }



	/* rpcTerminate */
	printf("\ndo you want to terminate? y/n: ");
	if (getchar() == 'y')
		rpcTerminate();

	/* end of client.c */
	return 0;

}
