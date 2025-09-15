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

#ifndef QW_UNITS_UVI_INCLUDE_UVI_H_
#define QW_UNITS_UVI_INCLUDE_UVI_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>

namespace qw::units {

constexpr int base_units_in_uvi = 100;
constexpr std::string uvi_default_format = "{0:.1f}";

class Uvi {
 public:
  Uvi();

  explicit Uvi(float index);

  Uvi(float index, std::string fmt_value);

  bool operator==(const Uvi& other) const;

  bool operator!=(const Uvi& other) const;

  bool operator<(const Uvi& other) const;

  bool operator>(const Uvi& other) const;

  bool operator<=(const Uvi& other) const;

  bool operator>=(const Uvi& other) const;

  std::strong_ordering operator<=> (const Uvi& other) const;

  Uvi& operator=(const Uvi& other);

  Uvi& operator+=(const Uvi& other);

  Uvi& operator-=(const Uvi& other);

  /*
   * I don't know if arithmetic operators make sense
   */
  const Uvi operator+(const Uvi& other) const;

  const Uvi operator-(const Uvi& other) const;

  float value();

  std::string toString();

  std::string toString(std::string format);

  void setFormat(std::string fmt_value);

 private:
  int base_value_;

  std::string fmt_value_ = uvi_default_format;
};

}  // namespace qw::units

#endif  // QW_UNITS_UVI_INCLUDE_UVI_H_
