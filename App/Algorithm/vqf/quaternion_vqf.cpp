#include "quaternion_vqf.hpp"
#include <cmath>
#include <cstring>

namespace
{
void Publish(VQF_INS_t *ins)
{
    vqf_output(&ins->filter);
    memcpy(ins->q, ins->filter.q, sizeof(ins->q));
    memcpy(ins->GyroBias, ins->filter.bias, sizeof(ins->GyroBias));
    ins->Yaw = ins->filter.yaw;
    ins->Pitch = ins->filter.roll;
    ins->Roll = ins->filter.pitch;
    ins->YawTotalAngle = ins->filter.YawTotalAngle;
    ins->StableFlag = ins->filter.restDetected;
}

bool ValidPeriod(float dt)
{
    return std::isfinite(dt) && dt >= 0.0001f && dt <= 0.02f;
}
}

bool IMU_QuaternionVQF_Init(VQF_INS_t *ins, const float *init_quaternion, float dt)
{
    if (ins == nullptr || !ValidPeriod(dt)) return false;
    float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    if (init_quaternion != nullptr)
    {
        float norm2 = 0.0f;
        for (int i = 0; i < 4; ++i)
        {
            if (!std::isfinite(init_quaternion[i])) return false;
            norm2 += init_quaternion[i]*init_quaternion[i];
        }
        if (!std::isfinite(norm2) || norm2 < 1e-12f) return false;
        const float inv_norm = 1.0f/std::sqrt(norm2);
        for (int i = 0; i < 4; ++i) q[i] = init_quaternion[i]*inv_norm;
    }
    memset(ins, 0, sizeof(*ins));
    vqf_init(&ins->filter, dt);
    memcpy(ins->filter.accQuat, q, sizeof(q));
    Publish(ins);
    // Initial yaw is an orientation, not a crossing of +/-180 degrees.
    ins->filter.yaw_laps = 0.0f;
    ins->filter.YawTotalAngle = ins->filter.yaw;
    ins->YawTotalAngle = ins->Yaw;
    ins->dt = dt;
    ins->Initialized = 1;
    return true;
}

bool IMU_QuaternionVQF_Update(VQF_INS_t *ins, float gx, float gy, float gz,
                            float ax, float ay, float az, float dt)
{
    if (ins == nullptr) return false;
    if (!ins->Initialized || !ValidPeriod(dt) || !std::isfinite(gx)
        || !std::isfinite(gy) || !std::isfinite(gz) || !std::isfinite(ax)
        || !std::isfinite(ay) || !std::isfinite(az)
        || std::fabs(gx) > 40.0f || std::fabs(gy) > 40.0f || std::fabs(gz) > 40.0f
        || std::fabs(ax) > 300.0f || std::fabs(ay) > 300.0f || std::fabs(az) > 300.0f)
    {
        ++ins->ErrorCount;
        return false;
    }
    if (!vqf_set_sample_period(&ins->filter, dt))
    {
        ++ins->ErrorCount;
        return false;
    }
    vqf_update(&ins->filter, gx, gy, gz, ax, ay, az);
    Publish(ins);
    ins->Gyro[0] = gx; ins->Gyro[1] = gy; ins->Gyro[2] = gz;
    ins->Accel[0] = ax; ins->Accel[1] = ay; ins->Accel[2] = az;
    ins->dt = dt;
    ++ins->UpdateCount;
    return true;
}
