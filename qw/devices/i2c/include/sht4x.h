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
 * This contains the SHT45 information. See data sheet
 * https://cdn-shop.adafruit.com/product-files/5665/5665_Datasheet_SHT4x.pdf
 *
 */

#ifndef QW_DEVICES_I2C_INCLUDE_SHT4X_H_
#define QW_DEVICES_I2C_INCLUDE_SHT4X_H_

#include <errno.h>
#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
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
 * This device has temperature and relative humidity sensors so add the units
 * and measurements.
 */
#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/temperature/include/celsius.h"
#include "qw/units/temperature/include/fahrenheit.h"
#include "qw/units/temperature/include/kelvin.h"
#include "qw/units/temperature/include/temperature.h"
#include "qw/units/include/unit_measurement.h"

namespace qw::devices {

/*
 * Fixed address. could be 0x45 you have to check the model from the data sheet
 */
constexpr uint8_t kSht4xI2cPrimaryAddress = 0x44;
constexpr uint8_t kSht4xI2cSecondaryAddress = 0x45;

const std::vector<uint8_t> sht4x_slave_address_options = {
    kSht4xI2cPrimaryAddress, kSht4xI2cSecondaryAddress};

constexpr std::chrono::milliseconds kDefaultMeasurementInterval(
    2000); /* The number of msecs that a reading is good */
constexpr std::chrono::milliseconds kSht44xMinimumMeasurementInterval(
    1000); /* The minimum value for measurement_interval_ */
constexpr std::chrono::milliseconds kSteadyClockZero(0);

constexpr float kSht4xHumidityMin = 0;
constexpr float kSht4xHumidityMax = 100;
/*
 * These were found in the datasheet
 */
const qw::units::Celsius kSht4xTemperatureAccuracy(.5);
const qw::units::RelativeHumidity kSht44xHumidityAccuracy(.1);
/*
 * Commands
 */
constexpr uint8_t kSht4xResponseLength =
    6;  // All commands have same response length
constexpr uint8_t kSht4xCommandLength = 1;
constexpr uint8_t kSht4xSerialReturnLength = 6;
constexpr uint8_t kSht4xResetLength = 1;
constexpr uint8_t kSht4xReadDataLength = 6;
constexpr uint8_t kSht4xDataTemperatureMsbOffset = 0;
constexpr uint8_t kSht4xDataTemperatureLsbOffset = 1;
constexpr uint8_t kSht4xDataTemperatureCrcOffset = 2;
constexpr uint8_t kSht4xDataRelativeHumidityMsbOffset = 3;
constexpr uint8_t kSht4xDataRelativeHumidityLsbOffset = 4;
constexpr uint8_t kSht4xDataRelaiveHumidityCrcOffset = 5;

constexpr uint8_t kSht4xCommandHighPrecisionMeasurement = 0xFD;
constexpr uint8_t kSht4xCommandMediumPrecisionMeasurement = 0xF6;
constexpr uint8_t kSht4xCommandLowPrecisionMeasurement = 0xE0;
constexpr uint8_t kSht4xCommandReadSerialNumber = 0x89;
constexpr uint8_t kSht4xCommandReset = 0x94;
constexpr uint8_t kSht4xCommandActivateHtr200mwOneSecond = 0x39;
constexpr uint8_t kSht4xCommandActivateHtr200mwTenthSecond = 0x32;
constexpr uint8_t kSht4xCommandActivateHtr110mwOneSecond = 0x2F;
constexpr uint8_t kSht4xCommandActivateHtr110mwTenthSecond = 0x24;
constexpr uint8_t kSht4xCommandActivateHtr20mwOneSecond = 0x1E;
constexpr uint8_t kSht4xCommandActivateHtr20mwTenthSecond = 0x15;

constexpr int kSht4xTemperatureCelsiusMultiplier = 175;
constexpr int kSht4xTemperatureCelsisusDivisor = 65535;
constexpr int kSht4xTemperatureCelsiusOffset = 45;

constexpr int kSht4xTemperatureFarenheitMultiplier = 315;
constexpr int kSht4xTemperatureFarenheitDivisor = 65535;
constexpr int kSht4xTemperatureFahrenheitOffset = 49;

constexpr int kSht4xRelativeHumidityMultiplier = 125;
constexpr int kSht4xRelativeHumidityDivisor = 65535;
constexpr int kSht4xRelativeHumidityOffset = 6;

constexpr int kSht4xTemperatureKelvinMultiplier =
    kSht4xTemperatureCelsiusMultiplier;
constexpr int kSht4xTemperatureKelvinDivisor = kSht4xTemperatureCelsisusDivisor;
constexpr float kSht4xTemperatureKelvinOffset =
    kSht4xTemperatureCelsiusOffset + 273.15;

typedef enum {
  SHT4X_MEASUREMENT_PRECISION_HIGH,
  SHT4X_MEASUREMENT_PRECISION_MEDIUM,
  SHT4X_MEASUREMENT_PRECISION_LOW,
  SHT4X_HEATER_ACTkSIVATION_200mW_ONE_SECOND,
  SHT4X_HEATER_ACTIVATION_200mW_TENTH_SECOND,
  SHT4X_HEATER_ACTIVATION_110mW_ONE_SECOND,
  SHT4X_HEATER_ACTIVATION_110mW_TENTH_SECOND,
  SHT4X_HEATER_ACTIVATION_20mW_ONE_SECOND,
  SHT4X_HEATER_ACTIVATION_20mW_TENTH_SECOND,
} Sht4xMeasurmentMode;

/*
 * The order here has to match the enum above so that you get the right value in the array
 */
const uint8_t sht4x_measurement_command_map[] = {
    kSht4xCommandHighPrecisionMeasurement,
    kSht4xCommandMediumPrecisionMeasurement,
    kSht4xCommandLowPrecisionMeasurement,
    kSht4xCommandActivateHtr200mwOneSecond,
    kSht4xCommandActivateHtr200mwTenthSecond,
    kSht4xCommandActivateHtr110mwOneSecond,
    kSht4xCommandActivateHtr110mwTenthSecond,
    kSht4xCommandActivateHtr20mwOneSecond,
    kSht4xCommandActivateHtr20mwTenthSecond};

/*
 * These are delay times from when a command is given to when it completes.
 * See section 3.1 of the Datasheet.
 */
typedef enum {
  SHT4X_TIMING_SOFT_RESET,
  SHT4X_TIMING_MEASUREMENT_LOW_REPEATABILITY,
  SHT4X_TIMING_MEASUREMENT_MED_REPEATABILITY,
  SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY,
  SHT4X_TIMING_HEATER_DURATION_LONG,
  SHT4X_TIMING_HEATER_DURATION_SHORT,
} Sht4xMaxTimings;

/*
 * These are in microseonds
 */
static std::map<Sht4xMaxTimings, int> sht4x_min_delays = {
    {SHT4X_TIMING_SOFT_RESET, 1000}, /* tpu 1 millisecond */
    /*
     * For the next 3 the table says including tpu. I take that
     * to mean I should add the value above, tpu, to these values
     * in the table. I may have that wrong though.
     */
    {SHT4X_TIMING_MEASUREMENT_LOW_REPEATABILITY,
     1600 + 1000}, /* 1.6 millisecond  + tpu = 2.6 milliseconds*/
    {SHT4X_TIMING_MEASUREMENT_MED_REPEATABILITY,
     500 + 1000}, /* 4.5 milliseconds + tpu = 5.5 milliseconds */
    {SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY,
     8300 + 1000}, /* 8.3 milliseconds + tpu = 9.3 milliseconds */
    {SHT4X_TIMING_HEATER_DURATION_LONG, 1100000}, /* 1.1 seconds */
    {SHT4X_TIMING_HEATER_DURATION_SHORT, 110000}  /* 0.11 seconds */
};

typedef enum { SHT4X_TEMPERATURE, SHT4X_HUMIDITY } Sht4xReading_t;

class Sht4xDeviceLocation {
 public:
  std::string bus_name_;
  uint8_t slave_address_;

  /*
   * We need the == comparison to support the contain function for this class
   * to be used as a key in a map.
   */
  bool operator==(const Sht4xDeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) &&
        (slave_address_ == data.slave_address_)) {
      return true;
    }
    return false;
  }

  /*
   * If you have == you should also have !=
   */
  bool operator!=(const Sht4xDeviceLocation& data) const {
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
  bool operator<(const Sht4xDeviceLocation& data) const {
    if (bus_name_.compare(data.bus_name_) < 0)
      return true;
    if (bus_name_.compare(data.bus_name_) > 0)
      return false;
    if (slave_address_ < data.slave_address_)
      return true;
    return false;
  }
};

class Sht4xDeviceData {
 public:
  std::recursive_mutex lock_ = {};
  uint64_t read_total_ = 0;
  std::atomic_bool initialized = false;
  uint16_t temperature_measurement_ = 0;

  /*
   * The time we read in the temperature
   */
  std::chrono::time_point<std::chrono::system_clock>
      temperature_measurement_system_time_;
  std::chrono::time_point<std::chrono::steady_clock>
      temperature_measurement_steady_time_;

  uint16_t humidity_measurement_ = 0;

  /*
   * The time we read in the humidity
   */
  std::chrono::time_point<std::chrono::system_clock>
      humidity_measurement_system_time_;
  std::chrono::time_point<std::chrono::steady_clock>
      humidity_measurement_steady_time_;
};

class I2cSht4x {
 public:
  I2cSht4x(I2cBus i2cbus_, uint8_t slave_address);

  uint8_t deviceAddress();

  int init();

  std::expected<uint32_t, int> getSerialNumber();

  int softReset();

  std::expected<qw::units::UnitMeasurement<qw::units::Temperature>, int>
  getTemperatureMeasurement();

  std::expected<qw::units::UnitMeasurement<qw::units::RelativeHumidity>, int>
  getRelativeHumidityMeasurement();

  std::chrono::milliseconds getMeasurementInterval(Sht4xReading_t reading);

  int setMeasurementInterval(std::chrono::milliseconds interval,
                             Sht4xReading_t reading);

  int error_code();

  std::string error_message();

 private:
  /*
    * Private Data
    */
  static std::mutex sht4x_devices_lock;
  static std::map<Sht4xDeviceLocation, std::shared_ptr<Sht4xDeviceData>>
      sht4x_devices;

  Sht4xDeviceLocation device_;
  std::shared_ptr<Sht4xDeviceData> device_data_ = nullptr;
  // Number of measurements made
  uint64_t measurement_count_ = 0;

  // The slave address of the device. The sht45 can be either 0x44 or 0x45.
  uint8_t slave_address_;

  // Which I2c bus is the device on
  I2cBus i2cbus_;

  // interval between making a measurement per reading type
  std::chrono::milliseconds temperature_measurement_interval_ =
      kDefaultMeasurementInterval;
  std::chrono::milliseconds humidity_measurement_interval_ =
      kDefaultMeasurementInterval;

  // Serial Number
  uint32_t serial_number_ = 0;

  int error_code_ = 0;

  std::string error_message_ = {};
  /*
   * Private Functions
   */
  int getMeasurement(Sht4xMeasurmentMode mode);

  bool measurementExpired(
      std::chrono::time_point<std::chrono::steady_clock> last_read_time,
      std::chrono::milliseconds interval);
};

}  // namespace qw::devices

#endif  // QW_DEVICES_I2C_INCLUDE_SHT4X_H_
