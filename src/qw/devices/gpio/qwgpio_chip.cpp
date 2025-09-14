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
#include "include/qwgpio_chip.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <expected>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "include/qwgpio_group.h"

using std::expected;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unexpected;
using std::unordered_map;
using std::count;

namespace qw::devices {
// We make space for the static variable defined in QwGpioChip here.
unordered_map<string, shared_ptr<QwGpioChip>> QwGpioChip::chip_list;

QwGpioChip::QwGpioChip() {}

QwGpioChip::QwGpioChip(string fpath) : fpath_(fpath) {}

expected<std::shared_ptr<QwGpioChip>, int> QwGpioChip::factory(
    string device_name) {

  /*
   * Check for a valid device name.
   * We can still test if it ends in a digit also if we want.
   * Maybe when I have time to figure that out.
   */
  if (regex_match(device_name, device_pattern) != true) {
    return unexpected(EINVAL);
  }

  if (QwGpioChip::chip_list.contains(device_name) == true) {
    return QwGpioChip::chip_list[device_name];
  }
  /*
   * I read that making a static variable will only happen once on the first time
   * through this routine.
   * make_shared can't work on a private constructor
   */
  // chip_list[device_name] = std::make_shared<QwGpioChip>(device_name);
  // std::shared_ptr<QwGpioChip> shared_chip_ptr(new QwGpioChip(device_name));
  QwGpioChip::chip_list[device_name] =
      std::shared_ptr<QwGpioChip>(new QwGpioChip(device_name));
  int count = QwGpioChip::chip_list[device_name].use_count();
  /*
   * Now open the file descriptor if it isn't already open
   */
  if (QwGpioChip::chip_list[device_name]->is_open() != true) {
    int result = QwGpioChip::chip_list[device_name]->open();
    // If open failed then return the error
    if (result != 0) {
      return unexpected(result);
    }
  }

  return QwGpioChip::chip_list[device_name];
}

int QwGpioChip::open() {
  if (fpath_ == "") {
    return ENOENT;
  }
  /*
   * Try and open the file
   */
  int result = ::open(fpath_.c_str(), O_RDWR);
  if (result < 0) {
    return result;
  }

  chip_fd_ = result;
  /*
   * If we get here we were successful
   */
  return 0;
}

bool QwGpioChip::is_open() {
  if (fpath_ == "") {
    return false;
  }

  if (chip_fd_ < 0) {
    return false;
  }

  return true;
}

expected<struct gpiochip_info, int> QwGpioChip::getInfo() {
  if (chip_fd_ < 0) {
    unexpected(ENXIO);
  }
  struct gpiochip_info info;
  /*
   * GPIO_GET_CHIPINFO_IOCTL _IOR(0xB4, 0x01, struct gpiochip_info)
   */
  int result = ::ioctl(chip_fd_, GPIO_GET_CHIPINFO_IOCTL, &info);
  if (result != 0) {
    return unexpected(result);
  }
  info_ = info;
  /*
   * I got a successful ioctl call so return the value
   */
  return info_;
}

int QwGpioChip::close() {
  if (chip_fd_ < 0) {
    return 0;
  }

  ::close(chip_fd_);
  chip_fd_ = QW_GPIO_FD_NOT_OPENED;

  return 0;
}

expected<struct gpio_v2_line_info, int> QwGpioChip::getLineInfo(uint offset) {
  if (chip_fd_ < 0) {
    return unexpected(ENODEV);
  }
  if (offset >= info_.lines) {
    return unexpected(ERANGE);
  }

  /*
   * Zero out a line info structure
   */
  struct gpio_v2_line_info line_info = {};
  // Now set the offset of interest
  line_info.offset = offset;
  // Now make the ioctl to update line_info
  int result = ::ioctl(chip_fd_, GPIO_V2_GET_LINEINFO_IOCTL, &line_info);
  if (result != 0) {
    return unexpected(result);
  }

  return line_info;
}

expected<shared_ptr<QwGpioGroup>, int> QwGpioChip::createGroup(
    string key, std::shared_ptr<QwGpioChip> chip_parent,
    struct gpio_v2_line_request request) {
  // Do sanity checks
  if ((request.num_lines > GPIO_V2_LINES_MAX) || (request.num_lines < 1)) {
    return unexpected(ERANGE);
  }

  if (groups_.contains(key) == true) {
    return unexpected(EINVAL);
  }

  /*
   * Walk through and make sure each offset is not already in use
   */
  struct gpio_v2_line_info line_info;
  for (int n = 0; n < request.num_lines; n++) {
    /*
     * Make sure there isn't a duplicate entry
     */
    if (count(request.offsets, request.offsets + request.num_lines, request.offsets[n]) != 1) {
      return unexpected(EINVAL);
    }
    /*
     * Make sure the line isn't already used by somne other group
     */
    auto ex_line_info = getLineInfo(request.offsets[n]);
    if (ex_line_info.has_value() != true) {
      return unexpected(EINVAL);
    }
    line_info = ex_line_info.value();
    if ((line_info.flags & GPIO_V2_LINE_FLAG_USED) != 0) {
      return unexpected(EINVAL);
    }
  }

  // Sanity checks are done so create the group
  int retval = ::ioctl(chip_fd_, GPIO_V2_GET_LINE_IOCTL, &request);
  if (retval != 0) {
    return unexpected(errno);
  }

  /*
   * Get the chip to be sent to the group
   */
  auto ex_chip = QwGpioChip::factory(fpath_);
  if (ex_chip.has_value() != true) {
    return unexpected(
        ENODEV);  // May be able to come up with a better error value
  }

  /*
   * We can't use make_share() on a class that has a private constructor
   */
  shared_ptr<QwGpioGroup> gptr = std::shared_ptr<QwGpioGroup>(
      new QwGpioGroup(key, ex_chip.value(), request));
  /*
   * Store the raw pointer so the entry in the table doesn't increment the use_count
   * of the shared_ptr.
   */
  groups_[key] = gptr.get();
  /*
   * Return a shared_ptr
   * All references outside the gpio_chip structure are shared pointers.
   * When the last gpio_group shared_ptr goes out of scope the gpio_group
   * will call it's destructor. When the last gpio_group shared ptr goes
   * into it's destructor, the destructor will call deleteGroup and the
   * entry for that key will be erased. This will clear the ptr memory but
   * not the underlying object. We leave it to the gpio_group's
   * destructor to perform the delete of the underlying object.
   */

  return gptr;
}

expected<void, int> QwGpioChip::deleteGroup(string key) {
  if (groups_.contains(key) != true) {
    return {};  // if the key doesn't exist return a success
  }
  /*
   * This only gets called when all QwGpioGroup shared pointers
   * and the QwGpioGroup destructor calls this routine.
   * Remove the key entry on groups_. This just removes the raw_ptr
   * to the QwGpioGroup not the QwGpioGroup itself. We let
   * the destructor in QwGpiioGroup do the actual delete of the
   * QwGpioGroup object.
   */
  groups_.erase(key);

  return {};
}

QwGpioChip::~QwGpioChip() {
  /*
   * If the QwGpioChip is deleted then all the QwGpioGroups
   * created from it will essentially be broken because the
   * file descriptors will be closed.
   * 
   * Walk through the groups and close all the files
   * Then erase them from the groups_
   */
  for (const auto& [key, group] : groups_) {
    if (group->getFd() >= 0) {
      group->close();
    }
    groups_.erase(key);
  }
  /*
   * If the chip fd is open then close it.
   * Call this object's close and use this-> to make that clear.
   */
  if (chip_fd_ >= 0) {
    this->close();
  }
}

}  // namespace qw::devices
