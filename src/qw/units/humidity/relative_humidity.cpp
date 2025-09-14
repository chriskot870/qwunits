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

#include "qw/units/humidity/include/relative_humidity.h"

#include <expected>
#include <string>

#include "fmt/format.h"

using fmt::format;
using std::string;
using std::strong_ordering;

namespace qw::units {

/*
 * Constructor routines
 */
RelativeHumidity::RelativeHumidity() {}

RelativeHumidity::RelativeHumidity(float rh) {
  base_value_ = round(rh * rh_base_conversion_factor);

  return;
}

RelativeHumidity::RelativeHumidity(float rh, string fmt_value) {
  base_value_ = round(rh * rh_base_conversion_factor);

  fmt_value_ = fmt_value;

  return;
}

/*
 * Data manipulation routnes
 */
float RelativeHumidity::value() {
  float value = (static_cast<float>(base_value_)/ rh_base_conversion_factor);

  return value;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string RelativeHumidity::toString() {
  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string RelativeHumidity::toString(string fmt_value) {
  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

void RelativeHumidity::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

bool RelativeHumidity::operator==(const RelativeHumidity& other) const {
  bool value = (base_value_ == other.base_value_);

  return value;
}

bool RelativeHumidity::operator!=(const RelativeHumidity& other) const {
  bool value = (base_value_ != other.base_value_);

  return value;
}

bool RelativeHumidity::operator<(const RelativeHumidity& other) const {
  bool value = (base_value_ < other.base_value_);

  return value;
}

bool RelativeHumidity::operator>(const RelativeHumidity& other) const {
  bool value = (base_value_ > other.base_value_);

  return value;
}

bool RelativeHumidity::operator<=(const RelativeHumidity& other) const {
  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool RelativeHumidity::operator>=(const RelativeHumidity& other) const {
  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering RelativeHumidity::operator<=>
    (const RelativeHumidity& other) const {
  strong_ordering value = (base_value_ <=> other.base_value_);

  return value;
}

RelativeHumidity& RelativeHumidity::operator=(const RelativeHumidity& other) {
  /*
   * Guard against self assignement
   */
  if (this == &other) {
    return *this;
  }

  /*
   * This is the one that is to the left of = sign so we eant
   * to copy the base_value_ in other to the one in this
   */
  base_value_ = other.base_value_;

  return *this;
}

RelativeHumidity& RelativeHumidity::operator+=(const RelativeHumidity& other) {
  base_value_ += other.base_value_;

  return *this;
}

RelativeHumidity& RelativeHumidity::operator-=(const RelativeHumidity& other) {
  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 * it is unclear that these make sense. 
 */
const RelativeHumidity RelativeHumidity::operator+(
    const RelativeHumidity& other) const {

  RelativeHumidity result = *this;

  result += other;

  return result;
}

const RelativeHumidity RelativeHumidity::operator-(
    const RelativeHumidity& other) const {

  RelativeHumidity result = *this;

  result -= other;

  return result;
}

}  // namespace qw::units
