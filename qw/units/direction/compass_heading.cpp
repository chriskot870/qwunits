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
#include "qw/units/direction/include/compass_heading.h"

#include <algorithm>
#include <iterator>
#include <string>

#include "fmt/format.h"

#include "qw/units/direction/include/direction.h"

using std::string;
using fmt::format;

namespace qw::units {

/*
 * Constructor routines
 */
CompassHeading::CompassHeading() {}

CompassHeading::CompassHeading(CompassHeadingType compass_heading)
    : Direction(compassHeadingToBase(compass_heading)) {}

CompassHeading::CompassHeading(CompassHeadingType compass_heading,
                               string fmt_value)
    : Direction(compassHeadingToBase(compass_heading)), fmt_value_(fmt_value) {}

/*
 * Data manipulation routines
 */
CompassHeadingType CompassHeading::value() {
  return baseToCompassHeading(base_value_);
}

int64_t CompassHeading::compassHeadingToBase(
    CompassHeadingType compass_heading) {
  /*
   * 
   */
  int64_t value =
      round((compass_heading_range * static_cast<int>(compass_heading)) *
            degrees_base_conversion_factor);

  return value;
}

CompassHeadingType CompassHeading::baseToCompassHeading(int64_t base) {
  /*
   * We divide a circle into compass_heading_range*2 sections. The
   * sections will be either side of a compass heading. Each sector
   * will be either side of the compass headings. So, we return
   * the compass heading if the sector as on either side of the
   * compass heading.
   */
  int sector = (base_value_ % 360) / (compass_heading_range * 2);
  CompassHeadingType value;

  switch (sector) {
    case 0:
    case 31:
      value = DIRECTION_COMPASS_HEADING_N;
      break;

    case 1:
    case 2:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_NNE
                          : DIRECTION_COMPASS_HEADING_NNW;
      break;

    case 3:
    case 4:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_NE
                          : DIRECTION_COMPASS_HEADING_NW;
      break;

    case 5:
    case 6:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_ENE
                          : DIRECTION_COMPASS_HEADING_WNW;
      break;

    case 7:
    case 8:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_E
                          : DIRECTION_COMPASS_HEADING_W;
      break;

    case 9:
    case 10:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_ESE
                          : DIRECTION_COMPASS_HEADING_WSW;
      break;

    case 11:
    case 12:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_SE
                          : DIRECTION_COMPASS_HEADING_SW;
      break;

    case 13:
    case 14:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_ESE
                          : DIRECTION_COMPASS_HEADING_WSW;
      break;

    case 15:
    case 16:
      value = DIRECTION_COMPASS_HEADING_S;
      break;

    case 17:
    case 18:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_SSW
                          : DIRECTION_COMPASS_HEADING_SSE;
      break;

    case 19:
    case 20:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_SW
                          : DIRECTION_COMPASS_HEADING_SE;
      break;

    case 21:
    case 22:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_WSW
                          : DIRECTION_COMPASS_HEADING_ESE;
      break;

    case 23:
    case 24:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_W
                          : DIRECTION_COMPASS_HEADING_E;
      break;

    case 25:
    case 26:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_WNW
                          : DIRECTION_COMPASS_HEADING_ENE;
      break;

    case 27:
    case 28:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_NW
                          : DIRECTION_COMPASS_HEADING_NE;
      break;

    case 29:
    case 30:
      value = sector >= 0 ? DIRECTION_COMPASS_HEADING_NNW
                          : DIRECTION_COMPASS_HEADING_NNE;
      break;
  }

  return value;
}

void CompassHeading::setBase(int64_t base_value) {
  base_value_ = base_value;

  return;
}

/*
 * Use the default format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string CompassHeading::toString() {
  string data = format(fmt::runtime(fmt_value_), compass_heading_labels.at(value()));

  return data;
}

/*
 * Use the provided format
 * Use "fmt" so it doesn't get confused with fmt::format
 */
string CompassHeading::toString(string fmt_value) {
  string data = format(fmt::runtime(fmt_value), compass_heading_labels.at(value()));

  return data;
}

/*
 * Set the format to use for this instanace
 */
void CompassHeading::setFormat(string fmt_value) {
  fmt_value_ = fmt_value;

  return;
}

}  // namespace qw::units
