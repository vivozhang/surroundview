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
#ifndef YUYV_H
#define YUYV_H
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
static inline float clamp(float val, float mn, float mx) {
  return (val >= mn) ? ((val <= mx) ? val : mx) : mn;
}

static void YUVToRGB(const unsigned char y, const unsigned char u,
                     const unsigned char v, unsigned char *r, unsigned char *g,
                     unsigned char *b) {
  const int y1 = static_cast<int>(y) - 16;
  const int u1 = static_cast<int>(u) - 128;
  const int v1 = static_cast<int>(v) - 128;

  *r = clamp(1.164f * y1 + 2.018f * v1, 0.0f, 255.0f);
  *g = clamp(1.164f * y1 - 0.813f * u1 - 0.391f * v1, 0.0f, 255.0f);
  *b = clamp(1.164f * y1 + 1.596f * u1, 0.0f, 255.0f);
}

static void YUYVToRGB(unsigned char *yuv, int w, int h) {
  int num_pixels = w * h;
  unsigned char *bgr =
      (unsigned char *)calloc(num_pixels * 3, sizeof(unsigned char));
  memset(bgr, 0, num_pixels * 3 * sizeof(char));

  int i, j;
  unsigned char y0, y1, u, v;
  unsigned char r, g, b;
  for (i = 0, j = 0; i < (num_pixels << 1); i += 4, j += 6) {
    y0 = yuv[i + 0];
    u = yuv[i + 1];
    y1 = yuv[i + 2];
    v = yuv[i + 3];

    YUVToRGB(y0, u, v, &r, &g, &b);
    bgr[j + 0] = b;
    bgr[j + 1] = g;
    bgr[j + 2] = r;

    YUVToRGB(y1, u, v, &r, &g, &b);
    bgr[j + 3] = b;
    bgr[j + 4] = g;
    bgr[j + 5] = r;
  }
  cv::Mat img(h, w, CV_8UC3, bgr);
  cv::imshow("img", img);
  cv::waitKey();
  delete[] bgr;
}

void showYUYV(unsigned char **outbuf) {
  for (size_t i = 0; i < 5; i++) {
    YUYVToRGB(outbuf[i], 1280, 720);
  }
}

void loadImage(unsigned char **outbuf) {
  cv::Mat src = cv::imread("../data/front.png", cv::IMREAD_COLOR);
  // cv::imshow("111",src);
  // cv::waitKey(0);
  cv::Mat dst;
  cv::cvtColor(src, dst, cv::COLOR_BGR2BGRA);
  outbuf[0] = dst.data;

  cv::Mat src1 = cv::imread("../data/back.png", cv::IMREAD_COLOR);
  cv::Mat dst1;
  cv::cvtColor(src1, dst1, cv::COLOR_BGR2BGRA);
  outbuf[1] = dst1.data;

  cv::Mat src2 = cv::imread("../data/left.png", cv::IMREAD_COLOR);
  cv::Mat dst2;
  cv::cvtColor(src2, dst2, cv::COLOR_BGR2BGRA);
  outbuf[2] = dst2.data;

  cv::Mat src3 = cv::imread("../data/right.png", cv::IMREAD_COLOR);
  cv::Mat dst3;
  cv::cvtColor(src3, dst3, cv::COLOR_BGR2BGRA);
  outbuf[3] = dst3.data;

  cv::Mat src4 = cv::imread("../data/daoche.png", cv::IMREAD_COLOR);
  cv::Mat dst4;
  cv::cvtColor(src4, dst4, cv::COLOR_BGR2BGRA);
  outbuf[4] = dst4.data;
}

void saveImages(unsigned char** input,std::string dirPath){
  std::string imagePath[CAMERA_NUM]={"front","back","left","right","daoche"};
  static int index=0;
  for (size_t i = 0; i < CAMERA_NUM; i++) {
    cv::Mat imgbuf = cv::Mat(imgHeight, imgWidth, CV_8UC4, input[i]);
    cv::Mat mrgba(imgHeight, imgWidth, CV_8UC3);
    cv::cvtColor(imgbuf, mrgba, cv::COLOR_RGBA2BGR);
    std::string filename=dirPath+imagePath[i]+std::to_string(index)+".png";
    cv::imwrite(filename, mrgba);
  }
  index++;
}

#endif
