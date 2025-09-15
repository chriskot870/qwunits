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
#ifndef QW_DEVICES_GPIO_INCLUDE_QWGPIO_H_
#define QW_DEVICES_GPIO_INCLUDE_QWGPIO_H_

#include <map>
#include <string>
#include <regex>

namespace qw::devices {

constexpr int QW_GPIO_FD_NOT_OPENED = -1;
const std::regex device_pattern("/dev/gpiochip\\d+");

enum RPI_PINOUT_TYPES {
  RPI_BASE_PINOUT
};

/*
 * The string for model_pinout_types should match
 * cat /proc/device-tree/model
 */
const std::map<std::string, RPI_PINOUT_TYPES> model_pinout_types = {
  {"Raspberry Pi Zero 2 W Rev 1.0", RPI_BASE_PINOUT},
};

const std::map<RPI_PINOUT_TYPES, std::map<int, std::string>> pinout_options = {
  {RPI_BASE_PINOUT, {
      {1, "3.3V"},
      {2, "5V"},
      {3, "GPIO2"},
      {4, "5V"},
      {5, "GPIO3"},
      {6, "GND"},
      {7, "GPIO4"},
      {8, "GPIO14"},
      {9, "GND"},
      {10, "GPIO15"},
      {11, "GPIO17"},
      {12, "GPIO18"},
      {13, "GPIO27"},
      {14, "GND"},
      {15, "GPIO22"},
      {16, "GPIO23"},
      {17, "3.3V"},
      {18, "GPIO24"},
      {19, "GPIO10"},
      {20, "GND"},
      {21, "GPIO9"},
      {22, "GPIO25"},
      {23, "GPIO11"},
      {24, "GPIO8"},
      {25, "GND"},
      {26, "GPIO7"},
      {27, "GPIO0"},
      {28, "GPIO1"},
      {29, "GPIO5"},
      {30, "GND"},
      {31, "GPIO6"},
      {32, "GPIO12"},
      {33, "GPIO13"},
      {34, "GND"},
      {35, "GPIO19"},
      {36, "GPIO16"},
      {37, "GPIO26"},
      {38, "GPIO20"},
      {39, "GND"},
      {40, "GPIO21"}
    }
  }
};

}  // namespace qw::devices

#endif  // SRC_LIB_QW_DEVICES_INCLUDE_QWGPIO_H_
