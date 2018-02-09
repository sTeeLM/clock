#ifndef __CLOCK_SM_FUSE_TEST_H__
#define __CLOCK_SM_FUSE_TEST_H__

#include "sm.h"

enum sm_fuse_test_ss {
  SM_FUSE_TEST_INIT          = 0, // ≥ı º◊¥Ã¨0
  SM_FUSE_TEST_FUSE0_BROKE   = 1, // fuse0∂œ¬∑≤‚ ‘
  SM_FUSE_TEST_FUSE1_BROKE   = 2, // fuse1∂œ¬∑≤‚ ‘
  SM_FUSE_TEST_TRIPWIRE      = 3, // tripwire≤‚ ‘
  SM_FUSE_TEST_THERMO_HI     = 4, // thermo hi≤‚ ‘
  SM_FUSE_TEST_THERMO_LO     = 5,  // thermo lo≤‚ ‘
  SM_FUSE_TEST_HG            = 6, // hg≤‚ ‘
  SM_FUSE_TEST_GYRO          = 7  // gyro≤‚ ‘
};

extern const char * code sm_fuse_test_ss_name[];

void sm_fuse_test(unsigned char from, unsigned char to, enum task_events);

#endif