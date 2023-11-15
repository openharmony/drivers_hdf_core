/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_clock_entry_test.h"
#include "clock_test.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hdf_clock_entry_test

int32_t HdfClockTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        HDF_LOGE("HdfClockTestEntry: msg is null!");
        return HDF_FAILURE;
    }

    msg->result = ClockTestExecute(msg->subCmd);

    return HDF_SUCCESS;
}