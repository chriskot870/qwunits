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
#include "include/direction.h"

#include <compare>
#include <string>

#include "include/degrees.h"
#include "include/radians.h"
#include "include/compass_heading.h"

using std::string;
using std::strong_ordering;

namespace qw::units {

/*
 * Constructor routines
 */
Direction::Direction() {}

Direction::Direction(int64_t base_value) : base_value_(base_value) {}

bool Direction::operator==(const Direction& other) const {
  bool value = (base_value_ == other.base_value_);

  return value;
}

bool Direction::operator!=(const Direction& other) const {
  bool value = (base_value_ != other.base_value_);

  return value;
}

bool Direction::operator<(const Direction& other) const {
  bool value = (base_value_ < other.base_value_);

  return value;
}

bool Direction::operator>(const Direction& other) const {
  bool value = (base_value_ > other.base_value_);

  return value;
}

bool Direction::operator<=(const Direction& other) const {
  bool value = (base_value_ <= other.base_value_);

  return value;
}

bool Direction::operator>=(const Direction& other) const {
  bool value = (base_value_ >= other.base_value_);

  return value;
}

strong_ordering Direction::operator<=> (const Direction& other) const {
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
Direction& Direction::operator=(const Direction& other) {
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

Direction& Direction::operator+=(const Direction& other) {
  base_value_ += other.base_value_;

  return *this;
}

Direction& Direction::operator-=(const Direction& other) {
  base_value_ -= other.base_value_;

  return *this;
}

/*
 * Arithmetic operations
 */
const Direction Direction::operator+(const Direction& other) const {
  Direction result = *this;

  result.base_value_ += other.base_value_;

  return result;
}

const Direction Direction::operator-(const Direction& other) const {
  Direction result = *this;

  result.base_value_ -= other.base_value_;

  return result;
}

const Direction Direction::operator/(const int& other) const {
  Direction result = *this;

  result.base_value_ /= other;

  return result;
}

/*
 * You can multiply by an integer but the integer has to
 * be the second factor of the multiplication.
 * Speeed * int but not int * Direction
 */
const Direction Direction::operator*(const int& other) const {
  Direction result = *this;

  result.base_value_ *= other;

  return result;
}

/*
 * Intrinsic casting to direction units
 */
Direction::operator Degrees() const {
  Degrees d;
  d.setBase(base_value_);

  return d;
}

Direction::operator Radians() const {
  Radians r;
  r.setBase(base_value_);

  return r;
}

Direction::operator CompassHeading() const {
  CompassHeading ch;
  ch.setBase(base_value_);

  return ch;
}
}  // namespace qw::units
