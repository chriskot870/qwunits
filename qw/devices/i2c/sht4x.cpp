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
 * This contains the driver for the SHT4x series. Specifically the SHT45
 */
#include "qw/devices/i2c/include/sht4x.h"

#include <errno.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <expected>  // Cpplint is wrong to think this is a c system header // NOLINT
#include <map>
#include <memory>
#include <vector>

#include "qw/units/humidity/include/relative_humidity.h"  // This device measures in RH
#include "qw/units/temperature/include/celsius.h"  // This device measures in Celsius
#include "qw/units/include/unit_measurement.h"

using qw::units::Celsius;
using qw::units::Temperature;
using qw::units::RelativeHumidity;
using qw::units::UnitMeasurement;
using std::atomic_bool;
using std::expected;
using std::find;
using std::lock_guard;
using std::make_shared;
using std::map;
using std::max;
using std::min;
using std::mutex;
using std::recursive_mutex;
using std::shared_ptr;
using std::string;
using std::unexpected;
using std::vector;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::chrono::system_clock;
using std::chrono::time_point;

namespace qw::devices {

mutex I2cSht4x::sht4x_devices_lock;
map<Sht4xDeviceLocation, shared_ptr<Sht4xDeviceData>> I2cSht4x::sht4x_devices;

I2cSht4x::I2cSht4x(I2cBus i2cbus, uint8_t slave_address)
    : i2cbus_(i2cbus), slave_address_(slave_address) {

  /*
   * Check that the i2c bus name is a valid name
   */
  if (i2cbus.busName().compare(0, i2c_devicename_prefix.size(),
                               i2c_devicename_prefix) != 0) {
    return;
  }
  /*
   * Check that slave address is valid
   */
  auto item = find(sht4x_slave_address_options.begin(),
                   sht4x_slave_address_options.end(), slave_address_);
  if (item == sht4x_slave_address_options.end()) {
    return;
  }

  /*
   * If the names are valid, Create the device
   */
  device_.bus_name_ = i2cbus.busName();
  device_.slave_address_ = slave_address;

  /*
   * If the device is already on the list then some other instance has
   * validated it and we don't need to add it to the list
   */
  lock_guard<mutex> guard_devices(sht4x_devices_lock);
  if (sht4x_devices.contains(device_) == true) {
    /*
     * Some previous instance has added the device to the devices list
     */
    device_data_ = sht4x_devices[device_];
    return;
  }

  /*
   * Create a DeviceData and use it to see if serial number succeeds
   * If serial number succeeds we count it OK. We don't know what
   * the value should be for each device so we assume if we don't
   * get an error it is an SHT4x device
   * 
   * TODO: I should be using make_shared but it doesn't compile.
   */
  device_data_ = shared_ptr<Sht4xDeviceData>(new Sht4xDeviceData());

  expected<uint8_t, int> x_return = getSerialNumber();
  if (x_return.has_value() == false) {
    /*
     * I can't get the serial number so reset device_data_ to nullptr
     * to indicate the device doesn't seem to be at the bus and slave
     * address provided.
     * This causes all other routines to call an ENODEV error
     */
    device_data_.reset();
    // delete device_data_;
    // device_data_ = nullptr;
    return;
  }

  sht4x_devices[device_] = device_data_;

  return;
}

uint8_t I2cSht4x::deviceAddress() {
  return slave_address_;
}

expected<uint32_t, int> I2cSht4x::getSerialNumber() {
  int32_t retval;
  int i2c_bus;
  uint8_t command = kSht4xCommandReadSerialNumber;

  /*
   * Serial is two 16 bit values with a CRC after each one. So we need to read 6 bytes
   */
  uint8_t read_buffer[kSht4xSerialReturnLength] = {0, 0, 0, 0, 0, 0};

  /*
   * The serial number shouldn't change so we only have to get it once.
   * Check to see if all elements of the private serial number are 0.
   * If so then it means we haven't read the serial number before.
   * So, read it now.
   */

  if (serial_number_ != 0) {
    return serial_number_;
  }

  if (device_data_ == nullptr) {
    return unexpected(ENODEV);
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  /*
   * Write the serial number command to the device
   */
  retval = i2cbus_.writeCommand(slave_address_, &command, sizeof(command));

  /*
   * I seem to need a delay here so use the high reliability time
   */
  usleep(sht4x_min_delays[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * Now get the result of the command
   */
  retval = i2cbus_.readCommandResult(slave_address_, read_buffer,
                                     sizeof(read_buffer));
  /*
   * I am not sure I need this extra or not.
   * This could be just tpu value but for now give me plenty of delay
   */
  usleep(sht4x_min_delays[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * read_buffer high order bytes are in offsets 0 and 1. Offset 2 is the
   * CRC for offset 0 and 1. Offsets 3 and 4 are the lower order bytes
   * followed by a CRC of offset 3 and 4 in offset 5.
   */
  serial_number_ = (read_buffer[0] << 24) + (read_buffer[1] << 16) +
                   (read_buffer[3] << 8) + read_buffer[4];

  return serial_number_;
}

int I2cSht4x::softReset() {
  int32_t retval;
  uint8_t command = kSht4xCommandReset;

  if (device_data_ == nullptr) {
    return ENODEV;
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  /*
   * Write the command to the devices
   */
  retval = i2cbus_.writeCommand(slave_address_, &command, sizeof(command));
  /*
   * Give it time to do the reset
   */
  usleep(sht4x_min_delays[SHT4X_TIMING_SOFT_RESET]);

  return 0;
}

expected<UnitMeasurement<Temperature>, int> I2cSht4x::getTemperatureMeasurement() {
  int error;
  float temperature;

  if (device_data_ == nullptr) {
    return unexpected(ENODEV);
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  if (measurementExpired(device_data_->temperature_measurement_steady_time_,
                         temperature_measurement_interval_) == true) {
    error = getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
    if (error != 0) {
      return unexpected(error);
    }
  }

  /*
   * The temprature in Celsius
   */
  temperature = ((kSht4xTemperatureCelsiusMultiplier *
                  static_cast<float>(device_data_->temperature_measurement_)) /
                 kSht4xTemperatureCelsisusDivisor) -
                kSht4xTemperatureCelsiusOffset;

  Celsius tempc(temperature);

  UnitMeasurement<Temperature> measurement(
      tempc, kSht4xTemperatureAccuracy,
      device_data_->temperature_measurement_system_time_);

  return measurement;
}

milliseconds I2cSht4x::getMeasurementInterval(Sht4xReading_t reading) {
  milliseconds interval(0);
  /*
   * Return the interval allowed
   */
  switch (reading) {
    case SHT4X_TEMPERATURE:
      interval = temperature_measurement_interval_;
      break;
    case SHT4X_HUMIDITY:
      interval = humidity_measurement_interval_;
      break;
  }

  return interval;
}

int I2cSht4x::setMeasurementInterval(milliseconds interval,
                                     Sht4xReading_t reading) {
  milliseconds measurement_interval;

  measurement_interval = max(interval, kSht44xMinimumMeasurementInterval);

  switch (reading) {
    case SHT4X_TEMPERATURE:
      temperature_measurement_interval_ = measurement_interval;
      break;
    case SHT4X_HUMIDITY:
      humidity_measurement_interval_ = measurement_interval;
      break;
  }

  return 0;
}

expected<UnitMeasurement<RelativeHumidity>, int>
I2cSht4x::getRelativeHumidityMeasurement() {
  float relative_humidity;
  int error;

  if (device_data_ == nullptr) {
    return unexpected(ENODEV);
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  if (measurementExpired(device_data_->humidity_measurement_steady_time_,
                         humidity_measurement_interval_) == true) {
    error = getMeasurement(SHT4X_MEASUREMENT_PRECISION_HIGH);
    if (error != 0) {
      return unexpected(error);
    }
  }

  relative_humidity =
      ((kSht4xRelativeHumidityMultiplier *
        static_cast<float>(device_data_->humidity_measurement_)) /
       kSht4xRelativeHumidityDivisor) -
      kSht4xRelativeHumidityOffset;

  /*
   * SHT4x document says:
   * cropping of the RH signal to the range of 0 %RH … 100 %RH is advised.
   */
  relative_humidity =
      min(kSht4xHumidityMax, max(kSht4xHumidityMin, relative_humidity));

  RelativeHumidity rhdata(relative_humidity);

  UnitMeasurement<RelativeHumidity> measurement(
      rhdata, kSht44xHumidityAccuracy,
      device_data_->humidity_measurement_system_time_);

  return measurement;
}

/*
 * Private Methods
 */
int I2cSht4x::getMeasurement(Sht4xMeasurmentMode mode) {
  int retval;
  int i2c_bus;
  struct i2c_msg fetch_serial_com;
  struct i2c_rdwr_ioctl_data xfer;
  uint8_t command = sht4x_measurement_command_map[mode];

  if (device_data_ == nullptr) {
    return ENODEV;
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  /*
   * The data is transferred in the following format
   * Offsets: 0,1 16 bits of temperature
   *          2   8 bits of CRC for temperature
   *          3,4 16 bits of humidity
   *          5   8 bits of CRC for humidity
   */
  uint8_t read_buffer[kSht4xResponseLength] = {0, 0, 0, 0, 0, 0};

  /*
   * Increment the counter that monitors how often this routine gets called
   */
  device_data_->read_total_++;
  measurement_count_++;

  i2cbus_.writeCommand(slave_address_, &command, sizeof(command));

  /*
   * Give it time to make the measurement
   */
  usleep(sht4x_min_delays[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * Now read the result
   */
  i2cbus_.readCommandResult(slave_address_, read_buffer, sizeof(read_buffer));

  /*
   * I am not sure I need this extra or not.
   * This could be just tpu value but for now give me plenty of delay
   */
  usleep(sht4x_min_delays[SHT4X_TIMING_MEASUREMENT_HIGH_REPEATABILITY]);

  /*
   * We need figure out how to check the CRC's
   */
  device_data_->temperature_measurement_ =
      (read_buffer[0] << 8) + read_buffer[1];
  device_data_->temperature_measurement_system_time_ = system_clock::now();
  device_data_->temperature_measurement_steady_time_ = steady_clock::now();

  device_data_->humidity_measurement_ = (read_buffer[3] << 8) + read_buffer[4];
  device_data_->humidity_measurement_system_time_ = system_clock::now();
  device_data_->humidity_measurement_steady_time_ = steady_clock::now();

  return 0;
}

bool I2cSht4x::measurementExpired(time_point<steady_clock> last_read_time,
                                  milliseconds interval) {
  /*
   * check if the current measurement has expired
   */
  if (measurement_count_ == 0) {
    return true;
  }
  time_point<steady_clock> now = steady_clock::now();

  auto time_diff = duration_cast<milliseconds>(now - last_read_time);

  if (time_diff > interval) {
    return true;
  }

  return false;
}

}  // namespace qw::devices
