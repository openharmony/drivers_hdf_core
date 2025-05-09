/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "devhost_service_clnt.h"
#include "device_token_clnt.h"
#include "devmgr_service_start.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_core_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG devhost_service_clnt

int DevHostServiceClntInstallDriver(struct DevHostServiceClnt *hostClnt)
{
    int ret;
    struct HdfSListIterator it;
    struct HdfDeviceInfo *deviceInfo = NULL;
    struct IDevHostService *devHostSvcIf = NULL;
    if (hostClnt == NULL) {
        HDF_LOGE("failed to install driver, hostClnt is null");
        return HDF_FAILURE;
    }
    OsalMutexLock(&hostClnt->hostLock);
    devHostSvcIf = (struct IDevHostService *)hostClnt->hostService;
    if (devHostSvcIf == NULL || devHostSvcIf->AddDevice == NULL) {
        OsalMutexUnlock(&hostClnt->hostLock);
        HDF_LOGE("devHostSvcIf or devHostSvcIf->AddDevice is null");
        return HDF_FAILURE;
    }
    HdfSListIteratorInit(&it, &hostClnt->unloadDevInfos);
    while (HdfSListIteratorHasNext(&it)) {
        deviceInfo = (struct HdfDeviceInfo *)HdfSListIteratorNext(&it);
        if ((deviceInfo == NULL) || (deviceInfo->preload == DEVICE_PRELOAD_DISABLE)) {
            continue;
        }
        /*
         * If quick start feature enable, the device which 'preload' attribute set as
         * DEVICE_PRELOAD_ENABLE_STEP2 will be loaded later
         */
        if (DeviceManagerIsQuickLoad() == DEV_MGR_QUICK_LOAD &&
            deviceInfo->preload == DEVICE_PRELOAD_ENABLE_STEP2) {
            continue;
        }
        ret = devHostSvcIf->AddDevice(devHostSvcIf, deviceInfo);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("failed to AddDevice %{public}s, ret = %{public}d", deviceInfo->svcName, ret);
            continue;
        }
        deviceInfo->status = HDF_SERVICE_USABLE;
#ifndef __USER__
        HdfSListIteratorRemove(&it);
        HdfDeviceInfoFreeInstance(deviceInfo);
#endif
    }
    OsalMutexUnlock(&hostClnt->hostLock);
    return HDF_SUCCESS;
}

static int32_t DevHostServiceClntConstruct(struct DevHostServiceClnt *hostClnt)
{
    HdfSListInit(&hostClnt->devices);
    HdfSListInit(&hostClnt->unloadDevInfos);
    HdfSListInit(&hostClnt->dynamicDevInfos);
    hostClnt->deviceHashMap = (Map *)OsalMemCalloc(sizeof(Map));
    if (hostClnt->deviceHashMap == NULL) {
        HDF_LOGE("%{public}s:failed to malloc deviceHashMap", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (OsalMutexInit(&hostClnt->hostLock) != HDF_SUCCESS) {
        OsalMemFree(hostClnt->deviceHashMap);
        return HDF_FAILURE;
    }
    MapInit(hostClnt->deviceHashMap);
    return HDF_SUCCESS;
}

struct DevHostServiceClnt *DevHostServiceClntNewInstance(uint16_t hostId, const char *hostName)
{
    struct DevHostServiceClnt *hostClnt = (struct DevHostServiceClnt *)OsalMemCalloc(sizeof(struct DevHostServiceClnt));
    if (hostClnt == NULL) {
        return NULL;
    }
    hostClnt->hostId = hostId;
    hostClnt->hostName = hostName;
    hostClnt->devCount = 0;
    hostClnt->hostPid = -1;
    hostClnt->stopFlag = false;
    if (DevHostServiceClntConstruct(hostClnt) != HDF_SUCCESS) {
        OsalMemFree(hostClnt);
        hostClnt = NULL;
    }
    return hostClnt;
}

void DevHostServiceClntFreeInstance(struct DevHostServiceClnt *hostClnt)
{
    if (hostClnt != NULL) {
        HdfSListFlush(&hostClnt->devices, DeviceTokenClntDelete);
        HdfSListFlush(&hostClnt->unloadDevInfos, HdfDeviceInfoDelete);
        HdfSListFlush(&hostClnt->dynamicDevInfos, HdfDeviceInfoDelete);
        OsalMemFree(hostClnt->deviceHashMap);
        OsalMutexDestroy(&hostClnt->hostLock);
        OsalMemFree(hostClnt);
    }
}

void DevHostServiceClntDelete(struct DevHostServiceClnt *hostClnt)
{
    if (hostClnt != NULL) {
        DevHostServiceClntFreeInstance(hostClnt);
    }
}

