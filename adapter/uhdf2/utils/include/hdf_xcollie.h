/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef HDF_UHDF_UTILS_INCLUDE_HDF_XCOLLIE_H
#define HDF_UHDF_UTILS_INCLUDE_HDF_XCOLLIE_H

#include <string>
#ifdef HDFHICOLLIE_ENABLE
#include "xcollie/xcollie.h"
#else
#include "singleton.h"
#endif

namespace OHOS {
constexpr uint32_t HDF_XCOLLIE_FLAG_LOG = 1; // generate log file
constexpr uint32_t HDF_XCOLLIE_FLAG_RECOVERY = 2; // die when timeout
constexpr uint32_t DEFAULT_TIMEOUT_SECONDS = 10;
#ifdef HDFHICOLLIE_ENABLE
class HdfXCollie {
    HdfXCollie(const std::string& tag, uint32_t timeoutSeconds = DEFAULT_TIMEOUT_SECONDS,
        std::function<void(void *)> func = nullptr, void* arg = nullptr, uint32_t flag = HDF_XCOLLIE_FLAG_LOG);
    ~HdfXCollie();

    void CancelHdfXCollie();

private:
    int32_t id_;
    std::string tag_;
    bool isCanceled_;
};

#else
class HdfXCollie {
public:
    HdfXCollie(const std::string& tag, uint32_t timeoutSeconds = DEFAULT_TIMEOUT_SECONDS,
        std::function<void(void *)> func = nullptr, void* arg = nullptr, uint32_t flag = HDF_XCOLLIE_FLAG_LOG);
    ~HdfXCollie();
};
#endif
}
#endif // HDF_UHDF_UTILS_INCLUDE_HDF_XCOLLIE_H