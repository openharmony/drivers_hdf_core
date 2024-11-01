/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"
#include "osal_get_case_test.h"
#include "osal_test_case_def.h"

using namespace testing::ext;

class OsalTestPosix : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static int32_t HdfOsalUserEntry(enum HdfOsalTestCaseCmd cmd);
    void SetUp();
    void TearDown();
private:
    static const int OSAL_TEST_TIME = 30;
    static const int OSAL_WAIT_END_TIME = 5;
};

int32_t OsalTestPosix::HdfOsalUserEntry(enum HdfOsalTestCaseCmd cmd)
{
    int32_t result = 0;
    if (cmd < OSAL_TEST_MAX) {
        if (cmd == OSAL_TEST_START) {
            result = OsaTestBegin(0);
        } else if (cmd == OSAL_TEST_END) {
            result = OsaTestEnd();
        } else if (cmd == OSAL_TEST_ALL) {
            result = OsaTestALLResult();
        } else {
            result = OsalGetTestResult(cmd);
        }
    } else {
        return -1;
    }

    printf("HdfTestUser:osal test result[%d-%d]", result, cmd);

    return result;
}

#define OSAL_TEST_FUNC_DEFINE(subCmd) do { \
    printf("OSAL test enter cmd:%d\n\r", subCmd); \
    EXPECT_EQ(0, HdfOsalUserEntry(subCmd)); \
} while (0)

void OsalTestPosix::SetUpTestCase()
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_START);
    sleep(OSAL_TEST_TIME);
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_END);
    sleep(OSAL_WAIT_END_TIME);
}

void OsalTestPosix::TearDownTestCase()
{
}

void OsalTestPosix::SetUp()
{
}

void OsalTestPosix::TearDown()
{
}

/**
  * @tc.name: OsalGetMutex001
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_CREATE);
}

/**
  * @tc.name: OsalGetMutex002
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_LOCK_TIMEOUT);
}

/**
  * @tc.name: OsalGetMutex003
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_UNLOCK);
}

/**
  * @tc.name: OsalGetMutex004
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_LOCK_FOREVER);
}

/**
  * @tc.name: OsalGetMutex005
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_DESTROY);
}

/**
  * @tc.name: OsalGetMutex006
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_VISIT_DESTROY);
}

/**
  * @tc.name: OsalGetMutex007
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_STRESS_TEST);
}

/**
  * @tc.name: OsalGetMutex008
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTestPosix, OsalGetMutex008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_OTHER);
}

/**
  * @tc.name: OsalGetSpin001
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_CREATE);
}

/**
  * @tc.name: OsalGetSpin002
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK);
}

/**
  * @tc.name: OsalGetSpin003
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_UNLOCK);
}

/**
  * @tc.name: OsalGetSpin004
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK_IRQ);
}

/**
  * @tc.name: OsalGetSpin005
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_UNLOCK_IRQ);
}

/**
  * @tc.name: OsalGetSpin006
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK_IRQ_SAVE);
}

/**
  * @tc.name: OsalGetSpin007
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_UNLOCK_IRQ_RESTORE);
}

/**
  * @tc.name: OsalGetSpin008
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_DESTROY);
}

/**
  * @tc.name: OsalGetSpin009
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTestPosix, OsalGetSpin009, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK_AFTER_DESTROY);
}

/**
  * @tc.name: OsalGetSpin010
  * @tc.desc: LOG function test
  * @tc.type: FUNC
  * @tc.require: AR000F8695
  */
HWTEST_F(OsalTestPosix, OsalGetSpin010, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_STRESS_TEST);
}

/**
  * @tc.name: OsalGetSpin011
  * @tc.desc: LOG function test
  * @tc.type: FUNC
  * @tc.require: AR000F8695
  */
HWTEST_F(OsalTestPosix, OsalGetSpin011, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_OTHER);
}

/**
  * @tc.name: OsalGetTime001
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTestPosix, OsalGetTime001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_SLEEP);
}
/**
  * @tc.name: OsalGetTime002
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTestPosix, OsalGetTime002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_MSLEEP);
}
/**
  * @tc.name: OsalGetTime003
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTestPosix, OsalGetTime003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_USLEEP);
}
/**
  * @tc.name: OsalGetTime004
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTestPosix, OsalGetTime004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_GETTIME);
}
/**
  * @tc.name: OsalGetTime005
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTestPosix, OsalGetTime005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_DIFFTIME);
}

/**
  * @tc.name: OsalGetThread001
  * @tc.desc: thread function test
  * @tc.type: FUNC
  * @tc.require: AR000F868S
  */
HWTEST_F(OsalTestPosix, OsalGetThread001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_THREAD_CREATE);
}
/**
  * @tc.name: OsalGetThread002
  * @tc.desc: thread function test
  * @tc.type: FUNC
  * @tc.require: AR000F868S
  */
HWTEST_F(OsalTestPosix, OsalGetThread002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_THREAD_PARA_CHECK);
}
/**
  * @tc.name: OsalGetThread003
  * @tc.desc: thread function test
  * @tc.type: FUNC
  * @tc.require: AR000F868S
  */
HWTEST_F(OsalTestPosix, OsalGetThread003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_THREAD_RUN_CHECK);
}

/**
  * @tc.name: OsalGetMemory001
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTestPosix, OsalGetMemory001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_BIG);
}
/**
  * @tc.name: OsalGetMemory002
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTestPosix, OsalGetMemory002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_SMALL);
}
/**
  * @tc.name: OsalGetMemory003
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTestPosix, OsalGetMemory003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_BIG_STRESS);
}
/**
  * @tc.name: OsalGetMemory004
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTestPosix, OsalGetMemory004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_SMALL_STRESS);
}

/**
  * @tc.name: OsalGetList001
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_INIT);
}
/**
  * @tc.name: OsalGetList002
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_EMPTY);
}
/**
  * @tc.name: OsalGetList003
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_COUNT_CHECK);
}
/**
  * @tc.name: OsalGetList004
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_TAIL);
}
/**
  * @tc.name: OsalGetList005
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_HEAD);
}
/**
  * @tc.name: OsalGetList006
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_REMOVE);
}
/**
  * @tc.name: OsalGetList007
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_TRAVERSAL);
}
/**
  * @tc.name: OsalGetList008
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_TRAVERSAL_REMOVE);
}
/**
  * @tc.name: OsalGetList009
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetList009, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_STRESS);
}

/**
  * @tc.name: OsalGetSem001
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTestPosix, OsalGetSem001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_CREATE);
}
/**
  * @tc.name: OsalGetSem002
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTestPosix, OsalGetSem002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_POST);
}
/**
  * @tc.name: OsalGetSem003
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTestPosix, OsalGetSem003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_POST_RESULT);
}
/**
  * @tc.name: OsalGetSem004
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTestPosix, OsalGetSem004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_WAIT_TIMEOUT);
}
/**
  * @tc.name: OsalGetSem005
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTestPosix, OsalGetSem005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_WAIT_FOREVER);
}
/**
  * @tc.name: OsalGetSem006
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTestPosix, OsalGetSem006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_DESTROY);
}
/**
  * @tc.name: OsalGetSem007
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTestPosix, OsalGetSem007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_VISIT_AFTER_DESTROY);
}

/**
  * @tc.name: OsalGetAll001
  * @tc.desc: all function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTestPosix, OsalGetAll001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_ALL);
}
