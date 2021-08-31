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
#include "control.h"

#include "gflags/gflags.h"
#include "modules/common/adapters/adapter_gflags.h"
#include "modules/common/configs/config_gflags.h"
#include "modules/common/time/time.h"
// #include "modules/drivers/gnssins/can_data_ntoh.h"
// params config file
static std::string CONFIG_FILE_PATH = "../conf/surroundview_control.pb.txt";
const int kMax_INT_RESET_TIMES = 0x1FFFFFFF;
ScreenControl::ScreenControl() {
  node_ = apollo::cyber::CreateNode("ScreenControl");
  pre_status_ = VEHICLE_NONE;
  status_ = VEHICLE_FORWARD;
  gear_position_ = apollo::common::Chassis::GEAR_NEUTRAL;
  speed_ = 0;
  turn_min_angle_thresholds_ = 15;           // 转向角阈值：360/23
  turn_signal_nums_ack_thresholds_ = 20;     // 接受转向角信号来的次数
  turn_signal_nums_ = 0;                     // 累加次数
  steer_angle_ratio_ = 23;                   // 倍数
  phsical_angle_ = 0.;                       // 转化的角度
  steer_angle_ = 0.;                         // 方向盘转角
  last_turn_light_direction_ = TURN_NONE;    // 灯的信号
  last_turn_left_light_signal_seconds_ = 0;  // 最后左灯的时间
  last_turn_right_light_signal_seconds_ = 0;  // 最后右灯的时间
  last_turn_light_signal_seconds_ = 0;
  turn_check_seconds_ = 2;  // 转向灯持续时间

  last_turn_steer_direction_ = TURN_NONE;  // 方向盘的信号
  overall_view_speed_thresholds_ = 15;    // 全景视图和前视图切换阈值：hm/h

  screen_control_ = OVERALL_VIEW_CLOSED;
  screen_control_mode_ = SCREEN_NONE;
  last_screen_control_mode_ = SCREEN_NONE;
  last_screen_opr_timestamp_ = 0;
  turn_control_type_ = 1;
  qos_profile_depth_ = 10;  // apollo参数
  pending_queue_size_ = 50; 

  heavy_status_ = VEHICLE_STATUS_EMPTY_;  // 载重状态
}

ScreenControl::~ScreenControl() {}

bool ScreenControl::Init() {
  if (!InitConfig()) {
    return false;
  }
  InitMsgChannels();
  return true;
}

bool ScreenControl::InitConfig() {
  // read configurations
  apollo::surroundview::Config my_config;
  if (!apollo::cyber::common::GetProtoFromFile(CONFIG_FILE_PATH, &my_config)) {
    std::cout << "Read conf failed! : " << CONFIG_FILE_PATH << std::endl;
    return false;
  }
  std::cout << "Read conf succedly: " << CONFIG_FILE_PATH << std::endl;

  camera_360_message_topic_ = my_config.camera_360_message_topic();
  turn_min_angle_thresholds_ = my_config.turn_min_angle_thresholds();
  turn_signal_nums_ack_thresholds_ = my_config.turn_signal_nums_ack_thresholds();
  steer_angle_ratio_ = my_config.steer_angle_ratio();
  overall_view_speed_thresholds_ = my_config.overall_view_speed_thresholds();
  turn_control_type_ = my_config.turn_control_type();
  qos_profile_depth_ = my_config.qos_profile_depth();
  pending_queue_size_ = my_config.pending_queue_size();
  std::cout << "config paramter: 360:" << camera_360_message_topic_
            << ",turn_control_type:" << turn_control_type_
            << ",qos_profile_depth:" << qos_profile_depth_
            << ",pending_queue_size:" << pending_queue_size_ << std::endl;
  return true;
}

bool ScreenControl::InitMsgChannels() {
  if (nullptr == node_) {
    std::cout << "InitMsgChannels: node_ is null." << std::endl;
    return false;
  }
  std::cout << "InitMsgChannels: successfully." << std::endl;

  //读取底盘数据
  apollo::cyber::ReaderConfig reader_config;
  reader_config.channel_name = FLAGS_chassis_topic;
  reader_config.qos_profile.set_depth(qos_profile_depth_);
  reader_config.pending_queue_size = pending_queue_size_;
  if (nullptr == chassis_reader_) {
    chassis_reader_ = node_->CreateReader<apollo::common::Chassis>(
        reader_config, [this](const std::shared_ptr<apollo::common::Chassis> &message) {
          RecvChasisData(*(message.get()));
        });
  }

  // hmi模块给中间层模块数据
  reader_config.channel_name = FLAGS_hmi_message_topic;
  if (nullptr == hmi_message_reader_) {
    hmi_message_reader_ = node_->CreateReader<apollo::common::HmiMessage>(
        reader_config,
        [this](const std::shared_ptr<apollo::common::HmiMessage> &hmi_message) {
          RecvHmiMsgInfo(*(hmi_message.get()));
        });
  }

  if (hmi_message_reader_)
    std::cout << "message reader succc:" << std::endl;

  //中间层给hmi模块写数据
  if (nullptr == camera_360_message_writer_) {
    camera_360_message_writer_ =
        node_->CreateWriter<apollo::surroundview::Camera360Message>(
            camera_360_message_topic_);
    if (nullptr == camera_360_message_writer_) {
      return false;
    }
  }
  return true;
}

void ScreenControl::RecvChasisData(const apollo::common::Chassis &chasis) {
  // 方向盘和转向灯进行判断
  // 408车上方向盘存在问题，明天测试发出数据
  // ProcessChasisBySteer(chasis);
  // ProcessChasisByLight(chasis);
  {
    std::lock_guard<std::mutex> lock(mutex_);
    ProcessChasisBySteerLight(chasis);
  }
}

void ScreenControl::ProcessChasisBySteerLight(const apollo::common::Chassis &chasis){
  VehicleStatus pre_status = status_;
  apollo::common::Chassis::GearPosition last_gear_pos = gear_position_;
  // reverse gear procee particularly
  gear_position_ = chasis.gear_location();
  speed_ = (chasis.speed_mps() * 3600) / 1000; //车速
  steer_angle_ = chasis.steer_angle();
  phsical_angle_ = steer_angle_ / steer_angle_ratio_; // 360/23=15
  // 倒档（优先级最高）
  if (gear_position_ == apollo::common::Chassis::GEAR_REVERSE) {
    status_ = VEHICLE_BACKWARD;
    return;
  }
  apollo::common::LightState light_status = chasis.light_state(); //转向灯信息
  bool left_light_signal = light_status.turn_left_light_on();     //左转向灯信息
  bool right_light_signal = light_status.turn_right_light_on();   //右转向灯信息
  std::cout << "LightState:" << left_light_signal << "," << right_light_signal
            << ",VehicleStatus:" << status_ << std::endl;
  std::cout << "speed:" << speed_ << ",steer_angle:" << steer_angle_
            << ",phsical_angle:" << phsical_angle_
            << ",angle_thresholds:" << turn_min_angle_thresholds_
            << ",status_:" << status_ << std::endl;

  double now_second = apollo::common::time::Clock::NowInSeconds();
  // check the message type by light signals
  //解决转向灯信息丢失问题
  if (left_light_signal){
    last_turn_left_light_signal_seconds_ = now_second;
    last_turn_light_signal_seconds_ = now_second;
    status_ = VEHICLE_TURN_LEFT;
    last_turn_light_direction_ = TURN_LETF;
    return;
  }
  else if (right_light_signal){
    last_turn_right_light_signal_seconds_ = now_second;
    last_turn_light_signal_seconds_ = now_second;
    status_ = VEHICLE_TURN_RIGHT;
    last_turn_light_direction_ = TURN_RIGHT;
    return;
  }
  else {
    if ((now_second - last_turn_light_signal_seconds_) < turn_check_seconds_) {
      if (last_turn_light_direction_ != TURN_NONE)
        return;
    }
    else {
      if (last_turn_light_direction_ != TURN_NONE) {
        last_turn_light_direction_ = TURN_NONE;
        status_ = VEHICLE_FORWARD;
      }
    }
  }
  //方向盘
  if (abs(phsical_angle_) >= turn_min_angle_thresholds_) {
    TurnDirection direction = phsical_angle_ < 0 ? TURN_LETF : TURN_RIGHT; //左转负数，右转正数
    status_ = phsical_angle_ < 0 ? VEHICLE_TURN_LEFT : VEHICLE_TURN_RIGHT;
  }
  else {
    status_ = VEHICLE_FORWARD;
  }
}

// 360给中控屏信息
void ScreenControl::SendSurroundWarningInfo(const int &err_num, const int &report_text_id) {
  bool has_error = true; //有错误
  int32_t sound = 0;     //无播音
  apollo::surroundview::ErrType err_type = (apollo::surroundview::ErrType)err_num;
  // std::cout << "SendSurroundWarningInfo ErrType: " << err_type << std::endl;
  apollo::surroundview::Camera360Message camera360Message;
  camera360Message.mutable_surround_warn()->set_has_error(has_error);
  camera360Message.mutable_surround_warn()->set_err_type(err_type);
  camera360Message.set_report_text(report_text_id);
  camera_360_message_writer_->Write(camera360Message);
}

// optional int32 control_type = 1;     //360激活状态， 0 未激活  1 激活
// optional int32 control_mode = 2;   //360环视模式， 0 全息模式 1
// 左侧，2右侧，3 后方
void ScreenControl::RecvHmiMsgInfo(const apollo::common::HmiMessage &msg) {
  static long i = 0;
  if (msg.has_surround_enable()) {
    i += 1;
    std::cout << std::to_string(i) << ",HmiMsg surround enable:" << msg.surround_enable()
              << "," << msg.surround_mode() << std::endl;
    int control_mode = msg.surround_mode();
    if (msg.surround_enable()) {
      screen_control_ = OVERALL_VIEW_OPEN;
      app_ts_.scm_ = (ScreenControlMode)msg.surround_mode();     //状态
      app_ts_.ts_ = apollo::common::time::Clock::NowInSeconds(); //时间
      apollo::surroundview::Camera360Message camera360Message;
      camera360Message.set_control_type(1); // 360激活状态， 0 未激活  1 激活
      camera360Message.set_control_mode(control_mode);
      camera_360_message_writer_->Write(camera360Message);
    }
    else {
      screen_control_ = OVERALL_VIEW_CLOSED;
      app_ts_.scm_ = SCREEN_NONE;                                //无状态值
      app_ts_.ts_ = apollo::common::time::Clock::NowInSeconds(); //时间
      apollo::surroundview::Camera360Message camera360Message;
      camera360Message.set_control_type(0); // 360激活状态， 0 未激活  1 激活
      camera360Message.set_control_mode(SCREEN_OVERALL);
      camera_360_message_writer_->Write(camera360Message);
    }
  }

  //载重数据: min=0, max=8
  if (msg.has_cur_carry()){
    int control_mode = msg.cur_carry();
    if (control_mode < 3)
      heavy_status_ = VEHICLE_STATUS_EMPTY_;
    else
      heavy_status_ = VEHICLE_STATUS_HEAVY_;
  }
  // else  {    //保持上一次操作

  // }
}

void ScreenControl::RandParamters() {
  // screen_control_ = ScreenControlType(rand() % 2 + 1);
  //   enum ScreenControlType {
  //   OVERALL_NONE = 0,
  //   OVERALL_VIEW_OPEN = 1,
  //   OVERALL_VIEW_CLOSED = 2
  // };
  screen_control_ = OVERALL_VIEW_OPEN;
  // status_ = VehicleStatus(rand() % 4 + 1);
  //   enum VehicleStatus {
  //   VEHICLE_NONE = 0,
  //   VEHICLE_FORWARD = 1,
  //   VEHICLE_TURN_LEFT = 2,
  //   VEHICLE_TURN_RIGHT = 3,
  //   VEHICLE_BACKWARD = 4
  // };
  status_ = VEHICLE_FORWARD;
  //   enum ScreenControlMode {
  //   SCREEN_NONE = -1,
  //   SCREEN_OVERALL= 0,
  //   SCREEN_LEFT = 1,
  //   SCREEN_RIGHT = 2,
  //   SCREEN_BACK = 3,
  //   SCREEN_FRONT = 4
  // };
  screen_control_mode_ = ScreenControlMode(rand() % 5);
  speed_ = rand() % 30 + 20;
}

void ScreenControl::GetScreenControlMode(int &scm, float &phsical_angle, int &heavy_status) {
  static int num = 0;
  screen_control_mode_ = SCREEN_NONE; //修改 7.16
  phsical_angle = phsical_angle_;
  heavy_status = heavy_status_;
  double currentTimeStamp = apollo::common::time::Clock::NowInSeconds(); //当前时间
  //当AVM开启时，同时遇到车自身状态和按钮响应时，以车自身状态为优，并把当前按钮状态置无
  if (status_ == VEHICLE_BACKWARD) {
    screen_control_mode_ = SCREEN_BACKWARD;
    app_ts_.scm_ = SCREEN_NONE;
  }
  else if (status_ == VEHICLE_TURN_LEFT) {
    screen_control_mode_ = SCREEN_LEFT;
    app_ts_.scm_ = SCREEN_NONE;
  }
  else if (status_ == VEHICLE_TURN_RIGHT) {
    screen_control_mode_ = SCREEN_RIGHT;
    app_ts_.scm_ = SCREEN_NONE;
  }
  else if (status_ == VEHICLE_FORWARD) {
    //当速度为0时，全景显示
    if (speed_ == 0)
      screen_control_mode_ = SCREEN_OVERALL;

    //当车速大于0时，前视图显示
    if (speed_ > 0)
      screen_control_mode_ = SCREEN_FRONT;

    //来自中控屏按钮信息  ，如果车身状态没有发生改变，就维持
    if (app_ts_.scm_ != SCREEN_NONE)
      screen_control_mode_ = app_ts_.scm_;
  }

  // 当360处于关闭状态时，遇到车身情况时（倒档、转向灯、方向盘和速度介于0～15km/h），需要进行激活
  if (screen_control_ == OVERALL_VIEW_CLOSED) {
    //车身信息
    bool flag_a = ((screen_control_mode_ == SCREEN_BACKWARD ||
                    screen_control_mode_ == SCREEN_LEFT ||
                    screen_control_mode_ == SCREEN_RIGHT) &&
                   screen_control_mode_ != last_screen_control_mode_);
    //车点火启动后，速度超过
    bool flag_b = (screen_control_mode_ == SCREEN_FRONT && screen_control_mode_ != last_screen_control_mode_);
    if (flag_a || flag_b) {
      std::cout << std::to_string(num) << ",activate:last_screen_control_mode_:" << last_screen_control_mode_
                << ",screen_control_:" << screen_control_
                << ",status_:" << status_
                << ",screen_control_mode_:" << screen_control_mode_
                << ",speed_:" << speed_
                << ",overall_view_speed_thresholds_:" << overall_view_speed_thresholds_
                << std::endl;
      apollo::surroundview::Camera360Message camera360Message;
      camera360Message.set_control_type(1); //激活状态
      if (screen_control_mode_ == SCREEN_BACKWARD)
        camera360Message.set_control_mode(SCREEN_BACK);
      else
        camera360Message.set_control_mode(screen_control_mode_);
      camera_360_message_writer_->Write(camera360Message);
      screen_control_ = OVERALL_VIEW_OPEN;
    }
  }

  //上报前视状态
  if (screen_control_ == OVERALL_VIEW_OPEN) {
    bool flag_c = (screen_control_mode_ == SCREEN_FRONT ||
                   screen_control_mode_ == SCREEN_LEFT ||
                   screen_control_mode_ == SCREEN_BACKWARD ||
                   screen_control_mode_ == SCREEN_RIGHT ||
                   screen_control_mode_ == SCREEN_BACK ||
                   screen_control_mode_ == SCREEN_OVERALL);
    bool flag_d = (screen_control_mode_ != last_screen_control_mode_);
    if (flag_c && flag_d) {
      apollo::surroundview::Camera360Message camera360Message;
      camera360Message.set_control_type(1); //激活状态
      if (screen_control_mode_ == SCREEN_BACKWARD)
        camera360Message.set_control_mode(SCREEN_BACK);
      else
        camera360Message.set_control_mode(screen_control_mode_);
      camera_360_message_writer_->Write(camera360Message);
    }
  }

  //360处于激活状态，如果车速超过15km/h并且中控屏没有操作，就进行退出360环视
  // if(screen_control_==OVERALL_VIEW_OPEN&&screen_control_mode_ == SCREEN_FRONT
  //     &&app_ts_.scm_==SCREEN_NONE)
  // {
  //       std::cout << std::to_string(i)<<",deactivate:last_screen_control_mode_,screen_control_,status_,screen_control_mode_,speed_,overall_view_speed_thresholds_:"
  //      << last_screen_control_mode_<<","<<screen_control_ << "," << status_ << "," << screen_control_mode_
  //      << "," << speed_<<","<<overall_view_speed_thresholds_<<std::endl;
  //   apollo::surroundview::Camera360Message camera360Message;
  //   camera360Message.set_control_type(0);    //不激活状态
  //   camera360Message.set_control_mode(screen_control_mode_);
  //   camera_360_message_writer_->Write(camera360Message);
  //   screen_control_==OVERALL_VIEW_CLOSED;
  // }
  // num+=1;
  last_screen_control_mode_ = screen_control_mode_;
  if (screen_control_ == OVERALL_VIEW_CLOSED)
    return;
  scm = screen_control_mode_;
}

void ScreenControl::SendError(const int &err_num, const int &report_text_id) {
  //关闭时不进行播报
  if (screen_control_ == OVERALL_VIEW_CLOSED)
    return;

  //不是之前结果重新置0
  double currentTimeStamp = apollo::common::time::Clock::NowInSeconds();
  if (error_handle_.error_num_ != err_num){
    error_handle_.error_num_ = err_num;
    error_handle_.curr_time_ = 0;
    error_handle_.rec_ct_ = currentTimeStamp;
  }

  //作差进行判断，播放语音，连续播放5次
  double diff = currentTimeStamp - error_handle_.rec_ct_;
  if (diff > error_handle_.error_peroid_ && 
      error_handle_.curr_time_ <= error_handle_.times_){
    SendSurroundWarningInfo(err_num, report_text_id);
    error_handle_.curr_time_ += 1;
    error_handle_.rec_ct_ = currentTimeStamp;
  }

  //停止播放，经过一定的时间间隔，从新开始上报错误标识
  if (diff > error_handle_.error_stop_peroid_){
    error_handle_.curr_time_ = 0;
    error_handle_.rec_ct_ = currentTimeStamp;
  }
}
