/*
 * Copyright 2024,2025, Chris Kottaridis. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This contains the transfer commands to devices on the i2c bus
 */

#ifndef QW_DEVICES_I2C_INCLUDE_I2CBUS_H_
#define QW_DEVICES_I2C_INCLUDE_I2CBUS_H_

#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <mutex>
#include <string>

namespace qw::devices {

enum I2cBusStatus {
  I2CBUS_STATUS_OK,
  I2CBUS_STATUS_NODEV,
  I2CBUS_STATUS_UNKNOWN_FUNCTIONS,
  I2CBUS_STATUS_UNDEFINED
};

/*
 * I2C device name prefix.
 * all I2C device names begin with this value
 */
constexpr std::string i2c_devicename_prefix = "/dev/i2c-";

class I2cBus {
 public:
  explicit I2cBus(std::string bus_name);

  /*
   * This is for use with devices that use a command/result model. This writes a
   * command to the slave_address. It then reads from the slave address to get
   * the results of that command. Typically there is a delay between writing the
   * command and reading the result.
   * The sht4x works this way.
   */
  int writeCommand(uint8_t slave_address, uint8_t* command, uint8_t count);

  /*
   * This command covers devices that first send a command via a write. The
   * subsequent read of just the slave address will return the results.
   * These often require a delay to give some time for the command to
   * get executed on the device. It is then followed by a read.
   * This is how the sht4x device operates.
   */
  int readCommandResult(uint8_t slave_address, uint8_t* buffer, uint8_t count);

  /*
   * This routine copies multiple address' from the device to memory
   * The sht4x works this way.
   */
  int transferDataFromRegisters(uint8_t slave_address, uint8_t sub_adress,
                                uint8_t* buffer, uint8_t count);

  /*
   * This routine copies data from memory to multiple sub-address' in the device
   * The sht4x works this way.
   */
  int transferDataToRegisters(uint8_t slave_address, uint8_t sub_adress,
                              uint8_t* buffer, uint8_t count);

  /*
   * This routine returns the device name
   */
  std::string busName();

  I2cBusStatus status();

 private:
  static std::mutex i2cbus_lock;

  std::string bus_device_name_;

  uint64_t i2c_functions_ = 0;

  I2cBusStatus status_ = I2CBUS_STATUS_OK;
};

}  // namespace qw::devices

#endif  // QW_DEVICES_I2C_INCLUDE_I2CBUS_H_
