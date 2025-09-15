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
 * This contains the driver for the LPS22 HB chip.
 */
#include "qw/devices/i2c/include/lps22.h"

#include <errno.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <expected>  // Cpplint is wrong about this being a C system header // NOLINT
#include <map>
#include <memory>
#include <vector>

#include "qw/units/pressure/include/millibar.h"  // This device measures in millibars
#include "qw/units/temperature/include/celsius.h"  // This device measures in Celsius
#include "qw/units/include/unit_measurement.h"

using qw::units::Celsius;
using qw::units::Temperature;
using qw::units::Millibar;
using qw::units::Pressure;
using qw::units::UnitMeasurement;
using std::expected;
using std::find;
using std::lock_guard;
using std::make_shared;
using std::map;
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

mutex Lps22::lps22_devices_lock;
map<Lps22DeviceLocation, shared_ptr<Lps22DeviceData>> Lps22::lps22_devices;

/*
 * Constructor
 */
Lps22::Lps22(I2cBus i2cbus, uint8_t slave_address)
    : i2cbus_(i2cbus), slave_address_(slave_address) {

  /*
   * Check that the i2c bus name is a valid name
   */
  if (i2cbus.busName().compare(0, i2c_devicename_prefix.size(),
                               i2c_devicename_prefix) != 0) {
    return;
  }
  /*
   * Check that slave addresses are valid
   */
  auto item = find(lps22_slave_address_options.begin(),
                   lps22_slave_address_options.end(), slave_address_);
  if (item == lps22_slave_address_options.end()) {
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
  lock_guard<mutex> guard_devices(lps22_devices_lock);
  if (lps22_devices.contains(device_) == true) {
    /*
     * Some previous instance has added the device to the devices list
     */
    device_data_ = lps22_devices[device_];
    return;
  }

  /*
   * Create a DeviceData and use it to see if whoami works
   * If whoami does not work then reset device_data_
   * 
   * TODO: I should be using make_shared but it doesn't compile.
   */
  device_data_ = shared_ptr<Lps22DeviceData>(new Lps22DeviceData());

  expected<uint8_t, int> x_return = whoAmI();
  if (x_return.has_value() == false) {
    /*
     * I can't get the whoami so reset device_data_ to nullptr
     * to indicate the device doesn't seem to be at the bus and slave
     * address provided.
     * This causes all other routines to call an ENODEV error
     */
    device_data_.reset();
    return;
  }

  /*
   * If I get here I got a value. Make sure it matches the 
   */
  if (x_return.value() != kLps22hbWhoAmIValue) {
    /*
     * free up the memory and set the device_data_ to nullptr
     */
    device_data_.reset();
    return;
  }
  lps22_devices[device_] = device_data_;

  return;
}

/*
 * Return the whoami value on the device
 */
expected<uint8_t, int> Lps22::whoAmI() {
  int retval;
  uint8_t who_am_i;

  if (device_data_ == nullptr) {
    return unexpected(retval);
  }

  /* get the device lock
   * device lock will be unlcoked when
   * guard's destruct routine gets called
   */
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbWhoAmI,
                                             &who_am_i, sizeof(who_am_i));

  if (retval != 0) {
    return unexpected(retval);
  }

  return who_am_i;
}

int Lps22::reset() {
  int retval;
  uint8_t control_1, control_2;

  if (device_data_ == nullptr) {
    return ENODEV;
  }

  /* get the device lock
   * device lock will be unlcoked when
   * guard's destruct routine gets called
   */
  lock_guard<recursive_mutex> guard(device_data_->lock_);
  /*
   * Now send a software reset
   */
  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbCtrlReg2,
                                             &control_2, sizeof(control_2));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }
  control_2 |= kLps22hbCtrlReg2SwResetMask;
  retval = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg2,
                                           &control_2, sizeof(control_2));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }

  /*
   * Wait for Reset to clear
   */
  int cnt = 0;
  do {
    cnt++;
    retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbCtrlReg2,
                                               &control_2, sizeof(control_2));
    if (retval != 0) {
      /*
       * Return whatever error was found at the lower area
       */
      return retval;
    }
  } while (((control_2 & kLps22hbCtrlReg2SwResetMask) ==
            kLps22hbCtrlReg2SwResetMask) &&
           cnt < kLps22ResetWaitCount);

  /*
   * Set to the default value
   */
  control_1 = kLps22hbCtrlReg1Default;
  retval = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg1,
                                           &control_1, sizeof(control_1));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }

  control_2 = kLps22hbCtrlReg2Default;
  retval = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg2,
                                           &control_2, sizeof(control_2));
  if (retval != 0) {
    /*
     * Return whatever error was found at the lower area
     */
    return retval;
  }

  return 0;
}

milliseconds Lps22::getMeasurementInterval(Lps22hbReading_t reading) {
  milliseconds interval(0);
  /*
   * Return the minimum interval allowed
   */
  switch (reading) {
    case LPS22HB_TEMPERATURE:
      interval = temperature_interval_;
      break;
    case LPS22HB_PRESSURE:
      interval = pressure_interval_;
      break;
  }

  return interval;
}

int Lps22::init() {
  expected<uint8_t, int> x_return;
  int retval = 0;

  if (device_data_ == nullptr) {
    return ENODEV;
  }
  /*
   * Make sure this is the correct device at the expected I2C address
   * It is assumed we can always get the whoami value.
   */
  x_return = whoAmI();
  /*
   * If I can't do a whoami we probably have the wrong device so
   * clear device_data_
   */
  if (x_return.has_value() == false) {
    /*
     * Return whatever error was found at the lower area reort it.
     * We assume the deice isn't at the bus,slave location so
     * free the device_ptr_
     */
    return x_return.error();
  }
  if (x_return.value() != kLps22hbWhoAmIValue) {
    /*
     * If the value is not a match record it as a Bad Message
     */
    return EBADMSG;
  }

  retval = reset();

  return retval;
}

int Lps22::setMeasurementInterval(milliseconds interval,
                                  Lps22hbReading_t reading) {
  milliseconds measurement_interval;

  measurement_interval = max(interval, kLps22MinimumMeasurementInterval);

  switch (reading) {
    case LPS22HB_TEMPERATURE:
      temperature_interval_ = measurement_interval;
      break;
    case LPS22HB_PRESSURE:
      pressure_interval_ = measurement_interval;
      break;
  }

  return 0;
}

int Lps22::getMeasurement() {
  int retval;
  uint8_t ctrl_register_2, data_available, pressure_available_count,
      temp_available_count;
  uint8_t temp_buffer[] = {0, 0};     // Temperature is 2 bytes
  uint8_t pres_buffer[] = {0, 0, 0};  // Pressure is 3 bytes
  bool temp_updated = false, pres_updated = false, temp_overrun = false,
       pres_overrun = false;
  int error;

  temperature_error_ = 0;
  pressure_error_ = 0;
  temperature_valid_ = false;
  pressure_valid_ = false;
  instance_measurement_count_++;
  device_data_->read_total_++;

  if (device_data_ == nullptr) {
    return ENODEV;
  }

  /*
   * Start a measurement by sending a one shot command
   * We need to read in control register 2 and set the
   */
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  /*
   * Start a measurement by sending a one shot command
   * We need to read in control register 2 and set the one shot bit.
   * Then write it back.
   */
  retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbCtrlReg2,
                                             &ctrl_register_2,
                                             sizeof(ctrl_register_2));
  if (retval != 0) {
    /*
     * If we got an error it applies to both the temperature and pressure
     */
    temperature_error_ = retval;
    pressure_error_ = retval;
    return retval;
  }
  ctrl_register_2 |= kLps22hbCtrlReg2OneShotMask;
  error = i2cbus_.transferDataToRegisters(slave_address_, kLps22hbCtrlReg2,
                                          &ctrl_register_2,
                                          sizeof(ctrl_register_2));
  if (error != 0) {
    /*
     * If we got an error it applies to both the temperature and pressure
     */
    temperature_error_ = retval;
    pressure_error_ = retval;
    return error;
  }

  /*
   * Now loop on the status register waiting for data to be available
   */
  for (temp_available_count = 1;
       temp_available_count <= kLps22WaitResponseLoopCount;
       temp_available_count++) {
    /*
     * Add a sleep to allow the command to execute on the device
     */
    usleep(10000);
    /*
     * Get the status register
     */
    retval = i2cbus_.transferDataFromRegisters(slave_address_, kLps22hbStatus,
                                               &data_available,
                                               sizeof(data_available));
    if (retval != 0) {
      /*
       * Since this loops we may have gotten a valid value for temperature or pressure
       * on a previous pass. So, we only count an error for a value that is not marked
       * as valid.
       */
      if (temperature_valid_ == false) {
        temperature_error_ = retval;
      }
      if (pressure_valid_ == false) {
        pressure_error_ = retval;
      }
      return retval;
    }

    /*
     * Check for overruns
     * An overrun means that a new value has overwritten a previous one.
     * Since we are doing one-shots I don't see this as a problem. We
     * only want this new value we don't care if an older value has been
     * overwritten.
     * We record it, but don't really use it.
     */
    if ((data_available & kLps22hbStatusTemperatureDataOverRunMask) ==
        kLps22hbStatusTemperatureDataOverRunMask) {
      temp_overrun = true;
    }
    if ((data_available & kLps22hbStatusPressureDataOverRunMask) ==
        kLps22hbStatusPressureDataOverRunMask) {
      pres_overrun = true;
    }

    /*
     * If there is pressure data ready then get the pressure data
     */
    if ((pres_updated == false) &&
        ((data_available & kLps22hbStatusPressureDataAvailableMask) ==
         kLps22hbStatusPressureDataAvailableMask)) {
      error = i2cbus_.transferDataFromRegisters(
          slave_address_, kLps22hbPressureOutXl, pres_buffer,
          sizeof(pres_buffer));
      if (error != 0) {
        /*
         * Return whatever error was found at the lower area
         */
        pressure_error_ = error;
        return error;
      }

      /*
       * Convert the two's compliment Pressure value to int32_t.
       */
      device_data_->pressure_measurement_ =
          (((pres_buffer[2] << 16) | (pres_buffer[1] << 8) | (pres_buffer[0])) ^
           kLps22hbPressure2ComplimentXorMask) -
          kLps22hbPressure2ComplimentXorMask;
      pressure_error_ = 0;
      pressure_valid_ = true;
      device_data_->pressure_measurement_system_time_ = system_clock::now();
      device_data_->pressure_measurement_steady_time_ = steady_clock::now();
    }

    /*
     * If there is temperature available ready get the temperature data
     */
    if ((temp_updated == false) &&
        ((data_available & kLps22hbStatusTemperatureDataAvailableMask) ==
         kLps22hbStatusTemperatureDataAvailableMask)) {
      error = i2cbus_.transferDataFromRegisters(
          slave_address_, kLps22hbTempOutL, temp_buffer, sizeof(temp_buffer));
      if (error != 0) {
        /*
         * Return whatever error was found at the lower area
         */
        temperature_error_ = error;
        return error;
      }

      /*
       * Convert the two's compliment Temperature value to int16_t.
       */
      device_data_->temperature_measurement_ =
          (((temp_buffer[1] << 8) | temp_buffer[0]) ^
           kLps22hbTemperature2ComplimentXorMask) -
          kLps22hbTemperature2ComplimentXorMask;
      temp_updated = true;
      temperature_valid_ = true;
      temperature_error_ = 0;
      device_data_->temperature_measurement_system_time_ = system_clock::now();
      device_data_->temperature_measurement_steady_time_ = steady_clock::now();
    }

    if ((temp_updated == true) && (pres_updated == true)) {
      break;
    }
  }

  return 0;
}

expected<UnitMeasurement<Temperature>, int> Lps22::getTemperatureMeasurement() {
  uint8_t buffer[2] = {0, 0};
  float temperature;
  int error;

  if (device_data_ == nullptr) {
    return unexpected(ENODEV);
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  if (measurementExpired(device_data_->temperature_measurement_steady_time_,
                         temperature_interval_) == true) {
    /*
     * The device seems to always return a temperature of 0 Centigrade
     * on the first read after a power cycle.
     * So, if this is the first read after a power cycle get another measurment
     */
    error = getMeasurement();
    if (device_data_->read_total_ == 1) {
      error = getMeasurement();
    }
  }

  if (temperature_valid_ == false) {
    return unexpected(temperature_error_);
  }

  /*
   * Perform the conversion from the data sheet gives you degrees in celsius
   */
  temperature = static_cast<float>(device_data_->temperature_measurement_) /
                kLps22hbTemperatureFactor;

  /*
   * create a temprature in celsius
   */
  Celsius tempc(temperature);

  UnitMeasurement<Temperature> measurement(
      tempc, kLps22hbTemperatureAccuracy,
      device_data_->temperature_measurement_system_time_);

  return measurement;
}

expected<UnitMeasurement<Pressure>, int> Lps22::getPressureMeasurement() {
  uint8_t buffer[3] = {0, 0, 0};
  float pressure;
  int error;

  if (device_data_ == nullptr) {
    return unexpected(ENODEV);
  }
  lock_guard<recursive_mutex> guard(device_data_->lock_);

  if (measurementExpired(device_data_->pressure_measurement_steady_time_,
                         pressure_interval_) == true) {
    error = getMeasurement();
    if (pressure_valid_ == false) {
      return unexpected(pressure_error_);
    }
  }

  /*
   * pressure is measured in hPa which is same as millibar
   */
  pressure = static_cast<float>(device_data_->pressure_measurement_) /
             kLps22hbPressureHpaFactor;

  Millibar mb(pressure);

  UnitMeasurement<Pressure> measurement(
      mb, kLps22hbPressureAccuracy,
      device_data_->pressure_measurement_system_time_);

  return measurement;
}

/*
 * Private methods
 */

bool Lps22::measurementExpired(time_point<steady_clock> last_read_time,
                               milliseconds interval) {
  /*
   * check if the current measurement has expired
   */
  if (instance_measurement_count_ == 0) {
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
