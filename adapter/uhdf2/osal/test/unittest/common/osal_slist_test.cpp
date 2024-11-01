/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <thread>
#include <unistd.h>
#include <vector>
#include <gtest/gtest.h>
#include "hdf_core_log.h"
#include "hdf_slist.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG   osal_slist_test_cpp

namespace OHOS {
using namespace testing::ext;
using HdfSListOps = void (*)(struct HdfSList *list, struct HdfSListNode *link);

class HdfOsalSlistTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfOsalSlistTest::SetUpTestCase()
{
}

void HdfOsalSlistTest::TearDownTestCase()
{
}

void HdfOsalSlistTest::SetUp()
{
}

void HdfOsalSlistTest::TearDown()
{
}

struct TestList {
    struct HdfSListNode entry;
    uint32_t data;
};

void TestListDeleter(struct HdfSListNode *listNode)
{
    struct TestList *data = reinterpret_cast<struct TestList *>(listNode);
    if (data != nullptr) {
        data->data = 0;
        OsalMemFree(data);
    }
}

bool HdfTestSListSearchCompare(struct HdfSListNode *entry, uint32_t data)
{
    struct TestList *node = reinterpret_cast<struct TestList *>(entry);
    if (node->data == data) {
        return true;
    }
    return false;
}

/*
* @tc.name: SlistInitTest001
* @tc.desc: Slist init
* @tc.type: FUNC
* @tc.require: AR000DT1TK
*/
HWTEST_F(HdfOsalSlistTest, SlistInitTest001, TestSize.Level1)
{
    struct HdfSList list;
    HdfSListInit(&list);
    EXPECT_EQ(NULL, list.root);
    EXPECT_TRUE(HdfSListIsEmpty(&list));
}

static void HdfOsalSlistTestInit(struct HdfSList *list, int &totalCount, HdfSListOps ops)
{
    HdfSListInit(list);
    uint32_t insertData[] = {3, 1, 4, 5};
    totalCount = static_cast<int>(sizeof(insertData) / sizeof(insertData[0]));
    for (int i = 0; i < totalCount; i++) {
        struct TestList *testData = reinterpret_cast<struct TestList *>(OsalMemAlloc(sizeof(struct TestList)));
        ASSERT_NE(testData, nullptr);
        testData->data = insertData[i];
        ops(list, &testData->entry);
    }
    EXPECT_EQ(totalCount, HdfSListCount(list));
}

/*
* @tc.name: SlistCountTest001
* @tc.desc: Slist init
* @tc.type: FUNC
* @tc.require: AR000DT1TK
*/
HWTEST_F(HdfOsalSlistTest, SlistCountTest001, TestSize.Level1)
{
    struct HdfSList list;
    int totalCount = 0;
    HdfOsalSlistTestInit(&list, totalCount, HdfSListAdd);
    HdfSListFlush(&list, TestListDeleter);
    EXPECT_EQ(0, HdfSListCount(&list));
}

/*
* @tc.name: SlistSearchTest001
* @tc.desc: Slist init
* @tc.type: FUNC
* @tc.require: AR000DT1TK
*/
HWTEST_F(HdfOsalSlistTest, SlistSearchTest001, TestSize.Level1)
{
    struct HdfSList list;
    int totalCount = 0;
    HdfOsalSlistTestInit(&list, totalCount, HdfSListAdd);
    struct HdfSListNode *resultNode = HdfSListSearch(&list, 5, HdfTestSListSearchCompare);
    EXPECT_TRUE(resultNode != nullptr);
    EXPECT_EQ(5, static_cast<int>((reinterpret_cast<struct TestList *>(resultNode))->data));
    HdfSListFlush(&list, TestListDeleter);
    EXPECT_EQ(0, HdfSListCount(&list));
}

/*
* @tc.name: SlistGetTest001
* @tc.desc: Slist init
* @tc.type: FUNC
* @tc.require: AR000DT1TK
*/
HWTEST_F(HdfOsalSlistTest, SlistGetTest001, TestSize.Level1)
{
    struct HdfSList list;
    int totalCount = 0;
    HdfOsalSlistTestInit(&list, totalCount, HdfSListAdd);
    struct HdfSListNode *resultNode = HdfSListGetLast(&list);
    EXPECT_TRUE(resultNode != nullptr);
    EXPECT_EQ(3, static_cast<int>((reinterpret_cast<struct TestList *>(resultNode))->data));
    HdfSListFlush(&list, TestListDeleter);
    EXPECT_EQ(0, HdfSListCount(&list));

    HdfOsalSlistTestInit(&list, totalCount, HdfSListAddTail);
    resultNode = HdfSListGetLast(&list);
    EXPECT_TRUE(resultNode != nullptr);
    EXPECT_EQ(5, static_cast<int>((reinterpret_cast<struct TestList *>(resultNode))->data));
    HdfSListFlush(&list, TestListDeleter);
    EXPECT_EQ(0, HdfSListCount(&list));
}
} // namespace OHOS
