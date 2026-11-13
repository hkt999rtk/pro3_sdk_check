/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2c_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed I2C API
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2006 - 2022 Realtek Corporation. All rights reserved.
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the License); you may
  * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#ifndef MBED_I2C_API_H
#define MBED_I2C_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_I2C
 *  @brief MBED_I2C driver modules.
 *  @{
 */

/** @addtogroup MBED_I2C_Exported_Types MBED_I2C Exported Types
  * @{
  */

/** @defgroup MBED_I2C_Enumeration_Type Enumeration Type
  * @{
  */

enum {
	I2C_ERROR_NO_SLAVE = -1,
	I2C_ERROR_BUS_BUSY = -2
};


#define IC_DMA_CR_TDMAE           (1<<1)
#define IC_DMA_CR_RDMAE           (1<<0)

#define I2C_STATE_READY           0x0
#define I2C_STATE_RX_BUSY         (1<<1)
#define I2C_STATE_TX_BUSY         (1<<0)

/**
  * @}
  */

/** @defgroup MBED_I2C_Structure_Type Structure Type
  * @{
  */

typedef struct i2c_s i2c_t;

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


void i2c_init(i2c_t *obj, PinName sda, PinName scl);
void i2c_speed_trim(i2c_t *obj, u32 pullup_ohm);
void i2c_frequency(i2c_t *obj, int hz);
int i2c_start(i2c_t *obj);
int i2c_stop(i2c_t *obj);
void i2c_reset(i2c_t *obj);
int i2c_byte_read(i2c_t *obj, int last);
int i2c_byte_write(i2c_t *obj, int data);
void i2c_slave_mode(i2c_t *obj, int enable_slave);
int i2c_slave_receive(i2c_t *obj);
void i2c_slave_address(i2c_t *obj, int idx, uint32_t address, uint32_t mask);
int i2c_read(i2c_t *obj, int address, char *data, int length, int stop);
int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop);
int i2c_slave_read(i2c_t *obj, char *data, int length);
int i2c_slave_write(i2c_t *obj, const char *data, int length);
int i2c_slave_set_for_rd_req(i2c_t *obj, int set);
int i2c_slave_set_for_data_nak(i2c_t *obj, int set_nak);

int i2c_int_write(i2c_t *obj, int address, const char *data, int length, int stop);
int i2c_int_read(i2c_t *obj, int address, const char *data, int length, int stop);
void i2c_wait_sema(u32 write);
void i2c_send_sema(u32 write);

int i2c_dma_send(i2c_t *obj, int address, char *data, int length);
int i2c_dma_recv(i2c_t *obj, int address, char *data, int length);

/**
  * @brief  I2C master read in poll mode, timeout-guarded.
  * @param  obj: I2C object defined in application software.
  * @param  address: Slave address which will be transmitted.
  * @param  data: Pointer to the buffer to hold the received data.
  * @param  length: Length of data that to be received.
  * @param  stop: Unused, always issues a STOP; kept for API symmetry.
  * @param  timeout_ms: Timeout in ms.
  * @return Length of actually received data.
  */
int i2c_read_timeout(i2c_t *obj, int address, char *data, int length, int stop, int timeout_ms);

/**
  * @brief  I2C master write in poll mode, timeout-guarded.
  * @param  obj: I2C object defined in application software.
  * @param  address: Slave address which will be transmitted.
  * @param  data: Pointer to the data to be sent.
  * @param  length: Length of data that to be sent.
  * @param  stop: Unused, always issues a STOP; kept for API symmetry.
  * @param  timeout_ms: Timeout in ms.
  * @return Length of actually sent data.
  */
int i2c_write_timeout(i2c_t *obj, int address, const char *data, int length, int stop, int timeout_ms);

/**
  * @brief  I2C master write, then read with a repeated start (Sr), timeout-guarded.
  * @note   Use for register-mapped devices instead of i2c_write_timeout()
  *         + i2c_read_timeout(), which both always end in a STOP and so
  *         lose the register pointer on such devices.
  * @param  obj: I2C object defined in application software.
  * @param  address: Slave address which will be transmitted.
  * @param  pWriteBuf: Pointer to the data to be sent (e.g. register address).
  * @param  Writelen: Length of data to be sent, 1 ~ 255.
  * @param  pReadBuf: Pointer to the buffer to hold the received data.
  * @param  Readlen: Length of data to be received.
  * @param  timeout_ms: Timeout for the read phase, in ms.
  * @return Length of data actually received; 0 if Writelen is out of range
  *         or the write phase failed.
  */
int i2c_repeatread_timeout(i2c_t *obj, int address, uint8_t *pWriteBuf, int Writelen, uint8_t *pReadBuf, int Readlen,
						   int timeout_ms);

/**
  * @brief  I2C master write, then read with a repeated start (Sr). The read
  *         phase is not timeout-guarded; prefer i2c_repeatread_timeout().
  * @param  obj: I2C object defined in application software.
  * @param  address: Slave address which will be transmitted.
  * @param  pWriteBuf: Pointer to the data to be sent.
  * @param  Writelen: Length of data to be sent.
  * @param  pReadBuf: Pointer to the buffer to hold the received data.
  * @param  Readlen: Length of data to be received.
  * @return Length of received data.
  */
int i2c_repeatread(i2c_t *obj, int address, uint8_t *pWriteBuf, int Writelen, uint8_t *pReadBuf, int Readlen);

/**
  * @brief  I2C master send bytes with a RESTART instead of STOP after the
  *         last byte, timeout-guarded.
  * @param  I2Cx: where I2Cx can be I2C0_DEV, I2C1_DEV, I2C2_DEV or I2C3_DEV.
  * @param  pBuf: Pointer to the data to be sent.
  * @param  len: Length of data to be sent.
  * @param  restart: 1 to issue RESTART after the last byte instead of STOP.
  * @return Length of data actually sent.
  */
int i2c_send_restart(I2C_TypeDef *I2Cx, u8 *pBuf, u8 len, u8 restart);

/**
  * @brief  Enable I2C master RESTART function (sets IC_CON.IC_RESTART_EN).
  * @param  obj: I2C object defined in application software.
  */
void i2c_restart_enable(i2c_t *obj);

/**
  * @brief  Disable I2C master RESTART function (clears IC_CON.IC_RESTART_EN).
  * @param  obj: I2C object defined in application software.
  */
void i2c_restart_disable(i2c_t *obj);

/**
  * @brief  Enable or disable the I2C peripheral.
  * @param  obj: I2C object defined in application software.
  * @param  enable: 1 to enable, 0 to disable.
  * @retval 0
  */
int i2c_enable_control(i2c_t *obj, int enable);


#ifdef __cplusplus
}
#endif

#endif/* MBED_I2C_API_H */
