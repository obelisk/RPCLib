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
  int count7 = 2;
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

  argTypes7[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
  argTypes7[1] = (1 << ARG_INPUT) | (ARG_INT << 16) | 1;
  argTypes7[2] = 0;
  /* 
   * register server functions f0~f4
   */
  char name0[] = "f0";
  char name1[] = "f1";
  char name2[] = "f2";
  char name3[] = "f3";
  char name4[] = "f4";
  char name5[] = "f5";
  char name6[] = "f6";
  char name7[] = "f7";
  char name8[] = "f8";
  char name9[] = "f9";
  char name10[] = "f10";
  char name11[] = "f11";
  rpcRegister(name0, argTypes0, *f0_Skel);
  rpcRegister(name1, argTypes1, *f1_Skel);
  rpcRegister(name2, argTypes2, *f2_Skel);
  rpcRegister(name3, argTypes3, *f3_Skel);
  rpcRegister(name4, argTypes4, *f4_Skel);
    rpcRegister(name5, argTypes5, *f5_Skel);
    rpcRegister(name6, argTypes6, *f6_Skel);

    rpcRegister(name7, argTypes7, *f7_Skel);
    rpcRegister(name8, argTypes8, *f8_Skel);
    rpcRegister(name9, argTypes9, *f9_Skel);
    rpcRegister(name10, argTypes10, *f10_Skel);
	 rpcRegister(name11, argTypes11, *f11_Skel);

 /* call rpcExecute */
  rpcExecute();

  /* return */
  return 0;
}







