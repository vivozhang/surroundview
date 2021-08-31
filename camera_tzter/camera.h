/******************************************************************************
 * Copyright 2017 The SANY Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
/**
 * @file peripherals/video_device.h
 */

#ifndef VIDEO_DEVICE_H
#define VIDEO_DEVICE_H

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "modules/surroundview/camera_tzter/v4l2.h"
#define CAMERA_NUMS 5
#define PIX_W 1280
#define PIX_H 720
#define BUF_CNT 4
static char *dev_names[CAMERA_NUMS] =
    {"/dev/video0", "/dev/video1", "/dev/video2", "/dev/video3", "/dev/video4"};
static struct v4l2_buffer wholeimages[CAMERA_NUMS];
static struct v4l2_device vct[CAMERA_NUMS];
static unsigned char *yuyv[CAMERA_NUMS] = {0};

#ifdef __cplusplus
extern "C" {
#endif
    int InitCameras();
    int GetPixels(unsigned char **image);
    int DestoryCameras();
    int ReleasePixels();
#ifdef __cplusplus
}
#endif
#endif
