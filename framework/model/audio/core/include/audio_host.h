/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_HOST_H
#define AUDIO_HOST_H

#include "hdf_base.h"
#include "hdf_dlist.h"
#include "hdf_device_desc.h"
#include "hdf_core_log.h"
#include "device_resource_if.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AUDIO_LIST_HEAD_INIT(name) { &(name), &(name) }

#define AUDIO_LIST_HEAD(name) \
    struct DListHead name = AUDIO_LIST_HEAD_INIT(name)

enum AudioFormat {
    AUDIO_FORMAT_TYPE_PCM_8_BIT  = 0x1u,       /**< audio 8-bit PCM */
    AUDIO_FORMAT_TYPE_PCM_16_BIT = 0x2u,       /**< audio16-bit PCM */
    AUDIO_FORMAT_TYPE_PCM_24_BIT = 0x3u,       /**< audio 24-bit PCM */
    AUDIO_FORMAT_TYPE_PCM_32_BIT = 0x4u,       /**< audio 32-bit PCM */
    AUDIO_FORMAT_TYPE_AAC_MAIN   = 0x1000001u, /**< audio AAC main */
    AUDIO_FORMAT_TYPE_AAC_LC     = 0x1000002u, /**< audio AAC LC */
    AUDIO_FORMAT_TYPE_AAC_LD     = 0x1000003u, /**< audio AAC LD */
    AUDIO_FORMAT_TYPE_AAC_ELD    = 0x1000004u, /**< audio AAC ELD */
    AUDIO_FORMAT_TYPE_AAC_HE_V1  = 0x1000005u, /**< audio AAC HE_V1 */
    AUDIO_FORMAT_TYPE_AAC_HE_V2  = 0x1000006u, /**< audio AAC HE_V2 */
    AUDIO_FORMAT_TYPE_G711A      = 0x2000001u, /**< audio G711A */
    AUDIO_FORMAT_TYPE_G711U      = 0x2000002u, /**< audio G711u */
    AUDIO_FORMAT_TYPE_G726       = 0x2000003u, /**< audio G726 */
};

typedef enum {
    AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8kHz sample_rate */
    AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12kHz sample_rate */
    AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025kHz sample_rate */
    AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16kHz sample_rate */
    AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050kHz sample_rate */
    AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24kHz sample_rate */
    AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32kHz sample_rate */
    AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1kHz sample_rate */
    AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48kHz sample_rate */
    AUDIO_SAMPLE_RATE_64000  = 64000,   /* 64kHz sample_rate */
    AUDIO_SAMPLE_RATE_96000  = 96000,   /* 96kHz sample_rate */
    AUDIO_SAMPLE_RATE_BUTT,
} AudioSampleRate;

enum AuidoBitWidth {
    BIT_WIDTH8  =  8,      /* 8 bit witdth */
    BIT_WIDTH16 =  16,     /* 16 bit witdth */
    BIT_WIDTH18 =  18,     /* 18 bit witdth */
    BIT_WIDTH20 =  20,     /* 20 bit witdth */
    BIT_WIDTH24 =  24,     /* 24 bit witdth */
    BIT_WIDTH32 =  32,     /* 32 bit witdth */
};

struct AudioConfigData {
    const char *cardServiceName;
    const char *codecName;
    const char *platformName;
    const char *cpuDaiName;
    const char *codecDaiName;
    const char *dspName;
    const char *dspDaiName;
};

enum AudioSapmTurnStandbyMode {
    AUDIO_SAPM_TURN_STANDBY_LATER = 0,
    AUDIO_SAPM_TURN_STANDBY_NOW,
    AUDIO_SAPM_TURN_STANDBY_BUTT,
};

struct AudioCard {
    struct HdfDeviceObject *device;
    struct AudioRuntimeDeivces *rtd;
    struct AudioConfigData configData;

    /* Card-specific routes and components. */
    const struct AudioSapmComponent *sapmComponents;
    int32_t sapmComponentsNum;
    const struct AudioSapmRoute *sapmRoutes;
    int32_t sapmRoutesNum;

    struct DListHead list;
    struct DListHead controls; /* all controls for this card */
    struct DListHead components; /* all components for this card */
    struct DListHead paths; /* all paths for this card */
    struct DListHead sapmDirty; /* all dirty for this card */
    enum AudioSapmTurnStandbyMode standbyMode;
    uint64_t time;
    bool sapmSleepState;
    bool sapmStandbyState;
    bool sapmMonitorState;
    bool sapmStandbyStartTimeFlag;
    bool sapmSleepStartTimeFlag;
};

enum CriBuffStatus {
    ENUM_CIR_BUFF_NORMAL    = -1,
    ENUM_CIR_BUFF_FULL      = -2,
    ENUM_CIR_BUFF_EMPTY     = -3,
};

enum AudioStreamType {
    AUDIO_CAPTURE_STREAM = 0,
    AUDIO_RENDER_STREAM,
};

enum AudioStreamDirection {
    AUDIO_RENDER_STREAM_OUT = 0,
    AUDIO_CAPTURE_STREAM_IN,
};

enum AudioPortDirection {
    PORT_OUT    = 0x1u, /**< Output port */
    PORT_IN     = 0x2u, /**< Input port */
    PORT_OUT_IN = 0x3u, /**< Input/output port, supporting both audio input and output */
};

/* SoC PCM stream information */
struct AudioPcmStream {
    uint64_t portDirection;
    /* SNDRV_PCM_FMTBIT_* */
    uint64_t formats;
    /* SNDRV_PCM_RATE_* */
    uint64_t rates;
    /* min rate */
    uint64_t rateMin;
    /* max rate */
    uint64_t rateMax;
    /* min channels */
    uint64_t channelsMin;
    /* max channels */
    uint64_t channelsMax;
    /* max buffer size */
    uint64_t bufferBytesMax;
    /* min period size */
    uint64_t periodBytesMin;
    /* max period size */
    uint64_t periodBytesMax;
    /* min # of periods */
    uint64_t periodsMin;
    /* max # of periods */
    uint64_t periodsMax;
};

struct AudioPortInfo {
    struct AudioPcmStream render;
    struct AudioPcmStream capture;
};

struct PcmInfo {
    enum AudioStreamType streamType;
    /* The number of channels in a frame */
    uint32_t channels;
    /* The number of frames per second */
    uint32_t rate;
    uint32_t bitWidth;
    uint32_t frameSize;
    bool isBigEndian;
    bool isSignedData;
    uint32_t startThreshold;
    uint32_t stopThreshold;
    uint32_t silenceThreshold;
    uint32_t totalStreamSize;
    uint32_t interleaved;
};

struct AudioPcmHwParams {
    /* The stream type in a frame */
    enum AudioStreamType streamType;
    /* The number of channels in a frame */
    uint32_t channels;
    /* The number of frames per second */
    uint32_t rate;
    /* The number of frames in a period */
    uint32_t periodSize;
    /* The number of periods in a PCM */
    uint32_t periodCount;
    /* The sample format of a PCM */
    enum AudioFormat format; /* < Audio data format. For details, see {@link AudioFormat}. */
    const char *cardServiceName;
    uint32_t period;
    uint32_t frameSize;
    bool isBigEndian;
    bool isSignedData;
    uint32_t startThreshold;
    uint32_t stopThreshold;
    uint32_t silenceThreshold;
};

struct AudioHost {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    void *priv;
};

struct AudioTxData {
    int32_t status;
    char *buf; /* buf address */
    unsigned long frames; /* frames number */
};

struct AudioRxData {
    int32_t status;
    char *buf;
    unsigned long bufSize;
    unsigned long frames; /* frames number */
};

struct AudioMmapData {
    void *memoryAddress;                 /**< Pointer to the mmap buffer */
    int32_t memoryFd;                    /**< File descriptor of the mmap buffer */
    int32_t totalBufferFrames;           /**< Total size of the mmap buffer (unit: frame )*/
    int32_t transferFrameSize;           /**< Transfer size (unit: frame) */
    int32_t isShareable;                 /**< Whether the mmap buffer can be shared among processes */
    uint32_t offset;
};

struct AudioRuntimeDeivces       {
    /* runtime devices */
    struct CodecDevice *codec;
    struct PlatformDevice *platform;
    struct DaiDevice *codecDai;
    struct DaiDevice *cpuDai;
    struct DspDevice *dsp;
    struct DaiDevice *dspDai;
    uint8_t complete;
    uint32_t frameBits;
    uint8_t *dmaArea;
    unsigned long bufferSize;
};

struct AudioHost *AudioHostCreateAndBind(struct HdfDeviceObject *device);

/* Get a sound card instance */
struct AudioCard *GetCardInstance(const char *serviceName);

/* Get all sound card instance */
const struct DListHead *GetAllCardInstance(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_HOST_H */
