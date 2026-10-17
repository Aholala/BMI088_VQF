cmake_minimum_required(VERSION 3.22)

# Set the project name
set(CMAKE_PROJECT_NAME Project)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(SIZE arm-none-eabi-size)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add color diagnostics
set(CMAKE_COLOR_DIAGNOSTICS ON)

include("cmake/gcc-arm-none-eabi.cmake") # avoid CubeMX delete it

# Core project settings
project(${CMAKE_PROJECT_NAME})

# Enable CMake support for ASM, C and CXX languages
enable_language(C CXX ASM)

# Remove warning
add_compile_options(-w)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Generate .map file automatically
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map"
)

# Add STM32CubeMX generated sources
add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/stm32cubemx)

# ==============================
# 包含dir以下后缀文件定义函数
# ==============================
function(USER_ADD_DIR dir)
    file(GLOB USER_SOURCE
            ${dir}/*.h
            ${dir}/*.c
            ${dir}/*.hpp
            ${dir}/*.cpp
            ${dir}/*.cc
    )
    target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${USER_SOURCE})
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${dir})
endfunction()

# ==============================
# 框架根目录（始终加入）
# ==============================
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/ACE_ECF_27)

# ==============================
# 默认库配置（在软件“修改CMake模块 / 默认库配置”中设置）
# ==============================
# default libraries start

# default libraries end

# ==============================
# 软件自动生成（每次生成时更新）
# 格式: USER_ADD_DIR(${CMAKE_SOURCE_DIR}/ACE_ECF_27/...)
# ==============================
# auto generate start
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/ACE_ECF_27/Bsp/DWT)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/ACE_ECF_27/Bsp/PWM)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/ACE_ECF_27/Bsp/SPI)
USER_ADD_DIR(${CMAKE_SOURCE_DIR}/ACE_ECF_27/Module/imu)
# auto generate end

# ==============================
# 模块附加 CMake 配置（启用模块时自动写入）
# ==============================
# module cmake start

# module cmake end

# ==============================
# 自己的文件加这里（TASK，软件不会修改）
# ==============================
# user custom start

# USER_ADD_DIR(${CMAKE_SOURCE_DIR}/Usercode/Task)

# user custom end

# ==============================
# 生成器模板固定配置
# ==============================
set(CMSIS_DSP_DIR "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        "${CMAKE_SOURCE_DIR}"
        "${CMSIS_DSP_DIR}/Include"
)

# CMSIS-Core 5.x 的部分 cmsis_gcc.h 在 C++/GCC 14 下会触发局部类型 extern 错误。
# AUTO 会检查头文件内容与启动文件；ON/OFF 供已人工确认的工程覆盖。
set(ACE_ECF_CMSIS_CXX_COMPAT "AUTO" CACHE STRING
        "CMSIS/GCC C++ compatibility mode: AUTO, ON or OFF")
set_property(CACHE ACE_ECF_CMSIS_CXX_COMPAT PROPERTY STRINGS AUTO ON OFF)
string(TOUPPER "${ACE_ECF_CMSIS_CXX_COMPAT}" _ACE_ECF_CMSIS_CXX_COMPAT_MODE)

if(NOT _ACE_ECF_CMSIS_CXX_COMPAT_MODE MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR
            "ACE_ECF_CMSIS_CXX_COMPAT must be AUTO, ON or OFF; got: "
            "${ACE_ECF_CMSIS_CXX_COMPAT}")
endif()

set(_ACE_ECF_ENABLE_CMSIS_CXX_COMPAT OFF)
if(_ACE_ECF_CMSIS_CXX_COMPAT_MODE STREQUAL "ON")
    set(_ACE_ECF_ENABLE_CMSIS_CXX_COMPAT ON)
elseif(_ACE_ECF_CMSIS_CXX_COMPAT_MODE STREQUAL "AUTO")
    set(_ACE_ECF_CMSIS_GCC_HEADER
            "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include/cmsis_gcc.h")

    if(EXISTS "${_ACE_ECF_CMSIS_GCC_HEADER}")
        file(READ "${_ACE_ECF_CMSIS_GCC_HEADER}" _ACE_ECF_CMSIS_GCC_CONTENT)
        if(_ACE_ECF_CMSIS_GCC_CONTENT MATCHES "#[ \t]*ifndef[ \t]+__PROGRAM_START"
                AND _ACE_ECF_CMSIS_GCC_CONTENT MATCHES
                    "void[ \t\r\n]+__cmsis_start[ \t]*\\(")
            file(GLOB _ACE_ECF_STARTUP_FILES LIST_DIRECTORIES false
                    "${CMAKE_SOURCE_DIR}/startup_*.s"
                    "${CMAKE_SOURCE_DIR}/startup_*.S"
                    "${CMAKE_SOURCE_DIR}/startup_*.asm"
                    "${CMAKE_SOURCE_DIR}/startup_*.c"
                    "${CMAKE_SOURCE_DIR}/startup_*.cc"
                    "${CMAKE_SOURCE_DIR}/startup_*.cpp"
                    "${CMAKE_SOURCE_DIR}/startup_*.cxx"
                    "${CMAKE_SOURCE_DIR}/Core/Startup/startup_*"
                    "${CMAKE_SOURCE_DIR}/Core/Src/startup_*"
                    "${CMAKE_SOURCE_DIR}/Startup/startup_*")
            list(LENGTH _ACE_ECF_STARTUP_FILES _ACE_ECF_STARTUP_COUNT)
            if(NOT _ACE_ECF_STARTUP_COUNT EQUAL 1)
                message(FATAL_ERROR
                        "Old CMSIS __cmsis_start detected, but AUTO expected exactly one "
                        "startup_* source and found ${_ACE_ECF_STARTUP_COUNT}. Verify the "
                        "startup path, then set ACE_ECF_CMSIS_CXX_COMPAT=ON only when safe, "
                        "or use OFF after upgrading/fixing CMSIS.")
            endif()

            list(GET _ACE_ECF_STARTUP_FILES 0 _ACE_ECF_STARTUP_FILE)
            get_filename_component(_ACE_ECF_STARTUP_EXT
                    "${_ACE_ECF_STARTUP_FILE}" LAST_EXT)
            string(TOLOWER "${_ACE_ECF_STARTUP_EXT}" _ACE_ECF_STARTUP_EXT)

            if(_ACE_ECF_STARTUP_EXT STREQUAL ".c")
                # 宏只添加到 C++ 编译单元，C 启动单元的 __PROGRAM_START 保持原状。
                set(_ACE_ECF_ENABLE_CMSIS_CXX_COMPAT ON)
            elseif(_ACE_ECF_STARTUP_EXT STREQUAL ".s"
                    OR _ACE_ECF_STARTUP_EXT STREQUAL ".asm")
                file(READ "${_ACE_ECF_STARTUP_FILE}" _ACE_ECF_STARTUP_CONTENT)
                string(TOLOWER "${_ACE_ECF_STARTUP_CONTENT}" _ACE_ECF_STARTUP_CONTENT_LOWER)
                string(FIND "${_ACE_ECF_STARTUP_CONTENT_LOWER}" "_sdata" _ACE_ECF_SDATA_POS)
                string(FIND "${_ACE_ECF_STARTUP_CONTENT_LOWER}" "_sbss" _ACE_ECF_SBSS_POS)
                string(FIND "${_ACE_ECF_STARTUP_CONTENT_LOWER}"
                        "__libc_init_array" _ACE_ECF_INIT_ARRAY_POS)
                string(REGEX MATCH
                        "(^|[\r\n])[ \t]*(bl|b)[ \t]+main([ \t\r\n]|$)"
                        _ACE_ECF_MAIN_CALL "${_ACE_ECF_STARTUP_CONTENT_LOWER}")
                if(NOT _ACE_ECF_SDATA_POS EQUAL -1
                        AND NOT _ACE_ECF_SBSS_POS EQUAL -1
                        AND NOT _ACE_ECF_INIT_ARRAY_POS EQUAL -1
                        AND _ACE_ECF_MAIN_CALL)
                    set(_ACE_ECF_ENABLE_CMSIS_CXX_COMPAT ON)
                else()
                    message(FATAL_ERROR
                            "Old CMSIS __cmsis_start detected, but ${_ACE_ECF_STARTUP_FILE} "
                            "does not prove data/bss initialization, C++ constructors and a "
                            "direct main call. Set ACE_ECF_CMSIS_CXX_COMPAT=ON only after "
                            "manual verification, or use OFF after upgrading/fixing CMSIS.")
                endif()
            else()
                message(FATAL_ERROR
                        "Old CMSIS __cmsis_start detected with unsupported C++ startup file: "
                        "${_ACE_ECF_STARTUP_FILE}. Set ACE_ECF_CMSIS_CXX_COMPAT=ON only after "
                        "manual verification, or use OFF after upgrading/fixing CMSIS.")
            endif()
        endif()
    else()
        message(STATUS
                "ACE ECF CMSIS C++ compatibility AUTO: cmsis_gcc.h not found; no workaround")
    endif()
endif()

if(_ACE_ECF_ENABLE_CMSIS_CXX_COMPAT)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:__PROGRAM_START=main>)
    message(STATUS
            "ACE ECF CMSIS C++ compatibility: enabled (${_ACE_ECF_CMSIS_CXX_COMPAT_MODE})")
else()
    message(STATUS
            "ACE ECF CMSIS C++ compatibility: disabled (${_ACE_ECF_CMSIS_CXX_COMPAT_MODE})")
endif()

# Reuse the framework logo during configuration.
include("${CMAKE_SOURCE_DIR}/ACE_ECF_27/cmake/ecf_logo.cmake")

# H7 工程需要链接 CMSIS-DSP 预编译库；F4 工程跳过此库
if(EXISTS "${CMAKE_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c")
    set(DSP_LIB_PATH "${CMSIS_DSP_DIR}/Lib/GCC/libarm_cortexM7lfdp_math.a")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c")
    set(DSP_LIB_PATH "") # F4 不检查或链接 CMSIS-DSP 预编译库
else()
    message(FATAL_ERROR "Unsupported STM32 family: expected STM32H7xx or STM32F4xx HAL")
endif()

if(DSP_LIB_PATH AND NOT EXISTS "${DSP_LIB_PATH}")
    message(FATAL_ERROR "CMSIS-DSP library not found: ${DSP_LIB_PATH}")
endif()

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
        stm32cubemx
        ${DSP_LIB_PATH}
        # Add user defined libraries
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex
        # COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin
        COMMENT "Building hex & bin file...\nEXCUTABLE SIZE:"
        COMMAND ${SIZE} $<TARGET_FILE:${PROJECT_NAME}>
        VERBATIM
)

# Framework-owned build output; attach after firmware conversion commands.
set(ECF_ROOT "${CMAKE_SOURCE_DIR}/ACE_ECF_27")
include("${ECF_ROOT}/cmake/ecf_build.cmake")
ecf_attach_build_output(${CMAKE_PROJECT_NAME}
        BEFORE_TARGETS STM32_Drivers FreeRTOS
)
