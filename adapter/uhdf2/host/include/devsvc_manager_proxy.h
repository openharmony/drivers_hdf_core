/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DEVSVC_MANAGER_PROXY_H
#define DEVSVC_MANAGER_PROXY_H

#include "hdf_remote_service.h"
#include "devsvc_manager_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct DevSvcManagerProxy {
    struct IDevSvcManager pvtbl;
    struct HdfDeathRecipient recipient;
    struct HdfRemoteService *remote;
    uint32_t hostId;
};

enum {
    DEVSVC_MANAGER_ADD_SERVICE = 1,
    DEVSVC_MANAGER_UPDATE_SERVICE,
    DEVSVC_MANAGER_GET_SERVICE,
    DEVSVC_MANAGER_REGISTER_SVCLISTENER,
    DEVSVC_MANAGER_UNREGISTER_SVCLISTENER,
    DEVSVC_MANAGER_LIST_ALL_SERVICE,
    DEVSVC_MANAGER_LIST_SERVICE,
    DEVSVC_MANAGER_REMOVE_SERVICE,
    DEVSVC_MANAGER_LIST_SERVICE_BY_INTERFACEDESC,
};

struct HdfObject *DevSvcManagerProxyCreate(void);
void DevSvcManagerProxyRelease(struct HdfObject *object);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEVSVC_MANAGER_PROXY_H */
