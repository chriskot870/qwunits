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
 * This contains the ADS1015 information. See data sheet
 * https://cdn-shop.adafruit.com/datasheets/ads1015.pdf
 *
 */

#ifndef SRC_LIB_QW_DEVICES_I2C_INCLUDE_ADS1015_H_
#define SRC_LIB_QW_DEVICES_I2C_INCLUDE_ADS1015_H_

#include <errno.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <expected>  // Lint incorrectly counts this as a C system header // NOLINT
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "qw/devices/i2c/include/i2cbus.h"

namespace qw::devices {

/*
 * The I2C address is determined by the ADDR PIN.
 * The address is selected by connecting the ADDR PIn To
 * one of the following other pins.
 * 
 * Ground  0x48
 * VDD     0x49
 * SDA     0x4A
 * SCL     0x4B
 */
constexpr uint8_t kAds1015I2cPrimaryAddress = 0x48;
constexpr uint8_t kAds1015I2cSecondaryAddress = 0x49;
constexpr uint8_t kAds1015I2cThirdAddress = 0x4A;
constexpr uint8_t kAds1015I2cFourthAddress = 0x4B;

const std::vector<uint8_t> ads1015_slave_address_options = {
    kAds1015I2cPrimaryAddress, kAds1015I2cSecondaryAddress,
    kAds1015I2cThirdAddress, kAds1015I2cFourthAddress};

/*
 * There are 4 registers accesible by the I2C bus
 *
 * Conversion Register 0
 * Config Register  1
 * Lo_thresh register 2
 * Hi thresh register
 */
constexpr uint8_t kAds1015ConversionRegister = 0;
constexpr uint8_t kAds1015ConfigRegister = 1;
constexpr uint8_t kAds1015LowThreshold = 2;
constexpr uint8_t kAds1015highThreshold = 3;

/*
 * In single-ended mode the maximum value is 2**11.
 * In single-ended mode the range is 0 - 3.3 Volts.
 * Therefore there are kAds1015MaxRange/kAds1015MaxVoltage counts per voltage
 */
constexpr uint16_t kAds1015MaxRange = 2048;  // Single Ended Max value
constexpr float kAds1015MaxVoltage = 3.3;    // Highest Value of Voltage
constexpr float kAds1015CountPerVolts =
    (kAds1015MaxRange /
     kAds1015MaxVoltage);  // Each Voltage is this many counts

/*
 * Config register values
 */
/*
 * OS bit
 */
constexpr uint8_t kAds1015OsMask = 1;
constexpr uint8_t kAds1015OsShift = 15;
typedef enum {
  ADS1015_OS_NO_EFFECT_BUSY,  // 0 On write no effect, on read performing conversion
  ADS1015_OS_START_COMPLETE  // 1 On write, Start conversion, on read not converting
} Ads1015OsType;

/*
 * Mux bits
 * The different MUX values. The order is important
 */
constexpr uint8_t kAds1015MuxMask = 0x7;
constexpr uint8_t kAds1015MuxShift = 12;
constexpr uint8_t kAds1015MuxMax = 8;
typedef enum {
  ADS1015_MUX_AIN0_AIN1,  // 0 Differential AIN0 + AIN1 - (Default)
  ADS1015_MUX_AIN0_AIN3,  // 1 Differential AIN0 + AIN3 -
  ADS1015_MUX_AIN1_AIN3,  // 2 Differential AIN1 + Ain3 -
  ADS1015_MUX_AIN2_AIN3,  // 3 Differential AIN2 + AIN3 -
  ADS1015_MUX_AIN0_GND,   // 4 Single Ended AIN0 + GND -
  ADS1015_MUX_AIN1_GND,   // 5 Single Ended AIN1 + GND -
  ADS1015_MUX_AIN2_GND,   // 6 Single Ended AIN2 + GND -
  ADS1015_MUX_AIN3_GND,   // 7 Single Ended AIN3 + GND -
} Ads1015MuxType;

/*
 * PGA bits
 */
constexpr uint8_t kAds1015PgaMask = 0x7;
constexpr uint8_t kAds1015PgaShift = 9;
typedef enum {
  ADS1015_PGA_6144V,   // 000 : FS = ±6.144V
  ADS1015_PGA_4096V,   // 001 : FS = ±4.096V
  ADS1015_PGA_2048V,   // 010 : FS = ±2.048V (Default)
  ADS1015_PGA_1024V,   // 011 : FS = ±1.024V
  ADS1015_PGA_0512V,   // 100 : FS = ±0.512V
  ADS1015_PGA_0256V1,  // 101 : FS = ±0.256V
  ADS1015_PGA_0256V2,  // 110 : FS = ±0.256V
  ADS1015_PGA_0256V3,  // 111 : FS = ±0.256V
} Ads1015PgaType;

/*
 * Mode bits
 */
constexpr uint8_t kAds1015ModeMask = 1;
constexpr uint8_t kAds1015ModeShift = 8;
typedef enum {
  ADS1015_MODE_CONTINUOUS,  // 0 continuous mode
  ADS1015_MODE_SINGLE_SHOT  // 1 Power-down single-shot (Default)
} Ads1015ModeType;

/*
  * Data Rate bits
  */
constexpr uint8_t kAds1015DrMask = 0x7;
constexpr uint8_t kAds1015DrShift = 5;
typedef enum {
  ADS1015_DR_128_SPS,     // 000 : 128SPS
  ADS1015_DR_250_SPS,     // 001 : 250SPS
  ADS1015_DR_490_SPS,     // 010 : 490SPS
  ADS1015_DR_920_SPS,     // 011 : 920SPS
  ADS1015_DR_1600_SPS,    // 100 : 1600SPS (Default)
  ADS1015_DR_2400_SPS,    // 101 : 2400SPS
  ADS1015_DR_3300_SPS_1,  // 110 : 3300SPS
  ADS1015_DR_3300_SPS_2   // 111 : 3300SPS
} Ads1015DrType;

/*
 * Comparator Mode bit
 */
constexpr uint8_t kAds1015CompModeMask = 1;
constexpr uint8_t kAds1015CompModeShift = 4;
typedef enum {
  ADS1015_COMP_MODE_TRADITIONAL,  // 0 : Traditional comparator with hysteresis (Default)
  ADS1015_COMP_MODE_WINDOW  // 1 : Window comparator
} Ads1015CompModeType;

/*
 * Comparator Polarity bit
 */
constexpr uint8_t kAds1015CompPolarityMask = 1;
constexpr uint8_t kAds1015CompPolarityShift = 3;
typedef enum {
  ADS1015_COMP_POLARITY_ACTIVE_LOW,  // 0 : Active low (Default)
  ADS1015_COMP_POLARITY_ACTIVE_HIGH  // 1 : Active high
} Ads1015CompPolarityType;

/*
 * Latching Comparator bit
 */
constexpr uint8_t kAds1015CompLatchMask = 1;
constexpr uint8_t kAds1015CompLatchShift = 2;
typedef enum {
  ADS1015_COMP_NON_LATCHING,  // 0 : Non-latching comparator (Default)
  ADS1015_COMP_LATCHING       // 1 : Latching comparator
} Ads1015CompLatchType;

/*
 * Comparator Queue bits
 */
constexpr uint8_t kAds1015CompQueueMask = 2;
constexpr uint8_t kAds1015CompQueueShift = 0;
typedef enum {
  ADS1015_COMP_QUEUE_ASSERT_AFTER_ONE,   // 00 : Assert after one conversion
  ADS1015_COMP_QUEUE_ASSERT_AFTER_TWO,   // 01 : Assert after two conversions
  ADS1015_COMP_QUEUE_ASSERT_AFTER_FOUR,  // 10 : Assert after four conversions
  ADS1015_COMP_QUEUE_DISABLED            // 1 : Disable comparator (Default)
} Ads1015CompQueueType;

union Ads1015Config {
  uint16_t register_value;  // To address the register as one uint16_t
  struct {                  // To address each filed of the register
    Ads1015CompQueueType comp_queue : 2;   // Bits[1:0]
    Ads1015CompLatchType comp_latch : 1;   // Bit[2]
    Ads1015CompPolarityType comp_pol : 1;  // Bit[3]
    Ads1015CompModeType comp_mode : 1;     // Bit[4]
    Ads1015DrType dr : 3;                  // Bis[7:5]
    Ads1015ModeType mode : 1;              // Bit[8]
    Ads1015PgaType pga : 3;                // Bits[11:9]
    Ads1015MuxType mux : 3;                // Bits[14:12]
    Ads1015OsType os : 1;                  // Bit[15]
  } fields;
};

struct Ads1015DataPoint {
  int16_t measurement;
  std::chrono::time_point<std::chrono::system_clock> system_time;
  std::chrono::time_point<std::chrono::steady_clock> steady_time;
};

class Ads1015DeviceLocation {
 public:
  std::string bus_name_;
  uint8_t slave_address_;

  /*
   * We need the == comparison to support the contain function for this class
   * to be used as a key in a map.
   */
  bool operator==(const Ads1015DeviceLocation& data) const {
    if ((bus_name_ == data.bus_name_) &&
        (slave_address_ == data.slave_address_)) {
      return true;
    }
    return false;
  }

  /*
   * If you have == you should also have !=
   */
  bool operator!=(const Ads1015DeviceLocation& data) const {
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
  bool operator<(const Ads1015DeviceLocation& data) const {
    if (bus_name_.compare(data.bus_name_) < 0)
      return true;
    if (bus_name_.compare(data.bus_name_) > 0)
      return false;
    if (slave_address_ < data.slave_address_)
      return true;
    return false;
  }
};

class Ads1015DeviceData {
 public:
  std::recursive_mutex lock_ = {};
  uint64_t read_total_ = 0;
  std::atomic_bool initialized = false;

  uint64_t mux_reads[kAds1015MuxMax] = {0, 0, 0, 0, 0, 0, 0, 0};
  Ads1015DataPoint data_[kAds1015MuxMax];
};

class I2cAds1015 {
 public:
  I2cAds1015(I2cBus i2cbus, uint8_t slave_address);

  uint8_t deviceAddress();

  void setMultiplexor(Ads1015MuxType multiplexor);

  Ads1015MuxType getMultiplexor();

  void setProgrammableGainAmplifier(Ads1015PgaType gain);

  Ads1015PgaType getProgrammableGainAmplifier();

  void setMode(Ads1015ModeType mode);

  Ads1015ModeType getMode();

  void setDataRate(Ads1015DrType rate);

  Ads1015DrType getDataRate();

  void setComparatorMode(Ads1015CompModeType comp_mode);

  Ads1015CompModeType getComparatorMode();

  void setComparatorPolarityType(Ads1015CompPolarityType comp_pol);

  Ads1015CompPolarityType getComparatorPolarityType();

  void setComparatorLatching(Ads1015CompLatchType comp_latch);

  Ads1015CompLatchType getComparatorLatching();

  void setComparatorQueue(Ads1015CompQueueType comp_queue);

  Ads1015CompQueueType getComparatorQueue();

  void setDefaultConfiguration();

  std::expected<int16_t, int> getReading(Ads1015MuxType mux);

  std::expected<Ads1015Config, int> inspectConfigRegister();

 private:
  std::expected<Ads1015Config, int> readConfigRegister();

  std::expected<bool, int> writeConfigRegister(Ads1015Config config);

  std::expected<int16_t, int> readConversionRegister();

  static std::mutex ads1015_devices_lock;
  static std::map<Ads1015DeviceLocation, std::shared_ptr<Ads1015DeviceData>>
      ads1015_devices;

  Ads1015DeviceLocation device_;
  std::shared_ptr<Ads1015DeviceData> device_data_ = nullptr;

  // The slave address of the device. The sht45 can be either 0x44 or 0x45.
  uint8_t slave_address_;

  // Which I2c bus is the device on
  I2cBus i2cbus_;

  /*
   * Set the configuration to the power on default values.
   * Note that the actual register may be different if a
   * I2cAds1015 object is created sometime after power on
   * and the configuration has previously been changed
   * from the power on settings.
   */
  Ads1015Config configuration_;
};

}  // namespace qw::devices
#endif  // SRC_LIB_QW_DEVICES_I2C_INCLUDE_ADS1015_H_
