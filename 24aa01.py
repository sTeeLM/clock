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
#define ROM_BEEPER_MUSIC_TO    7

a[5] = 0
a[6] = 1
a[7] = 30

#define ROM_POWERSAVE_TO    8

a[8] = 0

#define ROM_FUSE_HG_ONOFF   9
#define ROM_FUSE_GYRO_ONOFF 10
#define ROM_FUSE_THERMO_HI  11
#define ROM_FUSE_THERMO_LO  12
#define ROM_FUSE_TRIPWIRE_ONOFF  13
#define ROM_FUSE_PASSWORD   14
# 14 - 19 is password, 6 bytes

a[9] = 1
a[10] = 1
a[11] = 40
a[12] = 216
a[13] = 1

a[14] = 1
a[15] = 2
a[16] = 3
a[17] = 4
a[18] = 5
a[19] = 6

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
a[24] = 15
a[25] = 10

#define ROM_FUSE0_SHORT_GOOD 40
#define ROM_FUSE0_BROKE_GOOD 41
#define ROM_FUSE1_SHORT_GOOD 42
#define ROM_FUSE1_BROKE_GOOD 43
#define ROM_TRIPWIRE_GOOD    44
#define ROM_THERMO_HI_GOOD   45
#define ROM_THERMO_LO_GOOD   46
#define ROM_HG_GOOD          47
#define ROM_GYRO_GOOD        48

a[40] = 1
a[41] = 1
a[42] = 1
a[43] = 1
a[44] = 1
a[45] = 1
a[46] = 1
a[47] = 1
a[48] = 1

with open('24aa01.bin', 'wb') as f:
	f.write(a);
