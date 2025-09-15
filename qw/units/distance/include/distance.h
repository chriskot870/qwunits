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

#ifndef QW_UNITS_DISTANCE_INCLUDE_DISTANCE_H_
#define QW_UNITS_DISTANCE_INCLUDE_DISTANCE_H_

#include <fmt/format.h>
#include <math.h>
#include <compare>
#include <string>

namespace qw::units {

/*
 * Need to pre-declare these for casting
 */
class Millimeter;
class Inches;

constexpr std::string distance_default_format = "{0:.2f}";
constexpr float base_units_in_mm = 100;
constexpr float mm_in_inch = 25.4;

class Distance {
  friend Millimeter;
  friend Inches;

 public:
  Distance();

  explicit Distance(const int64_t);

  bool operator==(const Distance& other) const;

  bool operator!=(const Distance& other) const;

  bool operator<(const Distance& other) const;

  bool operator>(const Distance& other) const;

  bool operator<=(const Distance& other) const;

  bool operator>=(const Distance& other) const;

  std::strong_ordering operator<=> (const Distance& other) const;

  Distance& operator=(const Distance& other);

  Distance& operator+=(const Distance& other);

  Distance& operator-=(const Distance& other);

  const Distance operator+(const Distance& other) const;

  const Distance operator-(const Distance& other) const;

  const Distance operator/(const int& other) const;

  const Distance operator*(const int& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator Millimeter() const;

  operator Inches() const;

 private:
  int64_t base_value_;
};


}  // namespace qw::units

#endif  // QW_UNITS_DISTANCE_INCLUDE_DISTANCE_H_
