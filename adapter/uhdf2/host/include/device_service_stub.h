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

#ifndef DEVICE_SERVICE_STUB_H
#define DEVICE_SERVICE_STUB_H

#include "hdf_remote_service.h"
#include "hdf_device_node.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct IRemoteService {
    struct HdfObject object;
    int (*Dispatch)(int code, struct HdfSBuf *data, struct HdfSBuf *reply);
};

struct DeviceServiceStub {
    struct HdfDeviceNode super;
    struct HdfRemoteService *remote;
};

enum {
    IFACE_OBJECT_SERVICE = 0,
    IFACE_REMOTE_SERVICE
};

struct HdfObject *DeviceServiceStubCreate(void);
void DeviceServiceStubRelease(struct HdfObject *object);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEVICE_SERVICE_STUB_H */
