/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_host.h"
#include "audio_codec_if.h"
#include "audio_core.h"
#include "audio_sapm.h"
#include "audio_driver_log.h"
#include "audio_parse.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

AUDIO_LIST_HEAD(g_cardManager);

/* Get all sound card instance */
const struct DListHead *GetAllCardInstance(void)
{
    if (DListIsEmpty(&g_cardManager)) {
        ADM_LOG_ERR("g_cardManager is empty.");
        return NULL;
    }

    return &g_cardManager;
}

/* Get a sound card instance */
struct AudioCard *GetCardInstance(const char *serviceName)
{
    struct AudioCard *audioCard = NULL;

    if (serviceName == NULL) {
        ADM_LOG_ERR("serviceName is NULL.");
        return NULL;
    }

    if (DListIsEmpty(&g_cardManager)) {
        ADM_LOG_ERR("g_cardManager is empty.");
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY(audioCard, &g_cardManager, struct AudioCard, list) {
        if (audioCard->configData.cardServiceName == NULL) {
            ADM_LOG_ERR("cardServiceName is NULL.");
            return NULL;
        }

        if (strcmp(audioCard->configData.cardServiceName, serviceName) == 0) {
            return audioCard;
        }
    }
    ADM_LOG_ERR("serviceName is %s.", serviceName);
    return NULL;
}

static int32_t AudioCodecDevInit(struct AudioCard *audioCard)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct CodecDevice *codec = NULL;
    ADM_LOG_DEBUG("entry.");

    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard is NULL.");
        return HDF_ERR_IO;
    }
    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("rtd is NULL.");
        return HDF_ERR_IO;
    }
    codec = rtd->codec;
    if (codec != NULL && codec->devData != NULL && codec->devData->Init != NULL) {
        /* codec initialization */
        int32_t ret = codec->devData->Init(audioCard, codec);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("codec initialization fail ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioPlatformDevInit(const struct AudioCard *audioCard)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct PlatformDevice *platform = NULL;

    if (audioCard == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_ERR_IO;
    }
    ADM_LOG_DEBUG("entry.");

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("Platform rtd is NULL.");
        return HDF_ERR_IO;
    }
    platform = rtd->platform;
    if (platform != NULL && platform->devData != NULL && platform->devData->PlatformInit != NULL) {
        /* platform initialization */
        int32_t ret = platform->devData->PlatformInit(audioCard, platform);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("platform initialization fail ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioDspDevInit(const struct AudioCard *audioCard)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct DspDevice *dsp = NULL;

    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard is NULL.");
        return HDF_ERR_IO;
    }
    ADM_LOG_DEBUG("entry.");

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("audioCard rtd object is NULL.");
        return HDF_ERR_IO;
    }

    dsp = rtd->dsp;
    if (dsp != NULL && dsp->devData != NULL && dsp->devData->DspInit != NULL) {
        /* dsp initialization */
        int32_t ret = dsp->devData->DspInit(dsp);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("dsp initialization fail ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioCodecDaiDevInit(struct AudioCard *audioCard)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct DaiDevice *codecDai = NULL;

    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard is NULL.");
        return HDF_ERR_IO;
    }
    ADM_LOG_DEBUG("entry.");

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("CodecDai rtd is NULL.");
        return HDF_ERR_IO;
    }
    codecDai = rtd->codecDai;
    if (codecDai != NULL && codecDai->devData != NULL && codecDai->devData->DaiInit != NULL) {
        /* codec dai initialization */
        int32_t ret = codecDai->devData->DaiInit(audioCard, codecDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("codec dai initialization fail ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioCpuDaiDevInit(struct AudioCard *audioCard)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct DaiDevice *cpuDai = NULL;

    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard is NULL.");
        return HDF_ERR_IO;
    }
    ADM_LOG_DEBUG("entry.");

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("cpuDai rtd is NULL.");
        return HDF_ERR_IO;
    }
    cpuDai = rtd->cpuDai;
    if (cpuDai != NULL && cpuDai->devData != NULL && cpuDai->devData->DaiInit != NULL) {
        /* cpu dai initialization */
        int32_t ret = cpuDai->devData->DaiInit(audioCard, cpuDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("cpu dai initialization fail ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioDspDaiDevInit(struct AudioCard *audioCard)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct DaiDevice *dspDai = NULL;
    int32_t ret;

    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard is NULL.");
        return HDF_ERR_IO;
    }
    ADM_LOG_DEBUG("dsp init entry.");

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("dspDai rtd is NULL.");
        return HDF_ERR_IO;
    }

    dspDai = rtd->dspDai;
    if (dspDai != NULL && dspDai->devData != NULL && dspDai->devData->DaiInit != NULL) {
        ret = dspDai->devData->DaiInit(audioCard, dspDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("dsp dai initialization fail ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioInitDaiLink(struct AudioCard *audioCard)
{
    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard is NULL.");
        return HDF_ERR_IO;
    }
    ADM_LOG_DEBUG("entry.");

    if (AudioPlatformDevInit(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("Platform init fail.");
        return HDF_FAILURE;
    }

    if (AudioCpuDaiDevInit(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("CpuDai init fail.");
        return HDF_FAILURE;
    }
    if (AudioCodecDevInit(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("codec Device init fail.");
        return HDF_FAILURE;
    }

    if (AudioCodecDaiDevInit(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("CodecDai Device init fail.");
        return HDF_FAILURE;
    }

    if (AudioDspDevInit(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("Dsp Device init fail.");
        return HDF_FAILURE;
    }

    if (AudioDspDaiDevInit(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("DspDai Device init fail.");
        return HDF_FAILURE;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

struct AudioHost *AudioHostCreateAndBind(struct HdfDeviceObject *device)
{
    struct AudioHost *audioHost = NULL;

    if (device == NULL) {
        ADM_LOG_ERR("device is NULL!");
        return NULL;
    }

    audioHost = (struct AudioHost *)OsalMemCalloc(sizeof(*audioHost));
    if (audioHost == NULL) {
        ADM_LOG_ERR("Malloc audio host fail!");
        return NULL;
    }
    audioHost->device = device;
    device->service = &audioHost->service;
    return audioHost;
}

/* HdfDriverEntry implementations */
static int32_t AudioDriverBind(struct HdfDeviceObject *device)
{
    struct AudioHost *audioHost = NULL;

    ADM_LOG_DEBUG("entry.");
    if (device == NULL) {
        ADM_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    audioHost = AudioHostCreateAndBind(device);
    if (audioHost == NULL) {
        ADM_LOG_ERR("audioHost create failed!");
        return HDF_FAILURE;
    }

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioCardInit(struct HdfDeviceObject *device, struct AudioHost *audioHost)
{
    int32_t ret;
    struct AudioCard *audioCard = NULL;
    if (device == NULL || audioHost == NULL) {
        ADM_LOG_ERR("device or audioHost is NULL.");
        return HDF_FAILURE;
    }

    audioCard = (struct AudioCard *)OsalMemCalloc(sizeof(*audioCard));
    if (audioCard == NULL) {
        ADM_LOG_ERR("Malloc audioCard fail!");
        return HDF_FAILURE;
    }
    audioHost->priv = audioCard;
    /* Get node information through HCS file */
    ret = AudioFillConfigData(device, &(audioCard->configData));
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("AudioFillConfigData fail ret=%d", ret);
        return HDF_ERR_IO;
    }

    audioCard->device = device;
    audioCard->standbyMode = AUDIO_SAPM_TURN_STANDBY_LATER;

    /* Bind specific codec、platform and dai device */
    ret = AudioBindDaiLink(audioCard, &(audioCard->configData));
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("AudioBindDaiLink fail ret=%d", ret);
        return HDF_FAILURE;
    }
    if (audioCard->rtd == NULL || (!audioCard->rtd->complete)) {
        ADM_LOG_ERR("AudioBindDaiLink fail!");
        return HDF_ERR_IO;
    }

    /* Initialize controls list */
    DListHeadInit(&audioCard->list);
    DListHeadInit(&audioCard->controls);
    DListHeadInit(&audioCard->components);
    DListHeadInit(&audioCard->paths);
    DListHeadInit(&audioCard->sapmDirty);
    /* Initialize hardware device */
    ret = AudioInitDaiLink(audioCard);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("AudioInitDaiLink fail ret=%d", ret);
        return HDF_ERR_IO;
    }

    /* sound card added to list */
    DListInsertHead(&audioCard->list, &g_cardManager);

    return HDF_SUCCESS;
}

static int32_t AudioDriverInit(struct HdfDeviceObject *device)
{
    struct AudioHost *audioHost = NULL;

    ADM_LOG_DEBUG("entry.");
    if (device == NULL) {
        ADM_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (!HdfDeviceSetClass(device, DEVICE_CLASS_AUDIO)) {
        ADM_LOG_ERR("set audio class failed.");
        return HDF_FAILURE;
    }
    audioHost = (struct AudioHost *)device->service;
    if (AudioCardInit(device, audioHost) != HDF_SUCCESS) {
        ADM_LOG_ERR("audio card init failed.");
        return HDF_FAILURE;
    }
    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static void AudioDriverRelease(struct HdfDeviceObject *device)
{
    struct AudioHost *audioHost = NULL;
    struct AudioCard *audioCard = NULL;
    struct DListHead *componentHead = NULL;
    struct DListHead *controlHead = NULL;
    struct AudioSapmComponent *componentReq = NULL;
    struct AudioSapmComponent *componentTmp = NULL;
    struct AudioKcontrol *ctrlReq = NULL;
    struct AudioKcontrol *ctrlTmp = NULL;

    ADM_LOG_DEBUG("entry.");
    if (device == NULL) {
        ADM_LOG_ERR("device is NULL.");
        return;
    }
    audioHost = (struct AudioHost *)device->service;
    if (audioHost == NULL) {
        ADM_LOG_ERR("audioHost is NULL.");
        return;
    }

    if (audioHost->priv != NULL) {
        audioCard = (struct AudioCard *)audioHost->priv;

        componentHead = &audioCard->components;
        DLIST_FOR_EACH_ENTRY_SAFE(componentReq, componentTmp, componentHead, struct AudioSapmComponent, list) {
            DListRemove(&componentReq->list);
            OsalMemFree(componentReq->componentName);
            OsalMemFree(componentReq);
        }

        controlHead = &audioCard->controls;
        DLIST_FOR_EACH_ENTRY_SAFE(ctrlReq, ctrlTmp, controlHead, struct AudioKcontrol, list) {
            DListRemove(&ctrlReq->list);
            OsalMemFree(ctrlReq->privateData);
            OsalMemFree(ctrlReq);
        }

        DListRemove(&audioCard->list);
        OsalMemFree(audioCard->rtd);
        OsalMemFree(audioHost->priv);
    }

    OsalMemFree(audioHost);

    ADM_LOG_INFO("success.");
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_audioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_AUDIO",
    .Bind = AudioDriverBind,
    .Init = AudioDriverInit,
    .Release = AudioDriverRelease,
};
HDF_INIT(g_audioDriverEntry);
