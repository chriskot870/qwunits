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

#include "qw/devices/include/anemometer_adafruit.h"

#include <chrono>
#include <expected>  // Cpplint thinks this is c system header // NOLINT

#include "qw/devices/i2c/include/ads1015.h"
#include "qw/units/speed/include/meters_per_second.h"
#include "qw/units/include/unit_measurement.h"

using qw::devices::Ads1015MuxType;
using qw::devices::I2cAds1015;
using qw::devices::kAds1015CountPerVolts;
using qw::units::MetersPerSecond;
using qw::units::Speed;
using qw::units::UnitMeasurement;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::expected;
using std::unexpected;
using std::chrono::system_clock;

namespace qw::devices {

AnomometerAdafruit::AnomometerAdafruit(I2cAds1015 adc, Ads1015MuxType mux)
    : adc_(adc), mux_(mux) {}

expected<UnitMeasurement<Speed>, int> AnomometerAdafruit::getMeasurement() {
  expected<int16_t, int> reading = adc_.getReading(mux_);
  if (reading.has_value() == false) {
    return unexpected(EIO);
  }

  /*pee
     * Convert the reading to a speed
     */

  float mps = 0.0;
  float count = reading.value();

  /*
      * There should be a .4 volt if the anomometer is connected.
      * If it is far below that then there is no anomometer device.
      */

  if (count < kAnomometerAdafruitNotConnected) {
    return unexpected(ENODEV);
  }

  if (count > kAnomometerAdafruitMinValue) {
    mps =
        (count - kAnomometerAdafruitMinValue) * kAnomometerAdafruitMpsPerCount;
  }

  MetersPerSecond mps_speed(mps);
  MetersPerSecond accuracy(kAnomometerAdafruitAccuracy);
  time_point<system_clock> current_time = system_clock::now();

  UnitMeasurement<Speed> measured_speed(mps_speed, accuracy, current_time);

  return measured_speed;
}

}  // namespace qw::devices
