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

#include <compare>
#include <string>

#include "include/light.h"
#include "include/foot_candle.h"
#include "include/lux.h"
#include "include/klux.h"

using std::string;
using std::strong_ordering;

namespace qw::units {

/*
 * Constructor routines
 */
Light::Light() {}

Light::Light(int64_t base_value) : base_value_(base_value) {}

bool Light::operator==(const Light& other) const {
  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Light::operator!=(const Light& other) const {
  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Light::operator<(const Light& other) const {
  bool value = (base_value_ < other.base_value_);

  return value;
}

bool Light::operator>(const Light& other) const {
  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Light::operator<=(const Light& other) const {
  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Light::operator>=(const Light& other) const {
  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Light::operator<=> (const Light& other) const {
  /*
   * The <=> returns a std::strong_ordering type.
   * Either ::less, ::equal, or ::greater
   */
  strong_ordering value = (base_value_ <=> other.base_value_);

  return value;
}

/*
 * Assignment operators
 */
Light& Light::operator=(const Light& other) {
  /*
   * Guard against self assignement
   */
  if (this == &other) {
    return *this;
  }

  /*
   * This is the one that is to the left of = sign so we want
   * to copy the base_value_ in other to the one in this
   */
  base_value_ = other.base_value_;

  return *this;
}

Light& Light::operator+=(const Light& other) {
  base_value_ += other.base_value_;

  return *this;
}

Light& Light::operator-=(const Light& other) {
  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Light Light::operator+(const Light& other) const {
  Light result = *this;

  result.base_value_ += other.base_value_;

  return result;
}

const Light Light::operator-(const Light& other) const {
  Light result = *this;

  result.base_value_ -= other.base_value_;

  return result;
}

const Light Light::operator/(const int& other) const {
  Light result = *this;

  result.base_value_ /= other;

  return result;
}

/*
 * You can multiply by an integer but the integer has to
 * be the second factor of the multiplication.
 * Light * int but not int * Light
 */
const Light Light::operator*(const int& other) const {
  Light result = *this;

  result.base_value_ *= other;

  return result;
}

/*
 * Intrinsic casting to pressure units
 */
Light::operator Lux() const {
  Lux lux;
  lux.setBase(base_value_);

  return lux;
}

Light::operator Klux() const {
  Klux klux;
  klux.setBase(base_value_);

  return klux;
}


Light::operator FootCandle() const {
  FootCandle fc;
  fc.setBase(base_value_);

  return fc;
}

}  // namespace qw::units
