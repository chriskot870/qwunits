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
 * This contains the driver for the ADS 1015 ADC chip.
 */
#include "qw/devices/i2c/include/ads1015.h"

#include <errno.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <expected>  // Lint incorrectly counts this as a C header // NOLINT
#include <map>
#include <memory>
#include <vector>

/*
 * This is an i2c bus device so add the i2cbus.h
 */
#include "qw/devices/i2c/include/i2cbus.h"

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

mutex I2cAds1015::ads1015_devices_lock;
map<Ads1015DeviceLocation, shared_ptr<Ads1015DeviceData>>
    I2cAds1015::ads1015_devices;

I2cAds1015::I2cAds1015(I2cBus i2cbus, uint8_t slave_address)
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
  auto item = find(ads1015_slave_address_options.begin(),
                   ads1015_slave_address_options.end(), slave_address_);
  if (item == ads1015_slave_address_options.end()) {
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
  lock_guard<mutex> guard_devices(ads1015_devices_lock);
  if (ads1015_devices.contains(device_) == true) {
    /*
     * Some previous instance has added the device to the devices list
     */
    device_data_ = ads1015_devices[device_];
    return;
  }

  /*
   * Create a DeviceData
   * 
   * TODO: I should be using make_shared but it doesn't compile.
   */
  device_data_ = shared_ptr<Ads1015DeviceData>(new Ads1015DeviceData());

  /*
   * See if we can read the configuration register
   */
  expected<Ads1015Config, int> x_return = readConfigRegister();
  if (x_return.has_value() == false) {
    /*
     * I can't get the configuration register so we asume the device is not there.
     * This causes all other routines to call an ENODEV error
     */
    device_data_.reset();
    // delete device_data_;
    // device_data_ = nullptr;
    return;
  }
  /*
   * Set the default settings in the local configuration
   */
  setDefaultConfiguration();

  return;
}

uint8_t I2cAds1015::deviceAddress() {
  return slave_address_;
}

void I2cAds1015::setMultiplexor(Ads1015MuxType multiplexor) {
  configuration_.fields.mux = multiplexor;

  return;
}

Ads1015MuxType I2cAds1015::getMultiplexor() {
  return configuration_.fields.mux;
}

void I2cAds1015::setProgrammableGainAmplifier(Ads1015PgaType gain) {
  configuration_.fields.pga = gain;

  return;
}

Ads1015PgaType I2cAds1015::getProgrammableGainAmplifier() {
  return configuration_.fields.pga;
}

void I2cAds1015::setMode(Ads1015ModeType mode) {
  configuration_.fields.mode = mode;

  return;
}

Ads1015ModeType I2cAds1015::getMode() {
  return configuration_.fields.mode;
}

void I2cAds1015::setDataRate(Ads1015DrType rate) {
  configuration_.fields.dr = rate;

  return;
}

Ads1015DrType I2cAds1015::getDataRate() {
  return configuration_.fields.dr;
}

void I2cAds1015::setComparatorMode(Ads1015CompModeType comp_mode) {
  configuration_.fields.comp_mode = comp_mode;

  return;
}

Ads1015CompModeType I2cAds1015::getComparatorMode() {
  return configuration_.fields.comp_mode;
}

void I2cAds1015::setComparatorPolarityType(Ads1015CompPolarityType comp_pol) {
  configuration_.fields.comp_pol = comp_pol;

  return;
}

Ads1015CompPolarityType I2cAds1015::getComparatorPolarityType() {
  return configuration_.fields.comp_pol;
}

void I2cAds1015::setComparatorLatching(Ads1015CompLatchType comp_latch) {
  configuration_.fields.comp_latch = comp_latch;

  return;
}

Ads1015CompLatchType I2cAds1015::getComparatorLatching() {
  return configuration_.fields.comp_latch;
}

void I2cAds1015::setComparatorQueue(Ads1015CompQueueType comp_queue) {
  configuration_.fields.comp_queue = comp_queue;

  return;
}

Ads1015CompQueueType I2cAds1015::getComparatorQueue() {
  return configuration_.fields.comp_queue;
}

void I2cAds1015::setDefaultConfiguration() {
  configuration_.fields.os = ADS1015_OS_NO_EFFECT_BUSY;
  configuration_.fields.mux = ADS1015_MUX_AIN0_AIN1;
  configuration_.fields.pga = ADS1015_PGA_2048V;
  configuration_.fields.mode = ADS1015_MODE_SINGLE_SHOT;
  configuration_.fields.dr = ADS1015_DR_1600_SPS;
  configuration_.fields.comp_mode = ADS1015_COMP_MODE_TRADITIONAL;
  configuration_.fields.comp_pol = ADS1015_COMP_POLARITY_ACTIVE_LOW;
  configuration_.fields.comp_latch = ADS1015_COMP_NON_LATCHING;
  configuration_.fields.comp_queue = ADS1015_COMP_QUEUE_DISABLED;

  return;
}

expected<Ads1015Config, int> I2cAds1015::readConfigRegister() {
  int error;
  uint8_t config_data[2];
  uint16_t raw_config;
  Ads1015Config config_reg;

  error = i2cbus_.transferDataFromRegisters(
      slave_address_, kAds1015ConfigRegister, config_data, sizeof(config_data));

  if (error != 0) {
    return unexpected(ENODEV);
  }

  /*
   * Take care of endianness
   */
  config_reg.register_value = config_data[0] << 8 | config_data[1];

  return config_reg;
}

expected<Ads1015Config, int> I2cAds1015::inspectConfigRegister() {
  expected<Ads1015Config, int> result;

  result = readConfigRegister();
  if (result.has_value() == false) {
    return unexpected(EIO);
  }

  return result;
}

expected<bool, int> I2cAds1015::writeConfigRegister(Ads1015Config config) {
  int error;
  uint8_t config_data[2];
  uint16_t config_reg;

  /*
   * We send high byte first
   */
  config_data[0] = (config.register_value & 0xFF00) >> 8;
  config_data[1] = config.register_value & 0x00FF;

  error = i2cbus_.transferDataToRegisters(
      slave_address_, kAds1015ConfigRegister, config_data, sizeof(config_data));

  if (error != 0) {
    return unexpected(ENODEV);
  }

  return true;
}

expected<int16_t, int> I2cAds1015::readConversionRegister() {
  int error;
  uint8_t data[2];
  int16_t conversion_reg;

  error = i2cbus_.transferDataFromRegisters(
      slave_address_, kAds1015ConversionRegister, data, sizeof(data));

  if (error != 0) {
    return unexpected(ENODEV);
  }

  /*
     * The two bytes together are a signed 16 bit value with the first byte
     * being the high order byte. SO, shift the data[0] to the high order
     * byte and or data[1] into the low order byte. Then cast that as a signed
     * 16 bit integer.
     * The low 4 bits are always going to be zero because only the high 12 bits
     * count. But this is the actual raw value returned. We leave it to the higher
     * level to get rid of the low 4 bytes.
     */
  conversion_reg = (static_cast<int16_t>((data[0] << 8) | data[1]));

  return conversion_reg;
}

expected<int16_t, int> I2cAds1015::getReading(Ads1015MuxType mux) {
  int error;
  int16_t data;
  Ads1015Config config;
  expected<Ads1015Config, int> config_result;
  expected<bool, int> bool_result;
  expected<int16_t, int> result;

  /*
     * Set the inputs to measure the Analog Signal of choice
     */
  setMultiplexor(mux);

  /*
     * Use the configuration value, but set the START bit
     */
  config.register_value = (ADS1015_OS_START_COMPLETE << kAds1015OsShift) |
                          configuration_.register_value;

  /*
     * Start the conversion by setting the start bit in the configuration register
     */
  bool_result = writeConfigRegister(config);
  if (bool_result.has_value() == false) {
    return unexpected(EIO);
  }
  /*
     * May need a loop here checking config Os register to see if conversion has completed
     */

  do {
    config_result = readConfigRegister();
    if (config_result.has_value() == false) {
      return unexpected(EIO);
    }
    //
    // Keep looping while it is busy doing an ADC conversion.
    // May want a timeout loop count here for safety purposes.
    //
  } while ((config_result.value().fields.os) != ADS1015_OS_START_COMPLETE);
  /*
     * Now gather the conversion data
     */
  result = readConversionRegister();
  if (result.has_value() == false) {
    return unexpected(EIO);
  }

  /*
     * This is a twelve bit two's complement in 16 bits. The bottom 4 bits
     * are always zero. Since this is signed we divide by 16 rather than
     * shift 4 bits so we mainatin the sign
     */
  data = result.value() / 16;

  /*
     * This returns the signed 12 bit value in signed 16 bits.
     */
  return data;
}

}  // namespace qw::devices
