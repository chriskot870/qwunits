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
#ifndef SRC_LIB_QW_DEVICES_INCLUDE_QWGPIO_CHIP_H_
#define SRC_LIB_QW_DEVICES_INCLUDE_QWGPIO_CHIP_H_

#include <linux/gpio.h>
#include <expected>
#include <string>
#include <unordered_map>
#include <memory>
#include <regex>

#include "include/qwgpio.h"
#include "include/qwgpio_group.h"

namespace qw::devices {

class QwGpioGroup;  // predeclare

class QwGpioChip {
 public:
  // We define this here as part of class QwGpioChip but make space for it in cpp file
  static std::unordered_map<std::string, std::shared_ptr<QwGpioChip>> chip_list;

  static std::expected<std::shared_ptr<QwGpioChip>, int> factory(std::string device_name);

  int open();

  bool is_open();

  std::expected<struct gpiochip_info, int> getInfo();

  std::expected<struct gpio_v2_line_info, int> getLineInfo(uint offset);

  std::expected<std::shared_ptr<QwGpioGroup>, int> createGroup(
      std::string key, std::shared_ptr<QwGpioChip>, struct gpio_v2_line_request request);

  std::expected<void, int> deleteGroup(std::string key);

  int release(std::string key);

  int close();

  ~QwGpioChip();

 private:
  std::string fpath_ = "";
  int chip_fd_ = QW_GPIO_FD_NOT_OPENED;
  struct gpiochip_info info_ = {};
  std::unordered_map<std::string, QwGpioGroup*> groups_ = {};

  /*
   * make constructor private so another instance can't be made
   */
  QwGpioChip();

  explicit QwGpioChip(std::string fpath);

  /*
   * Delete copy constructor and assignment operator to prevent
   * cretaing a second instance
   */
  QwGpioChip(const QwGpioChip&) = delete;
  QwGpioChip& operator=(const QwGpioChip&) = delete;
};

}  // namespace qw::devices

#endif  // SRC_LIB_QW_DEVICES_INCLUDE_QWGPIO_CHIP_H_
