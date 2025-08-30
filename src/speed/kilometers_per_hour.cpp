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
#include "include/kilometers_per_hour.h"

#include <compare>
#include <string>

#include "fmt/format.h"

using fmt::format;
using std::string;

namespace qw::units {

/*
 * Constructor routines
 */
KilometersPerHour::KilometersPerHour() {}

KilometersPerHour::KilometersPerHour(float kph)
    : Speed(kilometersPerHourToBase(kph)) {}

KilometersPerHour::KilometersPerHour(float kph, string fmt_value)
    : Speed(kilometersPerHourToBase(kph)), fmt_value_(fmt_value) {}

/*
 * Data manipulation routines
 */
float KilometersPerHour::value() {
  return baseToKilometersPerHour(base_value_);
}

/*
 * For every mile per hour there are 100 base units.
 * So, convert kph to mph than apply the base conversion factor.
 */
int64_t KilometersPerHour::kilometersPerHourToBase(float kph) {
  int64_t value = round((kph * kMilesPerKilometer) * speed_base_conversion_factor);

  return value;
}

/*
 * Divide the base level by speed base conversion factor to get to miles.
 * Then convert miles to kilometers.
 */
float KilometersPerHour::baseToKilometersPerHour(int base) {
  float mph = (static_cast<float>(base) / speed_base_conversion_factor) * kKilometersPerMile;

  return mph;
}

void KilometersPerHour::setBase(int64_t base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Use the default format
 */
string KilometersPerHour::toString() {
  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format instead of one in private variable
 */
string KilometersPerHour::toString(string fmt_value) {
  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void KilometersPerHour::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

}  // namespace qw::units
