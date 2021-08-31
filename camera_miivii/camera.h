#pragma once
#ifndef CAMERA_H
#define CAMERA_H 
#define OPENCV3
#include <string>
#include <iostream>
#include <exception>
#include <zconf.h>
#include "MvGmslCamera.h"
const int CAMERA_NUM = 5;                         // A、B两组总共挂载相机个数//
const int GROUP_A_CAMERA_NUM = 4;  // A组挂载相机个数
const int GROUP_B_CAMERA_NUM = 1;  // B组挂载相机个数
const int GROUP_NUM =2;                            //同时使用组的个数，如果A、B两组同时使用，就是2，如果就A组使用就是1

const   int cam_w=1280;                                //图像窗口宽度
const	int cam_h=720;                                   //图像窗口高度
const	int fps=30;                                             //相机帧率
const	int imgWidth=1280;                           //图像宽度
const	int imgHeight=720;                            //图像高度

// init camera
static miivii::MvGmslCamera* mvcam[GROUP_NUM] = {NULL};
static uint8_t* group_a_buf[GROUP_A_CAMERA_NUM];  // A组图像数据
static uint8_t* group_b_buf[GROUP_B_CAMERA_NUM];  // B组图像数据
int InitCameras();
int DestoryCameras();
void show(unsigned char** input);
int GetPixels(unsigned char** buf);
int ReleasePixels(){};
#endif