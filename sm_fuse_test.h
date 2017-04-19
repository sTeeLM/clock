#ifndef __CLOCK_SM_FUSE_TEST_H__
#define __CLOCK_SM_FUSE_TEST_H__

#include "sm.h"

enum sm_fuse_test_ss {
  SM_FUSE_TEST_INIT          = 0, // ≥ı º◊¥Ã¨
  SM_FUSE_TEST_FUSE0_SHORT   = 1, // fuse0∂Ã¬∑≤‚ ‘
  SM_FUSE_TEST_FUSE0_BROKE   = 2, // fuse0∂œ¬∑≤‚ ‘
  SM_FUSE_TEST_FUSE1_SHORT   = 3, // fuse1∂Ã¬∑≤‚ ‘
  SM_FUSE_TEST_FUSE1_BROKE   = 4, // fuse1∂œ¬∑≤‚ ‘
  SM_FUSE_TEST_TRIPWIRE      = 5, // tripwire≤‚ ‘
  SM_FUSE_TEST_THERMO_HI     = 6, // thermo hi≤‚ ‘
  SM_FUSE_TEST_THERMO_LO     = 7,  // thermo lo≤‚ ‘
  SM_FUSE_TEST_HG            = 8, // hg≤‚ ‘
  SM_FUSE_TEST_GYRO          = 9  // gyro≤‚ ‘
};

void sm_fuse_test(unsigned char from, unsigned char to, enum task_events);

#endif