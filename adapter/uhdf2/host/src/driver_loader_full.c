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

#include "driver_loader_full.h"

#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>

#include "securec.h"

#include "hdf_core_log.h"
#include "hdf_object_manager.h"
#include "osal_mem.h"

#define DRIVER_DESC "driverDesc"
#define HDF_LOG_TAG driver_loader_full

static struct DriverLoaderFull *g_fullLoader = NULL;

struct HdfDriver *HdfDriverLoaderGetDriver(const char *moduleName)
{
    if (moduleName == NULL) {
        return NULL;
    }

    struct HdfDriver *driver = OsalMemAlloc(sizeof(struct HdfDriver));
    if (driver == NULL) {
        return NULL;
    }

    void *handle = dlopen(moduleName, RTLD_LAZY);
    if (handle == NULL) {
        HDF_LOGE("get driver handle, %{public}s dlopen failed, %{public}s", moduleName, dlerror());
        OsalMemFree(driver);
        return NULL;
    }

    struct HdfDriverEntry **driverEntry = (struct HdfDriverEntry **)dlsym(handle, DRIVER_DESC);
    if (driverEntry == NULL) {
        HDF_LOGE("get driver entry %{public}s dlsym failed", moduleName);
        dlclose(handle);
        OsalMemFree(driver);
        return NULL;
    }

    driver->entry = *driverEntry;
    driver->priv = handle;

    return driver;
}

void HdfDriverLoaderFullReclaimDriver(struct HdfDriver *driver)
{
    if (driver == NULL) {
        return;
    }

    OsalMemFree(driver);
}

void HdfDriverLoaderFullConstruct(struct DriverLoaderFull *inst)
{
    struct HdfDriverLoader *pvtbl = (struct HdfDriverLoader *)inst;
    pvtbl->super.GetDriver = HdfDriverLoaderGetDriver;
    pvtbl->super.ReclaimDriver = HdfDriverLoaderFullReclaimDriver;
}

struct HdfObject *HdfDriverLoaderFullCreate(void)
{
    if (g_fullLoader == NULL) {
        struct DriverLoaderFull *instance =
            (struct DriverLoaderFull *)OsalMemCalloc(sizeof(struct DriverLoaderFull));
        if (instance != NULL) {
            HdfDriverLoaderFullConstruct(instance);
            g_fullLoader = instance;
        }
    }
    return (struct HdfObject *)g_fullLoader;
}

void HdfDriverLoaderFullRelease(struct HdfObject *object)
{
    struct DriverLoaderFull *instance = (struct DriverLoaderFull *)object;
    if (instance == g_fullLoader) {
        g_fullLoader = NULL;
    }
    if (instance != NULL) {
        OsalMemFree(instance);
    }
}

struct IDriverLoader *HdfDriverLoaderGetInstance(void)
{
    static struct IDriverLoader *instance = NULL;
    if (instance == NULL) {
        instance = (struct IDriverLoader *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DRIVER_LOADER);
    }
    return instance;
}
