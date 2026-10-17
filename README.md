# BMI088_VQF

STM32H723 / BMI088 六轴姿态解算，使用导入的 VQF 算法，保留 ECF 原有 IMU 调用接口。

| 层 | 文件 | 职责 |
|---|---|---|
| Algorithm | `App/Algorithm/vqf/VQF_filter.c/.h` | 导入的 VQF 核心，陀螺积分、倾角修正、运动/静止零偏估计 |
| Algorithm | `App/Algorithm/vqf/quaternion_vqf.cpp/.hpp` | 对齐原 EKF 的 Init/Update、输出字段和欧拉角命名 |
| Module | `App/Module/imu/BMI088driver.cpp/.hpp` | 按库驱动适配采集、校准、温控，调用所选姿态算法 |
| Module | `App/Module/imu/imu_task.cpp/.h` | 保持原 IMU 任务和 `getImuPtr()` 入口 |
| Bsp | `ACE_ECF_27/Bsp/DWT`、`SPI`、`PWM` | 保留现有外设能力 |

本项目的实现全部放在 `App` 下，按 Algorithm / Module 分层；不修改 `ACE_ECF_27`。寄存器头直接引用库的 `ACE_ECF_27/Module/imu/BMI088reg.hpp`，不新增寄存器定义。App 驱动按库的原 IMU 实现适配，CMake 排除库中原来的 `BMI088driver.cpp` 和 `imu_task.cpp`，保证只编译一套驱动/任务；App 头文件优先搜索。算法层不依赖 HAL、FreeRTOS 或设备模块。

App 的 IMU 模块固定调用 VQF，没有 EKF 分支，也不使用 EKF 函数名做兼容替换。库中的 BMI088 驱动与任务不参加本项目编译。

项目根目录 `ecf_configList.h` 仍提供 SPI、PWM、温控和校准参数。配置工具及其规则不修改；本项目源码路径与排除项写在 CMake 的 user custom 段。

CubeMX 已创建的 `IMU_Task` 继续运行，任务仍以 1 ms 为调度周期，滤波使用 DWT 测得的实际秒数。首帧建立时间基准，非有限或超出 0.1–20 ms 的间隔跳过；暂停调试器后应检查时间状态，DWT 的 32 位周期计数不能独自识别跨越多次回绕的长时间暂停。

调用方式保持一致：

```cpp
#include "imu_task.h"

BMI088Heat_c *imu = getImuPtr();
if (imu != nullptr && imu->isImuInited() && imu->INS.init)
{
    const INS_t *ins = imu->Get_INS_Data_Point();
    float yaw = ins->Yaw;
    float pitch = ins->Pitch;
    float roll = ins->Roll;
    float yaw_total = ins->YawTotalAngle;
}
```

四元数为 `[w,x,y,z]`，表示机体系到导航系的旋转。输入角速度为 rad/s，加速度为 m/s²，输出欧拉角为 deg。为兼容现有 EKF，`Pitch` 表示 X 轴转角，`Roll` 表示 Y 轴转角；对应导入 VQF 的 `roll`、`pitch`。`Pitch_calibrated`、`Roll_calibrated`、重力剔除、运动加速度与质心补偿继续由原驱动处理。

Ozone 可观察：

- `imu->BMI088`：采集、温度、离线/启动标定参数。
- `imu->INS`：原有应用输出，`init` 表示已经有成功解算帧。
- `imu->VQF_INS`：VQF 的 `Initialized`、`UpdateCount`、`ErrorCount`、`dt`、`GyroBias` 和输出姿态。
- `imu->VQF_INS.filter`：`restDetected`、`bias`、滤波参数和内部状态。
- `imu->attitude_rejected_count`：间隔异常或算法拒绝的帧数。

VQF 的运动/静止零偏估计参数沿用导入代码的默认值。重新初始化会重置滤波器和累计角；更新采样周期保留已有滤波历史。当前 `isHeaterEnabled=0`、`isCalibrate=0` 沿用项目原配置，离线 `GyroOffset`/`gNORM` 需要按板子测量设置。没有磁力计或其他航向观测，累计 yaw 仍可能漂移。

构建与算法验证：

```powershell
cmake --preset Debug
cmake --build --preset Debug --parallel 8
.\App\tests\run_vqf_host_tests.ps1
```

主机测试通过解析姿态和人工六轴轨迹检查静止、倒置、倾斜、动态 X/Y 旋转、正反 yaw 多圈、零偏收敛、变周期、重置和异常输入。主机测试不验证 SPI、实机温控或板上姿态精度。测试输出放在已忽略的 `.cache/vqf-host-tests`，`build` 下只保留 `Debug`。固件产物位于 `build/Debug/Project.elf`、`.hex`、`.map`。

周期变化时滤波状态衔接公式核对自 [VQF 上游源码](https://github.com/dlaidig/vqf/blob/main/vqf/cpp/vqf.cpp)。主体算法仍使用导入的实现。

源码与测试均由主仓库管理；ECF 子模块保持原提交和干净状态。
