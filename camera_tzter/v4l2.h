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
#ifndef _CV_LIB_V4L2_H
#define _CV_LIB_V4L2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h> /* getopt_long() */
#include <fcntl.h>  /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <stdbool.h>
#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
  void *start;
  size_t length;
};

struct v4l2_device {
  char *dev_name;  ///< The name of the device
  int fd;          ///< The file pointer to the device
  int w;           ///< The width of the image
  int h;           ///< The height of the image
  int buffers_cnt;
  struct buffer *buffers;
};

bool initcamera(struct v4l2_device *vd, int *length);
bool readframe(struct v4l2_device *vd, struct v4l2_buffer *buf);
bool releaseframe(struct v4l2_device *vd, struct v4l2_buffer *buf);
bool stop(struct v4l2_device *vd);
#endif
