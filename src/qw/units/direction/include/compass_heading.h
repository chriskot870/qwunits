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

#ifndef SRC_QW_UNITS_DIRECTION_INCLUDE_COMPASS_HEADING_H_
#define SRC_QW_UNITS_DIRECTION_INCLUDE_COMPASS_HEADING_H_

#include <string>
#include <array>
#include <map>

#include "qw/units/direction/include/direction.h"

namespace qw::units {

  enum CompassHeadingType {
    DIRECTION_COMPASS_HEADING_N,
    DIRECTION_COMPASS_HEADING_NNE,
    DIRECTION_COMPASS_HEADING_NE,
    DIRECTION_COMPASS_HEADING_ENE,
    DIRECTION_COMPASS_HEADING_E,
    DIRECTION_COMPASS_HEADING_ESE,
    DIRECTION_COMPASS_HEADING_SE,
    DIRECTION_COMPASS_HEADING_SSE,
    DIRECTION_COMPASS_HEADING_S,
    DIRECTION_COMPASS_HEADING_SSW,
    DIRECTION_COMPASS_HEADING_SW,
    DIRECTION_COMPASS_HEADING_WSW,
    DIRECTION_COMPASS_HEADING_W,
    DIRECTION_COMPASS_HEADING_WNW,
    DIRECTION_COMPASS_HEADING_NW,
    DIRECTION_COMPASS_HEADING_NNW
  };

  const std::map<CompassHeadingType, std::string> compass_heading_labels = {
    {DIRECTION_COMPASS_HEADING_N, "N"},
    {DIRECTION_COMPASS_HEADING_NNE, "NNE"},
    {DIRECTION_COMPASS_HEADING_NE, "NE"},
    {DIRECTION_COMPASS_HEADING_ENE, "ENE"},
    {DIRECTION_COMPASS_HEADING_E, "E"},
    {DIRECTION_COMPASS_HEADING_ESE, "ESE"},
    {DIRECTION_COMPASS_HEADING_SE, "SE"},
    {DIRECTION_COMPASS_HEADING_ESE, "ESE"},
    {DIRECTION_COMPASS_HEADING_S, "S"},
    {DIRECTION_COMPASS_HEADING_SSW, "SSW"},
    {DIRECTION_COMPASS_HEADING_SW, "SW"},
    {DIRECTION_COMPASS_HEADING_WSW, "WSW"},
    {DIRECTION_COMPASS_HEADING_W, "W"},
    {DIRECTION_COMPASS_HEADING_WNW, "WNW"},
    {DIRECTION_COMPASS_HEADING_NW, "NW"},
    {DIRECTION_COMPASS_HEADING_NNW, "NNW"}
  };

class CompassHeading : public Direction {
  friend Direction;

 public:
  CompassHeading();

  explicit CompassHeading(CompassHeadingType compass_heading);

  CompassHeading(CompassHeadingType compass_heading, std::string fmt_value);

  CompassHeadingType value();

  std::string toString();

  std::string toString(std::string format);

  void setFormat(std::string fmt_value);

 private:
  std::string fmt_value_ = direction_default_format.data();

  int64_t compassHeadingToBase(CompassHeadingType heading);

  CompassHeadingType baseToCompassHeading(int64_t base);

  void setBase(int64_t base_value);
};

}  // namespace qw::units

#endif  // SRC_QW_UNITS_DIRECTION_INCLUDE_COMPASS_HEADING_H_
