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
#define ROM_ALARM0_DUR      3
#define ROM_ALARM1_ENABLE   4

a[0] = 0x7F
a[1] = 12
a[2] = 12
a[3] = 15
a[4] = 1

#define ROM_TIME_IS12       10
#define ROM_BEEPER_MUSIC_INDEX 11
#define ROM_BEEPER_ENABLE      12
# 0=off 1=15s 2=30s
#define ROM_POWERSAVE_TO    13
#define ROM_REMOTE_ONOFF    14
a[10] = 1
a[11] = 0
a[12] = 1
a[13] = 1
a[14] = 1

#define ROM_FUSE_HG_ONOFF   20
#define ROM_FUSE_MPU        21
#define ROM_FUSE_THERMO_HI  22
#define ROM_FUSE_THERMO_LO  23
#define ROM_FUSE_REMOTE_ONOFF 24
#define ROM_FUSE_PASSWORD   25
# 13 - 18 is password, 6 bytes

a[20] = 1
a[21] = 1
a[22] = 40
a[23] = 216
a[24] = 1

a[25] = 1
a[26] = 2
a[27] = 3
a[28] = 4
a[29] = 5
a[30] = 6

#define ROM_LT_TIMER_YEAR 40
#define ROM_LT_TIMER_MONTH  41
#define ROM_LT_TIMER_DATE  42
#define ROM_LT_TIMER_HOUR 43
#define ROM_LT_TIMER_MIN  44
#define ROM_LT_TIMER_SEC  45

a[40] = 14
a[41] = 7  # 0-11
a[42] = 18 # 0-30
a[43] = 12
a[44] = 13
a[45] = 10

#define ROM_POWER_FULL_INT   50
#define ROM_POWER_FULL_EXP   51
#define ROM_POWER_EMPTY_INT  52
#define ROM_POWER_EMPTY_EXP  53

a[50] = 4
a[51] = 20
a[52] = 3
a[53] = 30

#define ROM_RADIO_FREQ_HI 60
#define ROM_RADIO_FREQ_LO 61
#define ROM_RADIO_VOLUME 62
#define ROM_RADIO_HLSI 63
#define ROM_RADIO_MS  64
#define ROM_RADIO_BL  65
#define ROM_RADIO_HCC 66
#define ROM_RADIO_SNC 67
#define ROM_RADIO_DTC 68
a[60] = 0
a[61] = 0
a[62] = 50
a[63] = 0
a[64] = 0
a[65] = 0
a[66] = 0
a[67] = 0
a[68] = 0

with open('24aa01.bin', 'wb') as f:
	f.write(a);
