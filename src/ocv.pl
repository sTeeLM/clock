#!/usr/bin/python

ocv = [
  [420, 100],
  [408, 90], 
  [400, 80],
  [393, 70],
  [387, 60],
  [382, 50],
  [379, 40],
  [377, 30],
  [373, 20],
  [370, 15],
  [368, 10],
  [350, 5],
  [330, 0],
]

def voltohex(a) :
	return (int) (a * 10 / 1.3675213)

print "static struct ocv_slot code ocv_table[] = {"

for i in ocv :
	print ("  {%d,%d},"%(voltohex(i[0]), i[1]))

print "};"

exit(0)