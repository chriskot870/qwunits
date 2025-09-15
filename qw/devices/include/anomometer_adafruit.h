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
 * This the driver for the Ada Fruit Wind Speed Sensor.
 * See https://www.adafruit.com/product/1733. It uses
 * Analog output to identigfy the wind speed. It provides
 * a .4 Volt value when it is not spinning. The data
 * sheet on Ada Fruit says it provides 2.0V when wind speed
 * is 72 mph. I am assume that is not linear so determing
 * how to map the speed to voltage will have to be trial
 * and error.
 */

#ifndef SRC_LIB_QW_DEVICES_INCLUDE_ANOMOMETER_ADAFRUIT_H_
#define SRC_LIB_QW_DEVICES_INCLUDE_ANOMOMETER_ADAFRUIT_H_

#include <expected>  // cpplint is wrong about this being c system header // NOLINT

#include "qw/devices/i2c/include/ads1015.h"
#include "qw/units/speed/include/speed.h"
#include "qw/units/include/unit_measurement.h"

namespace qw::devices {

/*
  * The information on the Adafruit anomometer is written in a language
  * I don't know. I assume Chinese. But the digits are in arabic numerals.
  * One section has
  * DC 7-24VDC  - I assume this is the power input range
  * Another section has
  * +/- (0.3 + 0. 03) m/s  - I assume this is an accuracy of +/-.33 meters/second
  * Another section has
  * 0.4-2V  - I assume this is the output voltage range
  * Another section has
  * 0-32.4 m/s  - I assume this is the speed range that correspondes to the output voltage range
  */

constexpr float kAnonometerAdafruitMinVolts =
    .4;  // Minimum voltage, corresponds to 0 m/s
constexpr float kAnomometerAdafruitConnectedVolts =
    .3;  // If below this assume not plugged in
constexpr float kAnonometerAdafruitMaxVoltage =
    2.0;  // The maximum output voltage, corresponds to 32.4 m/s
constexpr float kAnomometerAdafruitMaxMps =
    32.4;  // Maximun m/s corresponds to 2.0 volts
constexpr float kAnomometerAdafruitAccuracy =
    .33;  // Accuracy .33 m/s from label on device
constexpr float kAnomometerAdafruitMpsPerCount =
    kAnomometerAdafruitMaxMps /
    (qw::devices::kAds1015CountPerVolts *
     (kAnonometerAdafruitMaxVoltage - kAnonometerAdafruitMinVolts));
constexpr float kAnomometerAdafruitMinValue =
    (qw::devices::kAds1015CountPerVolts * kAnonometerAdafruitMinVolts);
constexpr float kAnomometerAdafruitNotConnected =
    (qw::devices::kAds1015CountPerVolts * kAnomometerAdafruitConnectedVolts);

/*
 * volts = (volts/count) * count + b
 */

class AnomometerAdafruit {
 public:
  AnomometerAdafruit(qw::devices::I2cAds1015 adc,
                     qw::devices::Ads1015MuxType mux);

  std::expected<qw::units::UnitMeasurement<qw::units::Speed>, int> getMeasurement();

 private:
  qw::devices::I2cAds1015 adc_;

  qw::devices::Ads1015MuxType mux_;
};

}  // namespace qw::devices

#endif  // SRC_LIB_QW_DEVICES_INCLUDE_ANOMOMETER_ADAFRUIT_H_
