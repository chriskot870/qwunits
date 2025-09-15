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
#ifndef SRC_LIB_QW_DEVICES_INCLUDE_ECOWITT_LN90LP_H_
#define SRC_LIB_QW_DEVICES_INCLUDE_ECOWITT_LN90LP_H_

#include <expected>

#include <array>
#include <string>
#include <string_view>

#include "modbus/modbus-rtu.h"
#include "modbus/modbus.h"

#include "qw/units/direction/include/degrees.h"
#include "qw/units/humidity/include/relative_humidity.h"
#include "qw/units/pressure/include/millibar.h"
#include "qw/units/speed/include/meters_per_second.h"
#include "qw/units/temperature/include/celsius.h"
#include "qw/units/uvi/include/uvi.h"
#include "qw/units/light/include/klux.h"
#include "qw/units/light/include/light.h"
#include "qw/units/distance/include/millimeter.h"
#include "qw/units/distance/include/distance.h"
#include "qw/units/include/unit_measurement.h"

namespace qw::devices {

constexpr std::string_view kEwLn90lpRtuDevice = "/dev/ttyS0";
constexpr uint kEwLn90lpRtuDeviceDefaultBaudRate = 9600;
constexpr char kEwLn90lpRtuDeviceParity = 'N';
constexpr uint8_t kEwLn90lpRtuDeviceDataBits = 8;
constexpr uint8_t kEwLn90lpRtuDeviceStopBits = 1;
constexpr uint8_t kEwLn90lpRtuDefaultSlaveAddress = 0x90;
// I assume this is a fixed value
// I came up with it by reading the register.
// I hope that it identifies the model, but I am not sure
constexpr uint16_t kEwLn90lpRtuDeviceId = 0x90;

// Special command code
constexpr uint8_t kEwLn90lpSpecialCommandCode[3] = {0xFD, 0xFD, 0xFD};

struct WsEwLn90lpSpecialDataInquiry {
  uint8_t bps;
  uint8_t device_address;
};

struct WsEwLn90lpSpecialDataResponse {
  uint32_t baud_rate;
  uint8_t device_address;
};

struct WsEwLn90lpSpecialFrame {
  uint8_t prefix[3];
  struct WsEwLn90lpSpecialDataInquiry data;
};

constexpr uint8_t kEwLn90lpErrIllegalFunction = 1;
constexpr uint8_t kEwLn90lpErrIllegalAddress = 2;
constexpr uint8_t kEwLn90lpErrIllegalData = 3;
constexpr uint8_t kEwLn90lpErrCrcFail = 8;

// One more than the offsets are the values to exchange with the device
// to get the corresponding baud rate.
// If when asked for the baud rate it returns a value of 2 then that
// means the value is at 2-1 offset. That would be 9600. To set the
// baud rate to 115200 you would send 3 + 1.
constexpr uint8_t kEwLn90lpBaudRateCount = 4;
constexpr std::array<uint32_t, 4> kEwLn90lpBaudRates({4800, 9600, 19200,
                                                        115200});

constexpr uint16_t kEwLn90lpAddressMin = 1;
constexpr uint16_t kEwLn90lpAddressMax = 252;

constexpr uint8_t kEwLn90lpRtuRead = 0x03;
constexpr uint8_t kEwLn90lpRtuWrite = 0x06;

// Temperature measuring range
const qw::units::Celsius kEwLn90lpTemperatureRange[2] = {
    qw::units::Celsius(-40), qw::units::Celsius(60)};
// +/-1 degree Celsius accuracy
const qw::units::Celsius kEwLn90lpTemperatureAccuracy(1);
// .1 degree Celsius resolution
const qw::units::Celsius kEwLn90lpTemperatureResolution(.1);

// Relative Humidity measuring range
const qw::units::RelativeHumidity kEwLn90lpRelativeHumidityRange[2] = {
    qw::units::RelativeHumidity(1), qw::units::RelativeHumidity(99)};
// +/-5 % Relative Humidity accuracy
const qw::units::RelativeHumidity kEwLn90lpRelativeHumidityAccuracy(5);
// 1% Relative Humidity resolution
const qw::units::RelativeHumidity kEwLn90lpRelativeHumidityResolution(1);

// Pressure measuring range
const qw::units::Millibar kEwLn90lpPressureRange[2] = {
    qw::units::Millibar(300), qw::units::Millibar(1100)};
// +/-5 mbar accuracy
const qw::units::Millibar kEwLn90lpPressureAccuracy(5);
// .1 mbar resolution
const qw::units::Millibar kEwLn90lpPressureResolution(.1);

// Wind speed measuring range
const qw::units::MetersPerSecond kEwLn90lpWindSpeedRange[2] = {
    qw::units::MetersPerSecond(0), qw::units::MetersPerSecond(40)};
// wind speed accuracy is not a constant. It is implemented in readWindSpeed().
// wind speed reolution
const qw::units::MetersPerSecond kEwLn90lpWindSpeedResolution(.1);

// Wind direction measuring range
const qw::units::Degrees kEwLn90lpWindDirectionRange[2] = {
    qw::units::Degrees(0), qw::units::Degrees(359)};
// +/- 15 degrees accuracy
const qw::units::Degrees kEwLn90lpWindDirectionAccuracy(15);
// 1 degree resolution
const qw::units::Degrees kEwLn90lpWindDirectionResolution(1);

// UVI measuring range
const qw::units::Uvi kEwLn90lpUviRange[2] = {
    qw::units::Uvi(0), qw::units::Uvi(15)};
// +/- 2 UVI accuracy
const qw::units::Uvi kEwLn90lpUviAccuracy(15);
// 1 UVI resolution
const qw::units::Uvi kEwLn90lpUviResolution(1);

// Light measuring range
const qw::units::Klux kEwLn90lpLightRange[2] = {
    qw::units::Klux(0), qw::units::Klux(200)};
// Accuracy is +/- 25% for now used a fixed value
const qw::units::Lux kEwLn90lpLightAccuracy(15);
// 1 UVI resolution
const qw::units::Lux kEwLn90lpLightResolution(.1);

// Rainfall measuring range
const qw::units::Millimeter kEwLn90lpRainFallRange[2] = {
    qw::units::Millimeter(0), qw::units::Millimeter(6553.5)};
// Rainfall accuracy is more complicated
// Rainfall measuring resolution
const qw::units::Millimeter kEwLn90lpRainFallResolution(.1);

constexpr uint16_t kEwLn90lpRtuRegisterDeviceName = 0x0160;
constexpr uint16_t kEwLn90lpRtuRegisterDataRate = 0x0161;
constexpr uint16_t kEwLn90lpRtuRegisterDeviceAddress = 0x0162;
constexpr uint16_t kEwLn90lpRtuRegisterDeviceIdMsb = 0x0163;
constexpr uint16_t kEwLn90lpRtuRegisterDeviceIdLsb = 0x0164;
constexpr uint16_t kEwLn90lpRtuRegisterLight = 0x0165;
constexpr uint16_t kEwLn90lpRtuRegisterUvi = 0x0166;
constexpr uint16_t kEwLn90lpRtuRegisterTemperature = 0x0167;
constexpr uint16_t kEwLn90lpRtuRegisterHumidity = 0x0168;
constexpr uint16_t kEwLn90lpRtuRegisterWindSpeed = 0x0169;
constexpr uint16_t kEwLn90lpRtuRegisterGustSpeed = 0x016A;
constexpr uint16_t kEwLn90lpRtuRegisterWindDirection = 0x016B;
constexpr uint16_t kEwLn90lpRtuRegisterRainfall = 0x016C;
constexpr uint16_t kEwLn90lpRtuRegisterAbsPressure = 0x016D;
constexpr uint16_t kEwLn90lpRtuRegisterRainCounter = 0x016E;

constexpr int kEwLn90lpRtuAllDataCount =
    kEwLn90lpRtuRegisterAbsPressure - kEwLn90lpRtuRegisterLight + 1;

/*
 * These command initiate a measurement
 */
constexpr uint16_t kEwLn90lpRtuMeasuringLight = 0x9C92;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringLightDelay(113);
constexpr uint16_t kEwLn90lpRtuMeasuringUvi = 0x9C93;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringUviDelay(113);
constexpr uint16_t kEwLn90lpRtuMeasuringTemperature = 0x9C94;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringTemperatureDelay(31);
constexpr uint16_t kEwLn90lpRtuMeasuringHumidity = 0x9C95;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringHumidityDelay(31);
constexpr uint16_t kEwLn90lpRtuMeasuringWindSpeed = 0x9C96;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringWindSpeedDelay(31);
constexpr uint16_t kEwLn90lpRtuMeasuringGustSpeed = 0x9C97;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringGustSpeedDelay(31);
constexpr uint16_t kEwLn90lpRtuMeasuringWindDirection = 0x9C98;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringWindDirectionDelay(
    31);
// constexpr uint16_t kEwLn90lpRtuReserved = 0x9C99;
constexpr uint16_t kEwLn90lpRtuMeasuringAbsPressure = 0x9C9A;
constexpr std::chrono::milliseconds kEwLn90lpRtuMeasuringWindPressureDelay(
    136);

struct WsEwLn90lpRtuInputData {
  uint16_t light;
  uint16_t uvi;
  uint16_t temperature;
  uint16_t humidity;
  uint16_t wind_speed;
  uint16_t gust_speed;
  uint16_t wind_direction;
  uint16_t rainfall;
};

// The device updates it's values every 8.8 seconds.
constexpr std::chrono::milliseconds kEwLn90lpDataRefreshInterval(8800);
// Wind speed is faster every 2.2 seconds
constexpr std::chrono::milliseconds kEwLn90lpWindSpeedDataRefreshInterval(
    2200);

class EcowittLn90lp {
 public:
  explicit EcowittLn90lp(std::string_view device_name);

  bool initialize(uint32_t baud = 0, uint8_t device_addr = 0);

  /*
   * These three routines make a thermometer
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Temperature>, int> getTemperature();

  std::chrono::milliseconds getTemperatureValidInterval();

  void setTemperatureValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a hygrometer
   */
  std::expected<qw::units::UnitMeasurement<qw::units::RelativeHumidity>, int>
  getRelativeHumidity();

  std::chrono::milliseconds getRelativeHumidityValidInterval();

  void setRelativeHumidityValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a barometer
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Pressure>, int> getPressure();

  std::chrono::milliseconds getPressureValidInterval();

  void setPressureValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make an anemometer
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Speed>, int> getWindspeed();

  std::chrono::milliseconds getWindSpeedValidInterval();

  void setWindSpeedValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a wind vane
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Direction>, int> getWindDirection();

  std::chrono::milliseconds getWindDirectionValidInterval();

  void setWindDirectionValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a UVI unit
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Uvi>, int> getUvi();

  std::chrono::milliseconds getUviValidInterval();

  void setUviValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a Light meter
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Light>, int> getLight();

  std::chrono::milliseconds getLightValidInterval();

  void setLightValidInterval(std::chrono::milliseconds interval);

  /*
   * These three make a Rain Gauge
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Distance>, int> getRainFall();

  std::chrono::milliseconds getRainFallValidInterval();

  void setRainFallValidInterval(std::chrono::milliseconds interval);

  /*
   * These are the unbuffered read routines called by the get routines above.
   * These actually go fetch the data from the device.
   */
  std::expected<qw::units::UnitMeasurement<qw::units::Temperature>, int> readTemperatureData();

  std::expected<qw::units::UnitMeasurement<qw::units::RelativeHumidity>, int>
  readRelativeHumidityData();

  std::expected<qw::units::UnitMeasurement<qw::units::Pressure>, int> readPressureData();

  std::expected<qw::units::UnitMeasurement<qw::units::Speed>, int> readWindSpeedData();

  std::expected<qw::units::UnitMeasurement<qw::units::Direction>, int> readWindDirectionData();

  std::expected<qw::units::UnitMeasurement<qw::units::Uvi>, int> readUviData();

  std::expected<qw::units::UnitMeasurement<qw::units::Light>, int> readLightData();

  std::expected<qw::units::UnitMeasurement<qw::units::Distance>, int> readRainFallData();

  /*
   * MIicellaneous control functions
   */
  uint32_t getLocalBaudRate();

  std::expected<uint32_t, int> getDeviceBaudRate();

  int setLocalBaudRate(uint32_t baud_rate);

  int setDeviceBaudRate(uint32_t speed);

  uint8_t getSlaveAddress();

  std::expected<uint8_t, int> getDeviceAddress();

  int setSlaveAddress(uint8_t address);

  int setDeviceAddress(uint16_t device_address);

  std::expected<uint16_t, int> getDeviceId();

  std::expected<struct WsEwLn90lpSpecialDataResponse, int> specialCommand(
      uint32_t baud_rate, uint8_t address);

  bool findAndMatchDevice();

  ~EcowittLn90lp();

 private:
  std::string_view device_name_;
  uint baud_rate_;      // Initially 9600 can be changed
  char parity_;         // Fixed at N
  uint8_t data_bits_;   // Fixed at 8
  uint8_t stop_bits_;   // Fixed at 1
  uint8_t slave_addr_;  // Initially 0x90 can be changed on device
  WsEwLn90lpRtuInputData buffer_;

  qw::units::UnitMeasurement<qw::units::Temperature> last_temperature_;
  std::chrono::milliseconds temperature_valid_interval_ =
      kEwLn90lpDataRefreshInterval;

  qw::units::UnitMeasurement<qw::units::RelativeHumidity> last_rh_;
  std::chrono::milliseconds rh_valid_interval_ = kEwLn90lpDataRefreshInterval;

  qw::units::UnitMeasurement<qw::units::Pressure> last_pressure_;
  std::chrono::milliseconds pressure_valid_interval_ =
      kEwLn90lpDataRefreshInterval;

  qw::units::UnitMeasurement<qw::units::Speed> last_wind_speed_;
  std::chrono::milliseconds wind_speed_valid_interval_ =
      kEwLn90lpDataRefreshInterval;

  qw::units::UnitMeasurement<qw::units::Direction> last_wind_direction_;
  std::chrono::milliseconds wind_direction_valid_interval_ =
      kEwLn90lpDataRefreshInterval;

  qw::units::UnitMeasurement<qw::units::Uvi> last_uvi_;
  std::chrono::milliseconds uvi_valid_interval_ =
      kEwLn90lpDataRefreshInterval;

  qw::units::UnitMeasurement<qw::units::Light> last_light_;
  std::chrono::milliseconds light_valid_interval_ =
      kEwLn90lpDataRefreshInterval;

  qw::units::UnitMeasurement<qw::units::Distance> last_rain_fall_;
  std::chrono::milliseconds rain_fall_valid_interval_ =
      kEwLn90lpDataRefreshInterval;

  int downloadModBusData(uint16_t addr, int count, uint16_t* buffer);

  int uploadModBusData(uint16_t addr, int count, uint16_t* buffer);

  std::expected<qw::units::Temperature, int> convertRawTemperatureData(
      uint16_t raw_data);

  std::expected<qw::units::RelativeHumidity, int>
  convertRawRelativeHumidityData(uint16_t raw_data);

  std::expected<qw::units::Pressure, int> convertRawPressureData(
      uint16_t raw_data);

  std::expected<qw::units::Speed, int> convertRawWindSpeedData(
      uint16_t raw_data);

  std::expected<qw::units::Uvi, int> convertRawUviData(uint16_t raw_data);

  std::expected<qw::units::Light, int> convertRawLightData(uint16_t raw_data);

  std::expected<qw::units::Distance, int> convertRawRainFallData(uint16_t raw_data);
};

}  // namespace qw::devices

#endif  // SRC_LIB_QW_DEVICES_INCLUDE_ECOWITT_LN90LP_H_
