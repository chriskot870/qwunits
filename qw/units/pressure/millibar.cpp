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

#include "qw/units/pressure/include/millibar.h"

#include <string>

#include "fmt/format.h"

using fmt::format;
using std::string;

namespace qw::units {

/*
 * Constructor routines
 */
Millibar::Millibar() {}

Millibar::Millibar(float mb) : Pressure(millibarToBase(mb)) {}

Millibar::Millibar(float mb, string fmt_value)
    : Pressure(millibarToBase(mb)), fmt_value_(fmt_value) {}

/*
 * Data manpulation routines
 */
float Millibar::value() {
  return baseToMillibar(base_value_);
}

int Millibar::millibarToBase(float mb) {
  int value = round(mb * pressure_base_conversion_factor);

  return value;
}

float Millibar::baseToMillibar(int base) {
  float value = static_cast<float>(base) / pressure_base_conversion_factor;

  return value;
}

void Millibar::setBase(int64_t base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Millibar::toString() {
  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Millibar::toString(string fmt_value) {
  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

void Millibar::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

}  // namespace qw::units
