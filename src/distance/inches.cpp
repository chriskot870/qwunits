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
#include "include/inches.h"

#include <string>

using std::string;

namespace qw::units {

/*
 * Constructor routines
 */
Inches::Inches() {}

Inches::Inches(float temp)
    : Distance(inchesToBase(temp)) {}

Inches::Inches(float temp, string fmt_value)
    : Distance(inchesToBase(temp)), fmt_value_(fmt_value) {}

/*
 * Data manipulation routines
 */
float Inches::value() {
  return baseToInches(base_value_);
}

/*
 * I am using a base of milli-millibars as a base unit
 */
int Inches::inchesToBase(float inch) {
  int value = round(inch * mm_in_inch * base_units_in_mm);

  return value;
}

float Inches::baseToInches(int base) {
  float inches = ((static_cast<float>(base))/mm_in_inch)/base_units_in_mm;

  return inches;
}

void Inches::setBase(int64_t base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Inches::toString() {
  string data = format(fmt::runtime(fmt_value_), value());

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string Inches::toString(string fmt_value) {
  string data = format(fmt::runtime(fmt_value), value());

  return data;
}

/*
 * Set the format for this instance
 */
void Inches::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

}  // namespace qw::units
