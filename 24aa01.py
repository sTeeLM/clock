#!/usr/bin/python

import os,sys,getopt,re
import array

a = array.array('B')

for i in range(128) :
	a.append(0) 

#alarm
#00  alarm0.day_mask = 0x7F;
#01  alarm0.hour = 12;
#02  alarm0.min  = 12;
#03  alarm0_is12 = 1;
#04  alarm1_enable = 1;

#beeper
#05  beeper_music_index
#06  beep_enable

#power
#07  powersave_to


a[0] = 0x7F
a[1] = 12
a[2] = 12
a[3] = 1 
a[4] = 1
a[5] = 0
a[6] = 1
a[7] = 0

with open('24aa01.bin', 'wb') as f:
	f.write(a);
