#ifndef QUATERNION_VQF_HPP
#define QUATERNION_VQF_HPP

#include "VQF_filter.h"
#include <cstdint>

struct VQF_INS_t
{
    uint8_t Initialized;
    uint8_t StableFlag;
    uint64_t UpdateCount;
    uint64_t ErrorCount;
    float q[4];
    float GyroBias[3];
    float Gyro[3];
    float Accel[3];
    float Yaw;
    float Pitch;
    float Roll;
    float YawTotalAngle;
    float dt;
    VQF_FILTER_t filter;
};

bool IMU_QuaternionVQF_Init(VQF_INS_t *ins, const float *init_quaternion, float dt = 0.001f);
bool IMU_QuaternionVQF_Update(VQF_INS_t *ins, float gx, float gy, float gz,
                            float ax, float ay, float az, float dt);

#endif
