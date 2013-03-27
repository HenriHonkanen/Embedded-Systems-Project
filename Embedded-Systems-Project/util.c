#include "util.h"

long map(long v, long a, long b, long x,long y) {
  if(v<=a) {
		return x;
	}
	if(v>=b) {
		return y;
	}
	
	long l_ab = (b-a);	
	long l_xy = (y-x);
	
	long t = v-a;

	float r = (float)t / (float)l_ab;
	
	return x + (l_xy*r);
}
