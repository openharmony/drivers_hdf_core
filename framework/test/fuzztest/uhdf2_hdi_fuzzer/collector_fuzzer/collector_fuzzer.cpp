/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "collector_fuzzer.h"
#include "object_collector.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "parcel.h"
#include <string>
#include "osal_mem.h"
#include "hdi_support.h"
#include <string_ex.h>

namespace OHOS {
static void CollectionFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        HDF_LOGE("%{public}s: data is nullptr!", __func__);
    }

    Parcel parcel;
    parcel.WriteBuffer(data, size);
    const std ::string a = parcel.ReadString();
    const std::u16string INTERFACE_NAME = OHOS::Str8ToStr16(parcel.ReadString());
    const char *desc = a.c_str();
    HDI::HdiBase *service = (HDI::HdiBase *)OsalMemCalloc(sizeof(HDI::HdiBase));
    HDI::ObjectCollector *collector;
    collector->NewObject(service, INTERFACE_NAME);
    collector->GetOrNewObject(service, INTERFACE_NAME);
    collector->RemoveObject(service);
    OsalMemFree(service);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::CollectionFuzzTest(data, size);
    return HDF_SUCCESS;
}