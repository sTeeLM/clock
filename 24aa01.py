#!/usr/bin/python

import os,sys,getopt,re
import array

a = array.array('B')

for i in range(256) :
	a.append(0) 

#alarm
#define ROM_ALARM0_DAY_MASK 0
#define ROM_ALARM0_HOUR     1
#define ROM_ALARM0_MIN      2
#define ROM_ALARM0_IS12     3
#define ROM_ALARM1_ENABLE   4

a[0] = 0x7F
a[1] = 12
a[2] = 12
a[3] = 1
a[4] = 1

#define ROM_BEEPER_MUSIC_INDEX 5
#define ROM_BEEPER_ENABLE      6

a[5] = 0
a[6] = 1

#define ROM_POWERSAVE_TO    7

a[7] = 0

#define ROM_FUSE_HG_ONOFF   8
#define ROM_FUSE_GYRO_ONOFF 9
#define ROM_FUSE_THERMO_HI  10
#define ROM_FUSE_THERMO_LO  11
#define ROM_FUSE_TRIPWIRE_ONOFF  12
#define ROM_FUSE_PASSWORD   13
# 13 - 18 is password

a[8] = 1
a[9] = 1
a[10] = 40
a[11] = 216
a[12] = 1

a[13] = 1
a[14] = 2
a[15] = 3
a[16] = 4
a[17] = 5
a[18] = 6


#define ROM_LT_TIMER_SYNC_YEAR 19
#define ROM_LT_TIMER_SYNC_MON  20
#define ROM_LT_TIMER_SYNC_DATE 21
#define ROM_LT_TIMER_SYNC_HOUR 22
#define ROM_LT_TIMER_SYNC_MIN  23
#define ROM_LT_TIMER_SYNC_SEC  24

a[19] = 14
a[20] = 8
a[21] = 17
a[22] = 13
a[23] = 1
a[24] = 5

with open('24aa01.bin', 'wb') as f:
	f.write(a);
