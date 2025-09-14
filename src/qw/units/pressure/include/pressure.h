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

#ifndef SRC_QW_UNITS_PRESSURE_INCLUDE_PRESSURE_H_
#define SRC_QW_UNITS_PRESSURE_INCLUDE_PRESSURE_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>

namespace qw::units {

/*
 * Need to pre-declare these for casting
 */
class Millibar;
class InchesMercury;

 /*
   * Our pressure base is millicelsius so we want
   * to apply the pressur_base_conversion_factor
   * to millibar values.
   */
constexpr int pressure_base_conversion_factor = 1000;
constexpr float inHg_sea_level = 29.92;  // inches mercury at sea level
constexpr float mb_sea_level = 1013.25;  // millibars at sea level
constexpr std::string pressure_default_format = "{0:.2f}";

class Pressure {
  friend Millibar;
  friend InchesMercury;

 public:
  Pressure();

  explicit Pressure(const int64_t);

  bool operator==(const Pressure& other) const;

  bool operator!=(const Pressure& other) const;

  bool operator<(const Pressure& other) const;

  bool operator>(const Pressure& other) const;

  bool operator<=(const Pressure& other) const;

  bool operator>=(const Pressure& other) const;

  std::strong_ordering operator<=> (const Pressure& other) const;

  Pressure& operator=(const Pressure& other);

  Pressure& operator+=(const Pressure& other);

  Pressure& operator-=(const Pressure& other);

  const Pressure operator+(const Pressure& other) const;

  const Pressure operator-(const Pressure& other) const;

  const Pressure operator/(const int& other) const;

  const Pressure operator*(const int& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator Millibar() const;

  operator InchesMercury() const;

 private:
  int64_t base_value_;
};

}  // namespace qw::units

#endif  // SRC_QW_UNITS_PRESSURE_INCLUDE_PRESSURE_H_
