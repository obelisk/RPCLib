#include "rpc.h"
#include "server_function_skels.h"

#include <stdlib.h>

int main(int argc, char *argv[]) {
	// one int, one array to string, float, double,
	/* create sockets and connect to the binder */
	rpcInit();
	/* prepare server functions' signatures */
	int count0 = 3;
	int count1 = 5;
	int count2 = 3;
	int count3 = 1;
	int count4 = 1;
	int count5 = 1;
	int count6 = 2;
	int count7 = 1;
	int count8 = 3;
	int argTypes0[count0 + 1];
	int argTypes1[count1 + 1];
	int argTypes2[count2 + 1];
	int argTypes3[count3 + 1];
	int argTypes4[count4 + 1];
	int argTypes5[count5 + 1];
	int argTypes6[count6 + 1];
	int argTypes7[count7 + 1];
	int argTypes8[count8 + 1];

	int count11 = 5;
	int argTypes11[count11 + 1];
	argTypes11[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes11[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[4] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes11[5] = 0;

	int count12 = 3;
	int argTypes12[count12 + 1];

	argTypes12[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes12[1] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | 1;
	argTypes12[2] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | 1;
	argTypes12[3] = 0;

	argTypes8[0] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16);
	argTypes8[1] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes8[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes8[3] = 0;

	int count9 = 3;
	int argTypes9[count9 + 1];
	argTypes9[0] = (1 << ARG_OUTPUT) | (ARG_FLOAT << 16);
	argTypes9[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes9[2] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes9[3] = 0;

	int count10 = 3;
	int argTypes10[count10 + 1];
	argTypes10[0] = (1 << ARG_OUTPUT) | (ARG_LONG << 16);
	argTypes10[1] = (1 << ARG_INPUT) | (ARG_LONG << 16);
	argTypes10[2] = (1 << ARG_INPUT) | (ARG_LONG << 16);
	argTypes10[3] = 0;

	argTypes0[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes0[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes0[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes0[3] = 0;

	argTypes1[0] = (1 << ARG_OUTPUT) | (ARG_LONG << 16);
	argTypes1[1] = (1 << ARG_INPUT) | (ARG_CHAR << 16);
	argTypes1[2] = (1 << ARG_INPUT) | (ARG_SHORT << 16);
	argTypes1[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes1[4] = (1 << ARG_INPUT) | (ARG_LONG << 16);
	argTypes1[5] = 0;

	/*
	 * the length in argTypes2[0] doesn't have to be 100,
	 * the server doesn't know the actual length of this argument
	 */
	argTypes2[0] = (1 << ARG_OUTPUT) | (ARG_CHAR << 16) | 100;
	argTypes2[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes2[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes2[3] = 0;

	/*
	 * f3 takes an array of long.
	*/
	argTypes3[0] = (1 << ARG_OUTPUT) | (1 << ARG_INPUT) | (ARG_LONG << 16) | 1;
	argTypes3[1] = 0;

	/* same here, 28 is the exact length of the parameter */
	argTypes4[0] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | 28;
	argTypes4[1] = 0;

	argTypes5[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes5[1] = 0;

	argTypes6[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes6[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes6[2] = 0;

	argTypes7[0] = (1 << ARG_OUTPUT) | (1 << ARG_INPUT) | (ARG_CHAR << 16) | 1;
	argTypes7[1] = 0;
	      int count13 = 2;
  int argTypes13[count13 + 1];
   argTypes13[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
  argTypes13[1] = (1 << ARG_INPUT) | (ARG_INT << 16) | 1;
  argTypes13[2] = 0;

	/*
	 * register server functions f0~f4
	 */
  char f0_sum_scale[] = "f0-sum-scale";
  char f1_multi_math[] = "f1-multi-math";
  char f2_num_concat[] = "f2-num-concat";
  char f3_bubble_sort[] = "f3-bubble-sort";
  char f4_fail_test[] = "f4-fail_test";
  char f5_success_test[] = "f5-success_test";
  char f6_int_echo[] = "f6-int_echo";
  char f7_reverse_string[] = "f7-reverse_string";
  char f8_double_sum[] = "f8-double_sum";
  char f9_float_sum[] = "f9-float_sum";
  char f10_long_sum[] = "f10-long_sum";
  char f11_four_int_sum[] = "f11-four_int_sum";
  char f12_write_file[] = "f12-write_file";
  char f13_sum_int_arr[] = "f13-sum_int_arr"; 
	rpcRegister(f0_sum_scale, argTypes0, *f0_Skel);
	rpcRegister(f1_multi_math, argTypes1, *f1_Skel);
	rpcRegister(f2_num_concat, argTypes2, *f2_Skel);
	rpcRegister(f3_bubble_sort, argTypes3, *f3_Skel);
	rpcRegister(f4_fail_test, argTypes4, *f4_Skel);
	rpcRegister(f5_success_test, argTypes5, *f5_Skel);
	rpcRegister(f6_int_echo, argTypes6, *f6_Skel);
	rpcRegister(f7_reverse_string, argTypes7, *f7_Skel);
	rpcRegister(f8_double_sum, argTypes8, *f8_Skel);
	rpcRegister(f9_float_sum, argTypes9, *f9_Skel);
	rpcRegister(f10_long_sum, argTypes10, *f10_Skel);
	rpcRegister(f11_four_int_sum, argTypes11, *f11_Skel);
	rpcRegister(f12_write_file, argTypes12, *f12_Skel);
  rpcRegister(f13_sum_int_arr, argTypes13, *f13_Skel);

	/* call rpcExecute */
	rpcExecute();

	/* return */
	return 0;
}
