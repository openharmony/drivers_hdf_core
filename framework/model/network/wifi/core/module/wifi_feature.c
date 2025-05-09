/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "wifi_module.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

#ifdef __cplusplus
extern "C" {
#endif

int32_t AddFeature(struct WifiModule *module, uint16_t featureType, struct WifiFeature *feature)
{
    if ((module == NULL) || (feature == NULL) || (featureType >= HDF_WIFI_FEATURE_NUM)) {
        HDF_LOGE("%s: para error", __func__);
        return HDF_FAILURE;
    }
    module->feList.fe[featureType] = (struct WifiFeature *)feature;
    if (feature->init != NULL) {
        HDF_LOGD("%s: AddFeature finished!", __func__);
        return feature->init(feature);
    }

    HDF_LOGE("%s: feature has no init", __func__);
    return HDF_FAILURE;
}

int32_t DelFeature(struct WifiModule *module, uint16_t featureType)
{
    struct WifiFeature *featureData = NULL;
    if ((module == NULL) || (featureType >= HDF_WIFI_FEATURE_NUM)) {
        HDF_LOGE("%s: para error", __func__);
        return HDF_FAILURE;
    }

    featureData = module->feList.fe[featureType];
    if ((featureData != NULL) && (featureData->deInit != NULL)) {
        featureData->deInit(module->feList.fe[featureType]);
        featureData = NULL;
        HDF_LOGD("%s: DelFeature finished!", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: DelFeature fail!", __func__);
    return HDF_FAILURE;
}

#ifdef __cplusplus
}
#endif
