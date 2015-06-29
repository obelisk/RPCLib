void intToArr(int value, char out[4]){
	unsigned int mask = 0xFF000000;
	out[0] = ((value & mask) >> 24);
	mask = mask >> 8;
	out[1] = ((value & mask) >> 16);
	mask = mask >> 8;
	out[2] = ((value & mask) >> 8);
	mask = mask >> 8;
	out[3] = (value & mask);
}