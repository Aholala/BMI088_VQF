#ifndef __ECF_CONFIGLIST_H
#define __ECF_CONFIGLIST_H

#ifndef USE_ALG
#define USE_ALG
#endif // USE_ALG

#ifndef USE_DWT
#define USE_DWT
#endif // USE_DWT

#ifndef USE_IMU
#define USE_IMU
#endif // USE_IMU

#ifndef USE_PWM
#define USE_PWM
#endif // USE_PWM

#ifndef USE_SPI
#define USE_SPI
#endif // USE_SPI

#ifndef Heater_Kp
#define Heater_Kp 7.f
#endif // Heater_Kp

#ifndef Heater_Ki
#define Heater_Ki 0.f
#endif // Heater_Ki

#ifndef Heater_Kd
#define Heater_Kd 0.f
#endif // Heater_Kd

#ifndef Heater_maxIerror
#define Heater_maxIerror 10
#endif // Heater_maxIerror

#ifndef Init_Heater_Duty
#define Init_Heater_Duty 10.f // 预热阶段固定占空比上限前的目标值
#endif // Init_Heater_Duty

#ifndef IMU_CALI_GNORMDIFF_BREAK_THRESHOLD
#define IMU_CALI_GNORMDIFF_BREAK_THRESHOLD 1.5f // 校准内层采样中断阈值
#endif // IMU_CALI_GNORMDIFF_BREAK_THRESHOLD

#ifndef IMU_CALI_GNORMDIFF_PASS_THRESHOLD
#define IMU_CALI_GNORMDIFF_PASS_THRESHOLD 1.501f // 校准外层通过阈值
#endif // IMU_CALI_GNORMDIFF_PASS_THRESHOLD

#ifndef TEMP_UPDATE_TIMEOUT_MS
#define TEMP_UPDATE_TIMEOUT_MS 500 // 温度数据更新超时保护，单位ms
#endif // TEMP_UPDATE_TIMEOUT_MS

#ifndef Heater_forward
#define Heater_forward 15 // 加热器前馈值
#endif // Heater_forward

#ifndef Init_temp
#define Init_temp 35.f // 初始化前温度
#endif // Init_temp

#ifndef Target_temp
#define Target_temp 40.f // 目标温度
#endif // Target_temp

#ifndef Max_temp
#define Max_temp 80.f // 最高温度
#endif // Max_temp

#ifndef Preheat_Timeout
#define Preheat_Timeout 20*1000 // 预热超时时间，单位ms
#endif // Preheat_Timeout

#ifndef IMU_HEATER_TIM
#define IMU_HEATER_TIM &htim3 // 温控PWM用TIM
#endif // IMU_HEATER_TIM

#ifndef IMU_HEATER_TIM_CHANNEL
#define IMU_HEATER_TIM_CHANNEL TIM_CHANNEL_1 // 温控pWM用TIM通道
#endif // IMU_HEATER_TIM_CHANNEL

#ifndef BMI088_SPI
#define BMI088_SPI &hspi4
#endif // BMI088_SPI

#ifndef isHeaterEnabled
#define isHeaterEnabled 0 // 是否启用加热器
#endif // isHeaterEnabled

#ifndef GxOFFSET
#define GxOFFSET -0.0f // 离线校准数据,需要手动修改，x轴角速度补偿
#endif // GxOFFSET

#ifndef GyOFFSET
#define GyOFFSET -0.0f // 离线校准数据,需要手动修改，y轴角速度补偿
#endif // GyOFFSET

#ifndef GzOFFSET
#define GzOFFSET 0.0f // 离线校准数据,需要手动修改，z轴角速度补偿
#endif // GzOFFSET

#ifndef gNORM
#define gNORM 9.6f // 离线校准数据,需要手动修改
#endif // gNORM

#ifndef CENTER_IMU_rx
#define CENTER_IMU_rx 0.f // 加速度计修正安装误差，imu距离 机体中心/旋转中心/质心 在 imu 坐标系 x轴方向上的 距离 单位m
#endif // CENTER_IMU_rx

#ifndef CENTER_IMU_ry
#define CENTER_IMU_ry 0.f // 加速度计修正安装误差，imu距离 机体中心/旋转中心/质心 在 imu 坐标系 y轴方向上的 距离 单位m
#endif // CENTER_IMU_ry

#ifndef CENTER_IMU_rz
#define CENTER_IMU_rz 0.f // 加速度计修正安装误差，imu距离 机体中心/旋转中心/质心 在 imu 坐标系 z轴方向上的 距离 单位m
#endif // CENTER_IMU_rz

#ifndef IMU_Pitch_Offset
#define IMU_Pitch_Offset 0.f // imu安装俯仰角修正 单位deg 
#endif // IMU_Pitch_Offset

#ifndef IMU_Roll_Offset
#define IMU_Roll_Offset 0.f // imu安装横滚角修正 单位deg
#endif // IMU_Roll_Offset

#ifndef Heater_maxout
#define Heater_maxout 100.0 // 加热最大占空比
#endif // Heater_maxout

#ifndef isCalibrate
#define isCalibrate 0.f // 启动时是否在线校准
#endif // isCalibrate
#endif // __ECF_CONFIGLIST_H
