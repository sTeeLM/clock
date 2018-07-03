#ifndef __CLOCK_SM_FUSE_TEST_H__
#define __CLOCK_SM_FUSE_TEST_H__

#include "sm.h"

enum sm_fuse_test_ss {
  SM_FUSE_TEST_INIT          = 0, // 初始状态0
  SM_FUSE_TEST_BROKE         = 1, // fuse0, fuse1, tripwire 断线测试
  SM_FUSE_TEST_THERMO_HI_SET = 2, // 设置thermo hi参数
  SM_FUSE_TEST_THERMO_HI     = 3, // thermo hi测试
  SM_FUSE_TEST_THERMO_LO_SET = 4, // 设置thermo lo参数
  SM_FUSE_TEST_THERMO_LO     = 5, // thermo lo测试
  SM_FUSE_TEST_HG_SET        = 6, // 设置hg on/off
  SM_FUSE_TEST_HG            = 7, // hg测试
  SM_FUSE_TEST_MPU_SET       = 8, // 设置MPU参数
  SM_FUSE_TEST_MPU           = 9, // mpu测试
  SM_FUSE_TEST_REMOTE_SET    = 10, // remote设置
  SM_FUSE_TEST_REMOTE        = 11  // remote测试
};

extern const char * code sm_fuse_test_ss_name[];

void sm_fuse_test_init(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod0(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod1(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod2(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod3(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod4(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod5(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod6(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod7(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod8(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod9(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_test_submod10(unsigned char from, unsigned char to, enum task_events ev);

#endif
