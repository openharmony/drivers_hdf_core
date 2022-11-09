/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_io.h"
#include "osal_mem.h"
#include "pwm/pwm_core.h"

#define HDF_LOG_TAG pwm_virtual
#define PWM_DEFAULT_PERIOD     0x3E7
#define PWM_DEFAULT_POLARITY   0
#define PWM_DEFAULT_DUTY_CYCLE 0x14D
#define PWM_MIN_PERIOD 666

struct VirtualPwm {
    struct PwmDev dev;
    bool supportPolarity;
};

int32_t VirtualPwmDeviceGet(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}
int32_t VirtualPwmDevicePut(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}

int32_t VirtualPwmSetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s: pwm or config is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (config->polarity != PWM_NORMAL_POLARITY && config->polarity != PWM_INVERTED_POLARITY) {
        HDF_LOGE("%s: polarity %hhu is invalid", __func__, config->polarity);
        return HDF_ERR_INVALID_PARAM;
    }
    if (config->period < PWM_MIN_PERIOD) {
        HDF_LOGE("%s: period %u is not support, min period %d", __func__, config->period, PWM_MIN_PERIOD);
        return HDF_ERR_INVALID_PARAM;
    }
    if (config->duty < 1 || config->duty > config->period) {
        HDF_LOGE("%s: duty %u is not support, duty must in [1, period = %u].",
            __func__, config->duty, config->period);
        return HDF_ERR_INVALID_PARAM;
    }
    pwm->cfg = *config;

    return HDF_SUCCESS;
}
static struct PwmMethod g_method = {
    .setConfig = VirtualPwmSetConfig,
    .open = VirtualPwmDeviceGet,
    .close = VirtualPwmDevicePut,
};

static int32_t VirtualPwmProbe(struct VirtualPwm *virtual, struct HdfDeviceObject *obj)
{
    struct DeviceResourceIface *iface = NULL;

    iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }

    if (iface->GetUint32(obj->property, "num", &(virtual->dev.num), 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read num fail", __func__);
        return HDF_FAILURE;
    }

    virtual->supportPolarity = true;
    virtual->dev.method = &g_method;
    virtual->dev.cfg.duty = PWM_DEFAULT_DUTY_CYCLE;
    virtual->dev.cfg.period = PWM_DEFAULT_PERIOD;
    virtual->dev.cfg.polarity = PWM_DEFAULT_POLARITY;
    virtual->dev.cfg.status = PWM_DISABLE_STATUS;
    virtual->dev.cfg.number = virtual->dev.num;
    virtual->dev.busy = false;
    if (PwmDeviceAdd(obj, &(virtual->dev)) != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmDeviceAdd] failed.", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: set PwmConfig: number %u, period %u, duty %u, polarity %hhu, enable %hhu.", __func__,
        virtual->dev.cfg.number, virtual->dev.cfg.period, virtual->dev.cfg.duty,
        virtual->dev.cfg.polarity, virtual->dev.cfg.status);
    return HDF_SUCCESS;
}

static int32_t VirtualPwmBind(struct HdfDeviceObject *obj)
{
    (void)obj;
    return HDF_SUCCESS;
}

static int32_t VirtualPwmInit(struct HdfDeviceObject *obj)
{
    int ret;
    struct VirtualPwm *virtual = NULL;

    if (obj == NULL) {
        HDF_LOGE("%s: obj is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    virtual = (struct VirtualPwm *)OsalMemCalloc(sizeof(*virtual));
    if (virtual == NULL) {
        HDF_LOGE("%s: OsalMemCalloc virtual error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = VirtualPwmProbe(virtual, obj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error probe, ret is %d", __func__, ret);
        OsalMemFree(virtual);
        return ret;
    }
    HDF_LOGI("%s: pwm init success", __func__);
    return ret;
}

static void VirtualPwmRelease(struct HdfDeviceObject *obj)
{
    struct VirtualPwm *virtual = NULL;

    if (obj == NULL) {
        HDF_LOGE("%s: obj is null", __func__);
        return;
    }
    virtual = (struct VirtualPwm *)obj->service;
    if (virtual == NULL) {
        HDF_LOGE("%s: virtual is null", __func__);
        return;
    }
    PwmDeviceRemove(obj, &(virtual->dev));
    OsalMemFree(virtual);
}

struct HdfDriverEntry g_virtualPwm = {
    .moduleVersion = 1,
    .moduleName = "virtual_pwm_driver",
    .Bind = VirtualPwmBind,
    .Init = VirtualPwmInit,
    .Release = VirtualPwmRelease,
};

HDF_INIT(g_virtualPwm);