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

#include "qw/units/light/include/lux.h"

#include <string>

using std::string;

namespace qw::units {

/*
 * Constructor routines
 */
Lux::Lux() {}

Lux::Lux(float temp)
    : Light(luxToBase(temp)) {}

Lux::Lux(float temp, string fmt_value)
    : Light(luxToBase(temp)), fmt_value_(fmt_value) {}

/*
 * Data manipulation routines
 */
float Lux::value() {
  return baseToLux(base_value_);
}

/*
 * I am using a base of .01 millimeters as a base unit
 */
int Lux::luxToBase(float lx) {
  int value = round(lx * base_units_in_lux);

  return value;
}

float Lux::baseToLux(int base) {
  float lx = (static_cast<float>(base))/base_units_in_lux;

  return lx;
}

void Lux::setBase(int64_t base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Lux::toString() {
  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Lux::toString(string fmt_value) {
  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void Lux::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

}  // namespace qw::units
