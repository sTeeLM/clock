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
#define ROM_ALARM1_ENABLE   3

a[0] = 0x7F
a[1] = 12
a[2] = 12
a[3] = 1

#define ROM_TIME_IS12       4
#define ROM_BEEPER_MUSIC_INDEX 5
#define ROM_BEEPER_ENABLE      6
#define ROM_BEEPER_MUSIC_TO    7

a[4] = 1
a[5] = 0
a[6] = 1
a[7] = 30

#define ROM_POWERSAVE_TO    8
# 0=off 1=15s 2=30s

a[8] = 1

#define ROM_FUSE_HG_ONOFF   9
#define ROM_FUSE_MPU        10
#define ROM_FUSE_THERMO_HI  11
#define ROM_FUSE_THERMO_LO  12
#define ROM_FUSE_PASSWORD   13
# 13 - 18 is password, 6 bytes

a[9] = 1
a[10] = 1
a[11] = 40
a[12] = 216

a[13] = 1
a[14] = 2
a[15] = 3
a[16] = 4
a[17] = 5
a[18] = 6

#define ROM_LT_TIMER_YEAR 20
#define ROM_LT_TIMER_MONTH  21
#define ROM_LT_TIMER_DATE  22
#define ROM_LT_TIMER_HOUR 23
#define ROM_LT_TIMER_MIN  24
#define ROM_LT_TIMER_SEC  25

a[20] = 14
a[21] = 7  # 0-11
a[22] = 18 # 0-30
a[23] = 12
a[24] = 13
a[25] = 10

#define ROM_POWER_FULL_INT   30
#define ROM_POWER_FULL_EXP   31
#define ROM_POWER_EMPTY_INT  32
#define ROM_POWER_EMPTY_EXP  33

a[30] = 4
a[31] = 20
a[32] = 3
a[33] = 30

with open('24aa01.bin', 'wb') as f:
	f.write(a);
