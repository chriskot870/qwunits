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
 * This contains the LPS22HB information.
 * See data sheet that can be downloaded from
 * https://www.st.com/en/mems-and-sensors/lps22hb.html
 *
 */

#ifndef QW_DEVICES_I2C_INCLUDE_LPS22_H_
#define QW_DEVICES_I2C_INCLUDE_LPS22_H_

#include <errno.h>
#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdatomic.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <expected>  // Lint incorrectly counts this as a C header // NOLINT
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

/*
 * This is an i2c bus device so add the i2cbus.h
 */
#include "qw/devices/i2c/include/i2cbus.h"

/*
 * This device provides temperature and pressure data so include the interfaces.
 * The device makes temperature and pressure measurements so add those includes.
 */
#include "qw/units/pressure/include/inches_mercury.h"
#include "qw/units/pressure/include/millibar.h"
#include "qw/units/pressure/include/pressure.h"
#include "qw/units/temperature/include/celsius.h"
#include "qw/units/temperature/include/fahrenheit.h"
#include "qw/units/temperature/include/kelvin.h"
#include "qw/units/temperature/include/temperature.h"
#include "qw/units/include/unit_measurement.h"

namespace qw::devices {

constexpr uint8_t kLps22ResetWaitCount = 10;

constexpr std::chrono::milliseconds kLps22MinimumMeasurementInterval(
    1000); /* The minimum value for measurement_interval_ */

constexpr std::chrono::milliseconds kLps22DefaultMeasurementInterval(
    2000); /* The number of msecs that a reading is good */

constexpr int kLps22WaitResponseLoopCount = 10;
/*
 * There are two possible slave addresses
 */
constexpr uint8_t kLps22hbI2cPrimaryAddress =
    0x5D;  // See data sheet section 7.2.1.
constexpr uint8_t kLps22hbI2cSecondaryAddress =
    0x5C;  // See data sheet section 7.2.1.

const std::vector<uint8_t> lps22_slave_address_options = {
    kLps22hbI2cPrimaryAddress, kLps22hbI2cSecondaryAddress};

/*
 * The constant value always retrurned by the Who Am I register
 */
constexpr uint8_t kLps22hbWhoAmIValue = 0xB1;  // This is the value for LPS22HB
constexpr int kLps22hbPressureHpaFactor =
    4096;  // convert measured value to hPa
constexpr int kLps22hbTemperatureFactor = 100;
constexpr uint8_t kLps22hbMaxRegistersTransferred =
    5;  // The maximum number of registers in one transfer

/*
 * These are gotten from the data sheet
 */
const qw::units::Celsius kLps22hbTemperatureAccuracy(.1);
const qw::units::Millibar kLps22hbPressureAccuracy(.1);
/*
 * Registers
 */
constexpr uint8_t kLps22hbInterruptCfg = 0x0B;
constexpr uint8_t kLps22hbThsPL = 0x0C;
constexpr uint8_t kLps22hbThsPH = 0x0D;

constexpr uint8_t kLps22hbWhoAmI = 0x0F;
constexpr uint8_t kLps22hbCtrlReg1 = 0x10;
constexpr uint8_t kLps22hbCtrlReg2 = 0x11;
constexpr uint8_t kLps22hbCtrlReg3 = 0x12;

constexpr uint8_t kLps22hbFifoCtrl = 0x14;
constexpr uint8_t kLps22hbRefPXL = 0x15;
constexpr uint8_t kLps22hbRefPL = 0x16;
constexpr uint8_t kLps22hbRefPH = 0x17;
constexpr uint8_t kLps22hbRpdsL = 0x18;
constexpr uint8_t kLps22hbRpdsH = 0x19;
constexpr uint8_t kLps22hbResConf = 0x1A;

constexpr uint8_t kLps22hbIntSource = 0x25;
constexpr uint8_t kLps22hbFifoStatus = 0x26;
constexpr uint8_t kLps22hbStatus = 0x27;
constexpr uint8_t kLps22hbPressureOutXl = 0x28;
constexpr uint8_t kLps22hbPressureOutL = 0x29;
constexpr uint8_t kLps22hbPressureOutH = 0x2A;
constexpr uint8_t kLps22hbTempOutL = 0x2B;
constexpr uint8_t kLps22hbTempOutH = 0x2C;

constexpr uint8_t kLps22hbLpfpRes = 0x33;

constexpr uint8_t kLps22hbResConfMaskLcEn = 0x1;

/*
 * Control Register 1 Info
 */
constexpr uint8_t kLps22hbCtrlReg1Default = 0x0;  // THe default is all values 0
// Single bit flags
constexpr uint8_t kLps22hbCtrlReg1SimMask = 0x01;
constexpr uint8_t kLps22hbCtrlReg1BduMask = 0x02;
constexpr uint8_t kLps22hbCtrlReg1LpfpCfgMask = 0x04;
constexpr uint8_t kLps22hbCtrlReg1EnLpfpMask = 0x08;

// Multiple bit flags
constexpr uint8_t kLps22hbCtrlReg1OdrMask = 0x7;
constexpr uint8_t kLps22hbCtrlReg1OdrShift = 4;

typedef enum {
  LPS22HB_CTRL_REG_1_ODR_POWER_DOWN,
  LPS22HB_CTRL_REG_1_ODR_1_HZ,
  LPS22HB_CTRL_REG_1_ODR_10_HZ,
  LPS22HB_CTRL_REG_1_ODR_25_HZ,
  LPS22HB_CTRL_REG_1_ODR_50_HZ,
  LPS22HB_CTRL_REG_1_ODR_75_HZ,
  LPS22HB_CTRL_REG_1_ODR_MODE_MAX
} Lps22hbOdr_t;

/*
 * Control Register 2 Info
 */
constexpr uint8_t kLps22hbCtrlReg2Default = 0x10;
// Single bit flags
constexpr uint8_t kLps22hbCtrlReg2OneShotMask = 0x01;
constexpr uint8_t kLps22hbCtrlReg2SwResetMask = 0x04;
constexpr uint8_t kLps22hbCtrlReg2I2cDisMask = 0x08;
constexpr uint8_t kLps22hbCtrlReg2IfAddIncMask = 0x10;
constexpr uint8_t kLps22hbCtrlReg2StopOnFthMask = 0x20;
constexpr uint8_t kLps22hbCtrlReg2FifoEnMask = 0x40;
constexpr uint8_t kLps22hbCtrlReg2BootMask = 0x80;

/*
 * Control Register 3 
 */
constexpr uint8_t kLps22hbCtrlReg3Default = 0x00;
// Single bit flags
constexpr uint8_t kLps22hbCtrlReg3DrdyMask = 0x04;
constexpr uint8_t kLps22hbCtrlReg3FOvrMask = 0x08;
constexpr uint8_t kLps22hbCtrlReg3FFthMask = 0x10;
constexpr uint8_t kLps22hbCtrlReg3FFss5Mask = 0x20;
constexpr uint8_t kLps22hbCtrlReg3PpOvdMask = 0x40;
constexpr uint8_t kLps22hbCtrlReg3IntHLMask = 0x80;
// Mulitple bit flags
constexpr uint8_t kLps22hbCtrlReg3IntSMask = 0x03;
constexpr uint8_t kLps22hbCtrlReg3IntSShift = 0x00;

typedef enum {
  LPS22HB_CTRL_REG_3_INT_S_DATA_SIGNAL,
  LPS22HB_CTRL_REG_3_INT_S_PRESSURE_HIGH,
  LPS22HB_CTRL_REG_3_INT_S_PRESSURE_LOW,
  LPS22HB_CTRL_REG_3_INT_S_PRESSURE_LOW_OR_HIGH
} Lps22hbIntS_t;

/*
 * Status Register Info
 */
// Single bit flags
constexpr uint8_t kLps22hbStatusPressureDataAvailableMask = 0x01;
constexpr uint8_t kLps22hbStatusTemperatureDataAvailableMask = 0x02;
constexpr uint8_t kLps22hbStatusPressureDataOverRunMask = 0x10;
constexpr uint8_t kLps22hbStatusTemperatureDataOverRunMask = 0x20;

/*
 * FIFO Control Register
 */
// Multiple bit flags
constexpr uint8_t kLps22hbCtrlRegFifoCtrlFModeMask = 0x7;
constexpr uint8_t kLps22hbCtrlRegFifoCtrlFModeShift = 5;
constexpr uint8_t kLps22hbCtrlRegFifoCtrlWTMMask = 0x1F;
constexpr uint8_t kLps22hbCtrlRegFifoCtrlWTMShift = 0;

/*
 * Control register default values
 */
constexpr uint8_t default_ctrl_reg_1_ = 0;
constexpr uint8_t default_ctrl_reg_2_ =
    kLps22hbCtrlReg2IfAddIncMask;  // So we can do multiple register reads

typedef enum {
  LPS22HB_CTRL_FIFO_CTRL_BYPASS_MODE,
  LPS22HB_CTRL_FIFO_CTRL_FIFO_MODE,
  LPS22HB_CTRL_FIFO_CTRL_STREAM_MODE,
  LPS22HB_CTRL_FIFO_CTRL_STREAM_TO_FIFO_MODE,
  LPS22HB_CTRL_FIFO_CTRL_BYPASS_TO_STREAM_MODE,
  LPS22HB_CTRL_FIFO_CTRL_RESERVED_MODE,
  LPS22HB_CTRL_FIFO_CTRL_DYNAMIC_STREAM_MODE,
  LPS22HB_CTRL_FIFO_CTRL_BYPASS_TO_FIFO_MODE
} Lps22hbFifoCtrl_t;

constexpr int kLps22hbTemperature2ComplimentXorMask = 1 << 15;
constexpr int kLps22hbPressure2ComplimentXorMask = 1 << 23;

typedef enum { LPS22HB_TEMPERATURE, LPS22HB_PRESSURE } Lps22hbReading_t;

class Lps22DeviceLocation {
 public:
  std::string bus_name_;
  uint8_t slave_address_;

  /*
   * We need the == comparison to support the contain function for this class
   * to be used as a key in a map.
   */
  bool operator==(const Lps22DeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) &&
        (slave_address_ == data.slave_address_)) {
      return true;
    }
    return false;
  }

  /*
   * If you have == you should also have !=
   */
  bool operator!=(const Lps22DeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) &&
        (slave_address_ == data.slave_address_)) {
      return false;
    }
    return true;
  }

  /*
   * We need the < operator in order to use this class as a key for a map
   * We set the order that the busname is checked then slave on that bus
   */
  bool operator<(const Lps22DeviceLocation& data) const {
    if (bus_name_.compare(data.bus_name_) < 0)
      return true;
    if (bus_name_.compare(data.bus_name_) > 0)
      return false;
    if (slave_address_ < data.slave_address_)
      return true;
    return false;
  }
};

class Lps22DeviceData {
 public:
  std::recursive_mutex lock_ = {};
  uint64_t read_total_ = 0;
  atomic_bool initialized = false;

  /*
   * The time we read in the temperature
   */
  int16_t temperature_measurement_ = 0;
  std::chrono::time_point<std::chrono::system_clock>
      temperature_measurement_system_time_;
  std::chrono::time_point<std::chrono::steady_clock>
      temperature_measurement_steady_time_;
  std::chrono::milliseconds temperature_response_time;

  int32_t pressure_measurement_ = 0;
  std::chrono::time_point<std::chrono::system_clock>
      pressure_measurement_system_time_;
  std::chrono::time_point<std::chrono::steady_clock>
      pressure_measurement_steady_time_;
  std::chrono::milliseconds pressure_response_time;
};

class Lps22 {
 public:
  Lps22(I2cBus i2cbus_, uint8_t slave_address);

  int reset();

  int init();

  std::expected<uint8_t, int> whoAmI();

  std::expected<qw::units::UnitMeasurement<qw::units::Temperature>, int>
  getTemperatureMeasurement();

  std::expected<qw::units::UnitMeasurement<qw::units::Pressure>, int> getPressureMeasurement();

  std::chrono::milliseconds getMeasurementInterval(Lps22hbReading_t reading);

  int setMeasurementInterval(std::chrono::milliseconds interval,
                             Lps22hbReading_t reading);

 private:
  /*
   * Private Variables
   */
  static std::mutex lps22_devices_lock;
  static std::map<Lps22DeviceLocation, std::shared_ptr<Lps22DeviceData>>
      lps22_devices;
  /*
   * There can be multiple instances of this class.
   */

  Lps22DeviceLocation
      device_;  // Where the device is located on the system, bus and slave
  std::shared_ptr<Lps22DeviceData> device_data_ = nullptr;
  I2cBus i2cbus_;          // The i2c bus used to transfer data
  uint8_t slave_address_;  // slave address for device on the bus
  atomic_uint64_t instance_measurement_count_ = 0;
  std::chrono::milliseconds temperature_interval_ =
      kLps22DefaultMeasurementInterval;
  std::chrono::milliseconds pressure_interval_ =
      kLps22DefaultMeasurementInterval;

  bool temperature_error_;
  bool temperature_valid_ = false;

  bool pressure_error_;
  bool pressure_valid_ = false;

  int error_code_ = 0;

  std::string error_message_ = {};

  /*
   * This is only going to be used for taking ambient temperature and
   * barometric temperature which doesn't change within a second. So,
   * we don't need to enable the high speed modes that take measurements
   * automatically every second or less.
   * So, we want to run in power down mode then use one shot to start a
   * measurment of both pressure and temperature. We have to check the status
   * bits to wait for the temperature and pressure to take their reading.
   * Then we can read both the temperature and pressure in one read.
   */
  const uint8_t default_ctrl_reg_1_ = 0;

  const uint8_t default_ctrl_reg_2_ =
      kLps22hbCtrlReg2IfAddIncMask;  // So we can do multiple register reads

  /*
    * Private Functions
    */

  int getMeasurement();

  bool measurementExpired(
      std::chrono::time_point<std::chrono::steady_clock> last_read_time,
      std::chrono::milliseconds interval);
};

}  // namespace qw::devices

#endif  // QW_DEVICES_I2C_INCLUDE_LPS22_H_
