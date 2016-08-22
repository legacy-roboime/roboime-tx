cmake_minimum_required(VERSION 3.0)

project (roboime-firmware_v1.0)

IF (NOT CMAKE_CROSSCOMPILING)
  message (FATAL_ERROR "Cross compiling only. Please use -DCMAKE_TOOLCHAIN_FILE=/PATH/TO/TOOLCHAIN_FILE .")
ENDIF (NOT CMAKE_CROSSCOMPILING)

add_definitions (-DSTM32F4XX -DSTM32F40XX -DUSE_STDPERIPH_DRIVER -DUSE_AUTOACK -DUSE_STM32F4_DISCOVERY)
set (STM32F4_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -mcpu=cortex-m4 -g -Wall")
#                   -Wunused-variable -Wunused-but-set-variable -Wimplicit-function-declaration")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STM32F4_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++98 ${STM32F4_FLAGS}")
#set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/stm32f4-discovery.ld -nostartfiles --specs=rdimon.specs")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/stm32f4_flash.ld -nostartfiles --specs=rdimon.specs")

function (add_bin_from_elf bin elf)
  add_custom_target (${bin} ALL ${CMAKE_OBJCOPY} -Obinary ${elf} ${bin} DEPENDS ${elf})
endfunction (add_bin_from_elf)

#
# LIBS
#
include_directories (src)
include_directories (lib/Device/STM32F4xx/Include/)
include_directories (lib/CMSIS/Include/)

# utils
set (utils_LIB lib/utils/stm32f4xx)
include_directories (${utils_LIB})
include_directories (lib/utils/stm32f4_discovery)
#link_directories (${utils_LIB})
#link_directories (lib/utils/stm32f4_discovery)
add_library (utils_lib STATIC
  ${utils_LIB}/startup_stm32f40xx.s
  ${utils_LIB}/stm32f4xx_it.c
  ${utils_LIB}/system_stm32f4xx.c
  ${utils_LIB}/tiny_printf.c
  lib/utils/stm32f4_discovery/stm32f4_discovery.c
)

# STM32F4xx_StdPeriph_Driver
set (STM32F4xx_StdPeriph_Driver_LIBS lib/STM32F4xx_StdPeriph_Driver/src)
include_directories (lib/STM32F4xx_StdPeriph_Driver/inc)
#link_directories    (${STM32F4xx_StdPeriph_Driver_LIBS})
add_library (
  STM32F4xx_StdPeriph_Driver_libs STATIC
  ${STM32F4xx_StdPeriph_Driver_LIBS}/misc.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_adc.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_can.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_crc.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_cryp_aes.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_cryp.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_cryp_des.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_cryp_tdes.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_dac.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_dbgmcu.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_dcmi.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_dma.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_exti.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_flash.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_fsmc.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_gpio.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_hash.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_hash_md5.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_hash_sha1.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_i2c.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_iwdg.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_pwr.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_rcc.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_rng.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_rtc.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_sdio.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_spi.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_syscfg.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_tim.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_usart.c
  ${STM32F4xx_StdPeriph_Driver_LIBS}/stm32f4xx_wwdg.c
)



# usb_conf
set (usb_conf_LIBS lib/usb_conf)
include_directories (${usb_conf_LIBS})
#link_directories    (${usb_conf_LIBS})
add_library (usb_conf_libs STATIC
  ${usb_conf_LIBS}/usb_bsp.c
  ${usb_conf_LIBS}/usbd_desc.c
  ${usb_conf_LIBS}/usbd_usr.c
)

#
# usb_lib
#

#link_directories    (${usb_lib_LIBS})
# otg
set (usb_lib_otg_LIBS lib/usb_lib/otg)
include_directories (${usb_lib_otg_LIBS})
#link_directories    (${usb_lib_otg_LIBS})
add_library (
  usb_lib_otg_libs STATIC
  ${usb_lib_otg_LIBS}/usb_core.c
  ${usb_lib_otg_LIBS}/usb_dcd.c
  ${usb_lib_otg_LIBS}/usb_dcd_int.c
)

# cdc
set (usb_lib_cdc_LIBS lib/usb_lib/cdc)
include_directories (${usb_lib_cdc_LIBS})
#link_directories    (${usb_lib_cdc_LIBS})
add_library (
  usb_lib_cdc_libs STATIC
  ${usb_lib_cdc_LIBS}/usbd_cdc_core.c
  ${usb_lib_cdc_LIBS}/usbd_cdc_vcp.c
)

# core
set (usb_lib_core_LIBS lib/usb_lib/core)
include_directories (${usb_lib_core_LIBS})
#link_directories    (${usb_lib_core_LIBS})
add_library (
  usb_lib_core_libs STATIC
  ${usb_lib_core_LIBS}/usbd_core.c
  ${usb_lib_core_LIBS}/usbd_ioreq.c
  ${usb_lib_core_LIBS}/usbd_req.c
)

#
# NRF
#
set (NRF_LIBS lib/nrf)
include_directories (${NRF_LIBS})
#link_directories    (${NRF_LIBS}) 
add_library (nrf_lib STATIC
  ${NRF_LIBS}/CONFIG.cpp
  ${NRF_LIBS}/NRF24.cpp
  ${NRF_LIBS}/NRF_REGISTER_MAP.cpp
  ${NRF_LIBS}/REGISTER.cpp
  ${NRF_LIBS}/SPI_interface.cpp
)

#
# SRC
#

set (SRC src/main.cpp) 

set (LIBS
  nrf_lib
  utils_lib
  STM32F4xx_StdPeriph_Driver_libs
  usb_lib_cdc_libs
  usb_lib_core_libs
  usb_lib_otg_libs
  usb_conf_libs
)

#
# BIN
#
add_executable        (roboime_fw_v1.0.elf ${SRC})
target_link_libraries (roboime_fw_v1.0.elf ${LIBS})
add_bin_from_elf      (roboime_fw_v1.0.bin roboime_fw_v1.0.elf)
