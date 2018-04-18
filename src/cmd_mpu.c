#include <string.h>
#include "stdlib.h"
#include "cmd_mpu.h"
#include "shell.h"
#include "debug.h"
#include "mpu.h"
#include "com.h"

static void mpu_read(void)
{
	int x, y, z;
	float res;
	mpu_read_acc(&x, &y, &z);
	res = x;
	res = ((float) res / 16384.0 );
	printf("x = %f[%d] ", res, x);
	res = y;
	res = ((float) res / 16384.0 );
	printf("y = %f[%d] ", res, y);
	res = z;
	res = ((float) res / 16384.0 );
	printf("z = %f[%d]\n", res, z);	
}

char cmd_mpu(char arg1, char arg2)
{

  if(arg1 != 0 && arg2 == 0 && strcmp("on", shell_buf + arg1) == 0) {
    mpu_enable(1);
	} else if(arg1 != 0 && arg2 == 0 && strcmp("off", shell_buf + arg1) == 0) {
    mpu_enable(0);
	} else if(arg1 != 0 && arg2 == 0 && strcmp("rd", shell_buf + arg1) == 0) {
		mpu_read();	
	} else if(arg1 != 0 && arg2 == 0 && strcmp("lp", shell_buf + arg1) == 0) {
		while(1) {
			if(com_try_get_key() != 0)
				break;
			if(test_task(EV_SCAN_INT_HUB)) {
				mpu_read();	
				mpu_read_int_status();
				clr_task(EV_SCAN_INT_HUB);
			}
		}		
	} else if(arg1 != 0 && arg2 == 0 && strcmp("int", shell_buf + arg1) == 0) {
		printf("interrupt status = %02bx\n", mpu_read_int_status());
  } else {
		return 1;
	}
	return 0;
}