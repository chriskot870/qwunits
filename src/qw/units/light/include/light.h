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

#ifndef SRC_QW_UNITS_LIGHT_INCLUDE_LIGHT_H_
#define SRC_QW_UNITS_LIGHT_INCLUDE_LIGHT_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>

namespace qw::units {

/*
 * Need to pre-declare these for casting
 */
class Lux;
class Klux;
class FootCandle;

constexpr std::string light_default_format = "{0:.2f}";
constexpr float base_units_in_lux = 100;
constexpr float base_units_in_klux = 1000 * base_units_in_lux;
constexpr float lux_in_foot_candle = 10.764;

class Light {
  friend Lux;
  friend Klux;
  friend FootCandle;

 public:
  Light();

  explicit Light(const int64_t);

  bool operator==(const Light& other) const;

  bool operator!=(const Light& other) const;

  bool operator<(const Light& other) const;

  bool operator>(const Light& other) const;

  bool operator<=(const Light& other) const;

  bool operator>=(const Light& other) const;

  std::strong_ordering operator<=> (const Light& other) const;

  Light& operator=(const Light& other);

  Light& operator+=(const Light& other);

  Light& operator-=(const Light& other);

  const Light operator+(const Light& other) const;

  const Light operator-(const Light& other) const;

  const Light operator/(const int& other) const;

  const Light operator*(const int& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator Lux() const;

  operator Klux() const;

  operator FootCandle() const;

 private:
  int64_t base_value_;
};


}  // namespace qw::units

#endif  // SRC_QW_UNITS_LIGHT_INCLUDE_LIGHT_H_
