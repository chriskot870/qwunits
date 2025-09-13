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

#ifndef SRC_DIRECTION_INCLUDE_DIRECTION_H_
#define SRC_DIRECTION_INCLUDE_DIRECTION_H_

#include <compare>
#include <string>
#include <string_view>

namespace qw::units {

/*
 * Need to pre-declare these
 */
class Degrees;
class Radians;
class CompassHeading;
/*
 * Our direction base is .1 degree
 * There are  degrees_base_conversion_factor base units in a degree
 * There are 57.2958 degrees in 1 radian.
 * For Compass heading  we do up to 3 letters which is 16 different sections
 */
constexpr int degrees_base_conversion_factor = 10;
constexpr int radians_base_conversion_factor = (57.2958 * degrees_base_conversion_factor);
constexpr int compass_heading_points = 16;
constexpr float compass_heading_range = 360/compass_heading_points;
const std::string_view direction_default_format = "{0:.2f}";

class Direction {
  friend Degrees;
  friend Radians;
  friend CompassHeading;

 public:
  Direction();

  explicit Direction(const int64_t);

  bool operator==(const Direction& other) const;

  bool operator!=(const Direction& other) const;

  bool operator<(const Direction& other) const;

  bool operator>(const Direction& other) const;

  bool operator<=(const Direction& other) const;

  bool operator>=(const Direction& other) const;

  std::strong_ordering operator<=> (const Direction& other) const;

  Direction& operator=(const Direction& other);

  Direction& operator+=(const Direction& other);

  Direction& operator-=(const Direction& other);

  const Direction operator+(const Direction& other) const;

  const Direction operator-(const Direction& other) const;

  const Direction operator/(const int& other) const;

  const Direction operator*(const int& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator Degrees() const;

  operator Radians() const;

  operator CompassHeading() const;

 private:
  int64_t base_value_;
};

}  // namespace qw::units

#endif  // SRC_DIRECTION_INCLUDE_DIRECTION_H_
