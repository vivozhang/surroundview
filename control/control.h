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
#ifndef CONTROL_H
#define CONTROL_H

#include "cyber/cyber.h"
#include "modules/common/proto/chassis.pb.h"
#include "modules/common/proto/hmi_msg.pb.h"
#include "modules/common/proto/surroundview_control.pb.h"
#include <memory>
#include <string>
using apollo::common::Chassis;
using apollo::cyber::Reader;
using apollo::cyber::Writer;

enum VehicleStatus {
  VEHICLE_NONE = 0,
  VEHICLE_FORWARD = 1,
  VEHICLE_TURN_LEFT = 2,
  VEHICLE_TURN_RIGHT = 3,
  VEHICLE_BACKWARD = 4  //倒车
};

// turn direction type
enum TurnDirection {
  TURN_NONE = 0,
  TURN_LETF = 1,
  TURN_RIGHT = 2
};

enum ScreenControlType {
  OVERALL_VIEW_OPEN = 1,
  OVERALL_VIEW_CLOSED = 2
};

enum ScreenControlMode {
  SCREEN_NONE = -1,
  SCREEN_OVERALL = 0,   //全景图（全景图+四个田字格）
  SCREEN_LEFT = 1,    //左视图（点击左边按钮，左转向灯，左转方向盘）
  SCREEN_RIGHT = 2,   //右视图 （点击右边按钮，右转向灯，右转方向盘）
  SCREEN_BACK = 3,    //后视图  （点击后面按钮）
  SCREEN_FRONT = 4,   //前视图 （点击前面视图按钮）
  SCREEN_BACKWARD = 5  //倒车
};

struct ScreenControlAppTimeStamp {
  ScreenControlMode scm_ = SCREEN_NONE;  //状态值
  double ts_ = 0.;                      //时间戳
};

//进行播放的结构体处理
struct ErrorHandle {
  int error_num_ = -1;            //错误代码
  double error_peroid_ = 0.1;    //  内部时间间隔2秒
  double error_stop_peroid_ = 20;    //  进行轮训30秒
  int curr_time_ = 0;             //当前次数
  int times_ = 20;                //发送多少次
  double rec_ct_ = 0;             //接受的时间措
};

//空载或者满载状态
enum Status {
  VEHICLE_STATUS_EMPTY_ = 0,   //空载
  VEHICLE_STATUS_HEAVY_      //满载
};

class ScreenControl {
 public:
  ScreenControl();
  ~ScreenControl();
  bool Init();
  void SendSurroundWarningInfo(const int &err_num, const int &report_text_id);
  void GetScreenControlMode(int &scm, float &phsical_angle, int &heavy_status);
  void SendError(const int &err_num, const int &report_text_id);

 private:
  // init channels,readers and writers
  bool InitConfig();
  bool InitMsgChannels();
  // process the datas recved from channels
  void RecvChasisData(const Chassis &chasis);
  void ProcessChasisBySteerLight(const Chassis &chasis);
  void RecvHmiMsgInfo(const apollo::common::HmiMessage &msg);
  void RandParamters();

 private:
  std::shared_ptr<apollo::cyber::Node> node_ = nullptr;
  // chasis data reader and writer
  std::shared_ptr<Reader<Chassis>> chassis_reader_ = nullptr;
  std::shared_ptr<Reader<apollo::common::HmiMessage>> 
                                  hmi_message_reader_ = nullptr;
  std::shared_ptr<Writer<apollo::surroundview::Camera360Message>> 
                                  camera_360_message_writer_ = nullptr;
  // channels
  std::string camera_360_message_topic_;

  int speed_;   // km/h
  apollo::common::Chassis::GearPosition gear_position_;
  int turn_min_angle_thresholds_;
  int turn_signal_nums_ack_thresholds_;
  int turn_signal_nums_;
  int steer_angle_ratio_;
  int overall_view_speed_thresholds_;  // the threshold to show 360 vedio
  int turn_control_type_;             // 1-stear,2-light
  int qos_profile_depth_;
  int pending_queue_size_;

  VehicleStatus status_;
  VehicleStatus pre_status_;
  TurnDirection last_turn_light_direction_;     // last turn direction
  double last_turn_left_light_signal_seconds_;  // left light signal timestamp
  double last_turn_right_light_signal_seconds_;  //  right light signal timestamp
  double last_turn_light_signal_seconds_;       // light

  TurnDirection last_turn_steer_direction_;  // last turn direction
  int turn_check_seconds_;                  // check threshold
  ScreenControlType screen_control_;        // control type
  double last_screen_opr_timestamp_;
  ScreenControlMode screen_control_mode_;   // surround control mode
  ScreenControlMode last_screen_control_mode_
                 = SCREEN_NONE;  // the last surround control mode
  std::mutex mutex_;
  ScreenControlAppTimeStamp app_ts_;  // APP按钮的响应时间
  float phsical_angle_;              // 转换角
  double steer_angle_;               // 方向盘转角
  int heavy_status_;                 // 空载还是满载状态
  ErrorHandle error_handle_;
};

#endif
