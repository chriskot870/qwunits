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

#ifndef SRC_SPEED_INCLUDE_SPEED_H_
#define SRC_SPEED_INCLUDE_SPEED_H_

#include <compare>
#include <string>
#include <string_view>

namespace qw::units {

/*
   * Need to pre-declare these for
   */
class MilesPerHour;
class KilometersPerHour;
class MetersPerSecond;
class Knots;
/*
 * Our speed is based on .001 mile per hour
 */
constexpr int speed_base_conversion_factor = 1000;
constexpr float kMilesPerKilometer = .621371;
constexpr float kKilometersPerMile =
    (1 / kMilesPerKilometer);  // The inverse of above
constexpr float kMpsPerMph = 2.23694;
constexpr float kMphPerMps = (1 / kMpsPerMph);  // The inverse of above
constexpr float kMphPerKnot = 1.15078;
constexpr float kKnotPerMph = (1 / kMphPerKnot);  // The inverse of above.
const std::string_view speed_default_format = "{0:.2f}";

class Speed {
 public:
  friend MilesPerHour;
  friend KilometersPerHour;
  friend MetersPerSecond;
  friend Knots;

  Speed();

  explicit Speed(const int64_t);

  bool operator==(const Speed& other) const;

  bool operator!=(const Speed& other) const;

  bool operator<(const Speed& other) const;

  bool operator>(const Speed& other) const;

  bool operator<=(const Speed& other) const;

  bool operator>=(const Speed& other) const;

  std::strong_ordering operator<=> (const Speed& other) const;

  Speed& operator=(const Speed& other);

  Speed& operator+=(const Speed& other);

  Speed& operator-=(const Speed& other);

  const Speed operator+(const Speed& other) const;

  const Speed operator-(const Speed& other) const;

  const Speed operator/(const int& other) const;

  const Speed operator*(const int& other) const;

  /*
   * Supports implicit casting
   * hence the need for the predeclaration
   */
  operator MilesPerHour() const;

  operator KilometersPerHour() const;

  operator MetersPerSecond() const;

  operator Knots() const;

 private:
  int64_t base_value_;
};

}  // namespace qw::units

#endif  // SRC_SPEED_INCLUDE_SPEED_H_
